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

/**
 * Template class for binary operators
 * @tparam R
 *  Operator return type
 * @tparam T
 *  Operator parameter type
 */
template<typename R, typename T>
class BinaryOperator : public Node<R> {
public:

  /**
   * Constructor
   * @param lval
   *    Value to the left of the operator
   * @param rval
   *    Value to the right of the operator
   * @param functor
   *    Implements the operator
   * @param repr
   *    Human readable representation of the functor (i.e. +, >, -, ...)
   */
  BinaryOperator(const std::shared_ptr<Node<T>>& lval, const std::shared_ptr<Node<T>>& rval,
                 std::function<R(T, T)> functor, const std::string& repr)
    : m_lval{lval}, m_rval{rval}, m_functor{functor}, m_repr{repr} {}

  /**
   * @copydoc Node::repr
   */
  std::string repr() const final {
    return m_repr;
  }

  /**
   * @copydoc Node::repr
   */
  R eval(const Context& context, const Arguments& args) const final {
    return m_functor(m_lval->eval(context, args), m_rval->eval(context, args));
  }

  /**
   * @copydoc Node::visit
   */
  void visit(Visitor& visitor) const final {
    visitor.enter(this);
    m_lval->visit(visitor);
    m_rval->visit(visitor);
    visitor.exit(this);
  }

private:
  std::shared_ptr<Node<T>> m_lval, m_rval;
  std::function<R(T, T)> m_functor;
  std::string m_repr;
};

/**
 * When a method implemented by a BinaryOperator is called in Python, we expect to
 * *create* a BinaryOperator node. This factory is a functor that is called when an
 * operator is called, builds up the corresponding BinaryOperator, and return it instead
 * of what normally would be a numerical evaluation
 * @tparam R
 *  Type corresponding to the created new Node
 * @tparam T
 *  Type corresponding to the received Nodes
 */
template<typename R, typename T>
class BinaryOperatorFactory {
public:
  /**
   * Constructor of the factory
   * @param functor
   *    The functor that will be passed down to the created BinaryOperator nodes
   * @param repr
   *    Human readable representation of the operator
   * @param reverse
   *    Reverse the left and right sides of the operator when creating the new node.
   *    This is to be set to true when the factory is called from the right hand-side of an operator
   *    (i.e. __radd__), which happens when the left hand-side is something else that doesn't
   *    know how to add itself to a Node type (i.e a primitive float, integer, etc.)
   */
  BinaryOperatorFactory(std::function<R(T, T)> functor, const std::string& repr, bool reverse=false)
    : m_functor{functor}, m_repr{repr}, m_reverse{reverse} {}

  /**
   * Callable that creates the Node
   * @details
   *    This is what gets called from Python when an operator is used. For instance `a + b` will
   *    trigger a call `factory(a, b)`. If only a or b is of type Node, the conversion machinery
   *    will have triggered just before the creation of a Node wrapping the other one.
   * @see
   *    NodeConverter
   */
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
