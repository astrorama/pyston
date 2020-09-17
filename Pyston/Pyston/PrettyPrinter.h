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

#ifndef PYSTON_PRETTYPRINTER_H
#define PYSTON_PRETTYPRINTER_H

#include "AST/Node.h"
#include <sstream>

namespace Pyston {

class PrettyPrinter: public Visitor {
public:
  PrettyPrinter(): m_indent{0} {}

  void enter(const NodeBase *node) override {
    m_stream << std::string(m_indent, '\t') << node->repr() << std::endl;
    ++m_indent;
  }

  void exit(const NodeBase *) override {
    --m_indent;
  }

  std::string str() const {
    return m_stream.str();
  }

private:
  unsigned m_indent;
  std::stringstream m_stream;
};

}

#endif //PYSTON_PRETTYPRINTER_H
