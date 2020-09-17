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

#ifndef PYSTON_UNARYOPERATOR_H
#define PYSTON_UNARYOPERATOR_H

#include "Node.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace Pyston {

template<typename R, typename T>
class UnaryOperator : public Node<R> {
public:
  UnaryOperator(const std::shared_ptr<Node<T>>& node, std::function<T(T)> functor, const std::string&repr)
    : m_node{node}, m_functor{functor}, m_repr{repr} {}

  std::string repr() const override {
    return m_repr;
  }

  R eval() const final {
    return m_functor(m_node->eval());
  }

  void visit(Visitor& visitor) const override {
    visitor.enter(this);
    m_node->visit(visitor);
    visitor.exit(this);
  }

private:
  std::shared_ptr<Node<T>> m_node;
  std::function<R(T)> m_functor;
  std::string m_repr;
};

template<typename R, typename T>
class UnaryOperatorFactory {
public:
  UnaryOperatorFactory(std::function<T(T)> functor, const std::string& repr)
    : m_functor{functor}, m_repr{repr} {}

  std::shared_ptr<Node<R>> operator() (const std::shared_ptr<Node<T>>& node) const {
    return std::make_shared<UnaryOperator<R, T>>(node, m_functor, m_repr);
  }

private:
  std::function<T(T)> m_functor;
  std::string m_repr;
};

} // end of namespace Pyston

#endif //PYSTON_UNARYOPERATOR_H
