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
#include <boost/timer/timer.hpp>
#include "ElementsKernel/ProgramHeaders.h"
#include "ElementsKernel/Auxiliary.h"
#include "SEImplementation/PythonConfig/PythonInterpreter.h"
#include "SEUtils/Python.h"
#include "Pyston/AST/Placeholder.h"
#include "Pyston/PrettyPrinter.h"

using boost::program_options::options_description;
using boost::program_options::variable_value;
using namespace Pyston;
namespace py = boost::python;


static Elements::Logging logger = Elements::Logging::getLogger("Example");

class Example : public Elements::Program {

public:

  options_description defineSpecificProgramOptions() override {
    options_description options{};
    return options;
  }

  static void evalExamples() {
    SourceXtractor::GILStateEnsure ensure;

    py::object module = py::import("sourcextractor.config");
    py::dict evaluate = py::extract<py::dict>(module.attr("evaluate"));
    py::list keys = evaluate.keys();

    for (ssize_t i = 0; i < py::len(keys); ++i) {
      size_t nparams = py::extract<size_t>(keys[i]);
      logger.info() << "Callable with " << nparams << " parameters";

      try {
        py::object pyston = py::import("libPyston");
        py::list arguments;
        std::vector<double> values;

        for (size_t arg = 0; arg < nparams; ++arg) {
          auto placeholder = std::make_shared<Placeholder<double>>(arg);
          arguments.append(placeholder);
          values.emplace_back(::drand48());
          *placeholder = values.back();
        }

        py::object func = evaluate[nparams];
        py::object ast = func(*py::tuple(arguments));
        std::shared_ptr<Node<double>> node = py::extract<std::shared_ptr<Node<double>>>(ast);
        PrettyPrinter printer;
        node->visit(printer);
        logger.info() << "\n" << printer.str();

        const int REPEATS = 1000;

        // Evaluate python
        logger.info() << "Timing Python call, " << REPEATS << " times";
        boost::timer::cpu_timer timer;
        for (int j = 0; j < REPEATS; j++) {
          func(*py::tuple(values));
        }
        timer.stop();
        logger.info() << '\t' << timer.elapsed().wall / float(REPEATS) << " ns / call";

        // Evaluate direct
        logger.info() << "Timing AST call, " << REPEATS << " times";
        timer.start();
        for (int j = 0; j < REPEATS; j++) {
          node->eval();
        }
        timer.stop();
        logger.info() << '\t' << timer.elapsed().wall / float(REPEATS) << " ns / call";
      }
      catch (const py::error_already_set&) {
        SourceXtractor::pyToElementsException(logger);
      }
    }
  }

  Elements::ExitCode mainMethod(std::map<std::string, variable_value>&) override {
    logger.info("#");
    logger.info("# Entering mainMethod()");
    logger.info("#");

    auto pyfile = Elements::getAuxiliaryPath("example.py");
    auto& interpreter = SourceXtractor::PythonInterpreter::getSingleton();
    interpreter.runFile(pyfile.native(), {});

    evalExamples();

    logger.info("#");
    logger.info("# Exiting mainMethod()");
    logger.info("#");

    return Elements::ExitCode::OK;
  }

};

MAIN_FOR(Example)



