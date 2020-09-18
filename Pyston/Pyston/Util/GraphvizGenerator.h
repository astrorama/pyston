/**
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
 */

#ifndef PYSTON_GRAPHVIZGENERATOR_H
#define PYSTON_GRAPHVIZGENERATOR_H

#include "Pyston/AST/Node.h"
#include <boost/algorithm/string.hpp>
#include <sstream>

namespace Pyston {

/**
 * Concrete implementation of the Visitor class for the computing trees.
 * It will generate a string representing the graph in a format compatible with graphviz.
 */
class GraphvizGenerator : public Visitor {
public:
  /**
   * Constructor
   * @param label
   *    Name of the whole graph
   */
  GraphvizGenerator(const std::string& label): m_unique_id{0} {
    m_stream << "digraph G {" << std::endl
             << "\tlabel=\"" << escape(label) << "\"" << std::endl;
  }

  /**
   * @copydoc Visitor::enter
   */
  void enter(const NodeBase *node) override {
    m_stream << "\t" << '"' << m_unique_id << '"'
             << " [label=\"" << escape(node->repr()) << "\"];"
             << std::endl;
    if (!m_stack.empty()) {
      m_stream << "\t\"" << m_stack.back() << '"' << " -> \"" << m_unique_id << "\"" << std::endl;
    }
    m_stack.push_back(m_unique_id);
    ++m_unique_id;
  }

  /**
   * @copydoc Visitor::exit
   */
  void exit(const NodeBase *) override {
    m_stack.pop_back();
  }

  /**
   * @return
   *    The graphviz representation of the visited graph
   */
  std::string str() const {
    return m_stream.str() + "}";
  }

private:
  int64_t m_unique_id;
  std::stringstream m_stream;
  std::list<int64_t> m_stack;

  static std::string escape(const std::string& str) {
    return boost::replace_all_copy(str, "\"", "\\\"");
  }
};

}

#endif //PYSTON_GRAPHVIZGENERATOR_H
