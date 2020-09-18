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

#include "AST/Node.h"
#include <boost/algorithm/string.hpp>
#include <sstream>

namespace Pyston {

class GraphvizGenerator : public Visitor {
public:
  GraphvizGenerator(const std::string& label) {
    m_stream << "digraph G {" << std::endl
             << "\tlabel=\"" << escape(label) << "\"" << std::endl;
  }

  void enter(const NodeBase *node) override {
    m_stream << "\t" << '"' << node << '"'
             << " [label=\"" << escape(node->repr()) << "\"];"
             << std::endl;
    if (!m_stack.empty()) {
      m_stream << "\t\"" << m_stack.back() << '"' << " -> \"" << node << "\"" << std::endl;
    }
    m_stack.push_back(node);
  }

  void exit(const NodeBase *) override {
    m_stack.pop_back();
  }

  std::string str() const {
    return m_stream.str() + "}";
  }

private:
  std::stringstream m_stream;
  std::list<const NodeBase *> m_stack;

  static std::string escape(const std::string& str) {
    return boost::replace_all_copy(str, "\"", "\\\"");
  }
};

}

#endif //PYSTON_GRAPHVIZGENERATOR_H
