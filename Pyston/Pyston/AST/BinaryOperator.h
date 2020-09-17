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

#ifndef PYSTON_BINARYOPERATOR_H
#define PYSTON_BINARYOPERATOR_H

#include "Node.h"
#include <functional>
#include <memory>

namespace Pyston {

template<typename R, typename T>
class BinaryOperator : public Node<R> {
public:
  BinaryOperator(const std::shared_ptr<Node<T>>& lval, const std::shared_ptr<Node<T>>& rval,
                 std::function<R(T, T)> functor, const std::string& repr)
    : m_lval{lval}, m_rval{rval}, m_functor{functor}, m_repr{repr} {}

  std::string repr() const override {
    return std::string("(") + m_lval->repr() + m_repr + m_rval->repr() + ")";
  }

  R eval() const final {
    return m_functor(m_lval->eval(), m_rval->eval());
  }

private:
  std::shared_ptr<Node<T>> m_lval, m_rval;
  std::function<R(T, T)> m_functor;
  std::string m_repr;
};

template<typename R, typename T>
class BinaryOperatorFactory {
public:
  BinaryOperatorFactory(std::function<R(T, T)> functor, const std::string& repr, bool reverse=false)
    : m_functor{functor}, m_repr{repr}, m_reverse{reverse} {}

  std::shared_ptr<Node<R>> operator()(const std::shared_ptr<Node<T>>& left, const std::shared_ptr<Node<T>>& right) const {
    if (m_reverse)
      return std::make_shared<BinaryOperator<R, T>>(right, left, m_functor, m_repr);
    else
      return std::make_shared<BinaryOperator<R, T>>(left, right, m_functor, m_repr);
  }

private:
  std::function<T(T, T)> m_functor;
  std::string m_repr;
  bool m_reverse;
};

} // end of namespace Pyston

#endif //PYSTON_BINARYOPERATOR_H
