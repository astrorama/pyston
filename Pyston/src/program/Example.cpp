/**
 *
 * @copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include <map>
#include <string>

#include <boost/program_options.hpp>
#include <boost/python.hpp>
#include <boost/thread.hpp>
#include <boost/timer/timer.hpp>
#include "ElementsKernel/ProgramHeaders.h"
#include "ElementsKernel/Auxiliary.h"
#include "SEImplementation/PythonConfig/PythonInterpreter.h"
#include "SEUtils/Python.h"
#include "Pyston/AST/Placeholder.h"
#include "Pyston/GraphvizGenerator.h"

using namespace Pyston;
namespace po = boost::program_options;
namespace fs = boost::filesystem;
namespace py = boost::python;


static Elements::Logging logger = Elements::Logging::getLogger("Example");

class Example : public Elements::Program {

private:
  int m_threads, m_repeats;
  fs::path m_dot_file;

public:

  po::options_description defineSpecificProgramOptions() override {
    po::options_description options{"Pyston example options"};
    options.add_options()
      ("no-threads", po::value<int>()->default_value(1),
       "Number of threads")
      ("repeats", po::value<int>()->default_value(50000),
       "Number of iterations inside the timing block")
      ("file", po::value<std::string>()->default_value("example.py"),
       "Python file to run")
      ("dot-file", po::value<std::string>(),
       "Generate a graphviz dot file with the computing graph (prefix)");

    return options;
  }

  void generateGraphviz(Node<double>& node, int nparams) {
    if (m_dot_file.empty())
      return;

    auto full_name = m_dot_file.native() + "." + std::to_string(nparams);

    logger.info() << "Generating " << full_name;

    std::ofstream out(full_name.c_str());
    GraphvizGenerator generator(std::to_string(nparams));
    node.visit(generator);
    out << generator.str();
  }

  /**
   * Extract callables from Python, both Python and "compiled"
   */
  std::map<int, std::pair<py::object, std::shared_ptr<Node<double>>>> getFunctions() {
    std::map<int, std::pair<py::object, std::shared_ptr<Node<double>>>> calls;

    SourceXtractor::GILStateEnsure ensure;

    py::object pyston = py::import("libPyston");
    py::object module = py::import("sourcextractor.config");
    py::dict evaluate = py::extract<py::dict>(module.attr("evaluate"));
    py::list keys = evaluate.keys();

    for (int i = 0; i < py::len(keys); ++i) {
      int nparams = py::extract<int>(keys[i]);
      logger.info() << "Found callable with " << nparams << " parameters";

      try {
        // Setup placeholders
        py::list placeholders;
        for (int arg = 0; arg < nparams; ++arg) {
          std::string var_name(1, 'a' + arg);
          auto placeholder = std::make_shared<Placeholder<double>>(var_name);
          placeholders.append(placeholder);
        }

        // Get the function
        py::object func = evaluate[nparams];

        // Trigger a build of the tree calling with the placeholders
        py::object comp_tree = func(*py::tuple(placeholders));
        std::shared_ptr<Node<double>> node = py::extract<std::shared_ptr<Node<double>>>(comp_tree);

        // Generate graphviz if needed
        generateGraphviz(*node, nparams);

        // Store
        calls[nparams] = std::make_pair(func, node);
      }
      catch (const py::error_already_set&) {
        SourceXtractor::pyToElementsException(logger);
      }
    }

    return calls;
  }

  std::pair<std::vector<double>, Arguments> createParameters(int n) {
    std::pair<std::vector<double>, Arguments> result;
    for (int i = 0; i < n; ++i) {
      double value = ::drand48() * 100;
      result.first.push_back(value);
      result.second[std::string(1, 'a' + i)] = value;
    }
    return result;
  }

  void runPython(boost::python::object func, const std::vector<double>& args) {
    for (int j = 0; j < m_repeats; ++j) {
      SourceXtractor::GILStateEnsure ensure;
      func(*py::tuple(args));
    }
  }

  void runCpp(const std::shared_ptr<Node<double>>& node, const Arguments& args) {
    for (int j = 0; j < m_repeats; ++j) {
      node->eval(args);
    }
  }

  std::vector<double> measure(std::function<void(void)> func) {
    std::vector<double> measures;

    for (int nthreads = 1; nthreads <= m_threads; ++nthreads) {
      boost::thread_group thread_group;
      boost::timer::cpu_timer timer;
      for (int n = 0; n < nthreads; ++n) {
        thread_group.create_thread(func);
      }
      thread_group.join_all();
      timer.stop();

      auto calls_per_ns = (nthreads * m_repeats) / static_cast<double>(timer.elapsed().wall);
      auto calls_per_sec = calls_per_ns * 1e9;
      measures.emplace_back(calls_per_sec);
    }

    return measures;
  }

  void evalExamples() {
    auto callables = getFunctions();

    std::cout << "Method,Arguments,";
    for (int nthread = 1; nthread <= m_threads; ++nthread) {
      std::cout << nthread << ",";
    }
    std::cout << std::endl;

    for (auto& pair : callables) {
      logger.info() << "Timing calls with " << pair.first << " parameters";

      auto params = createParameters(pair.first);
      auto pyFunc = boost::bind(&Example::runPython, this, pair.second.first, params.first);
      auto cppFunc = boost::bind(&Example::runCpp, this, pair.second.second, params.second);

      // Python
      auto measurements = measure(pyFunc);
      std::cout << "Python," << pair.first << ",";
      for (int nthread = 1; nthread <= m_threads; ++nthread) {
        std::cout << std::fixed << std::setw(15) << std::setprecision(2)
                  << measurements[nthread - 1] << ",";
      }
      std::cout << std::endl;

      // Pyston
      measurements = measure(cppFunc);
      std::cout << "Pyston," << pair.first << ",";
      for (int nthread = 1; nthread <= m_threads; ++nthread) {
        std::cout << std::fixed << std::setw(15) << std::setprecision(2)
                  << measurements[nthread - 1] << ",";
      }
      std::cout << std::endl;
    }
  }

  Elements::ExitCode mainMethod(std::map<std::string, po::variable_value>& args) override {
    // Options
    m_threads = args.at("no-threads").as<int>();
    m_repeats = args.at("repeats").as<int>();
    if (args.count("dot-file"))
      m_dot_file = args.at("dot-file").as<std::string>();

    // Setup the python part
    auto& interpreter = SourceXtractor::PythonInterpreter::getSingleton();
    fs::path pyfile = args.at("file").as<std::string>();
    if (!fs::exists(pyfile)) {
      pyfile = Elements::getAuxiliaryPath(pyfile);
    }
    interpreter.runFile(pyfile.native(), {});

    // Evaluate calls
    evalExamples();

    return Elements::ExitCode::OK;
  }

};

MAIN_FOR(Example)
