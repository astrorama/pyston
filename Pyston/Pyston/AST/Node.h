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

#ifndef PYSTON_NODE_H
#define PYSTON_NODE_H

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <boost/variant.hpp>

namespace Pyston {

class Visitor;

class NodeBase {
public:
  virtual ~NodeBase() = default;

  virtual std::string repr() const = 0;

  virtual void visit(Visitor&) const = 0;
};

using Value = boost::variant<bool, int64_t, double>;
using Arguments = std::map<std::string, Value>;

template<typename T>
class Node : public NodeBase {
public:
  virtual ~Node() = default;

  virtual T eval(const Arguments&) const = 0;
};

class Visitor {
public:
  virtual void enter(const NodeBase *) = 0;

  virtual void exit(const NodeBase *) = 0;
};

} // end of namespace Pyston

#endif //PYSTON_NODE_H
