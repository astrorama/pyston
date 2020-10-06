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

/**
 * Template class for unary operators
 * @tparam R
 *  Operator return type
 * @tparam T
 *  Operator parameter type
 */
template<typename R, typename T>
class UnaryOperator : public Node<R> {
public:
  /**
   * Constructor
   * @param node
   *    Value to which the operator will be applied
   * @param functor
   *    Implements the operator
   * @param repr
   *    Human readable representation of the functor (i.e. +, -, abs, exp, ...)
   */
  UnaryOperator(const std::shared_ptr<Node < T>>

  & node,
  std::function<R(const Context&, T)> functor,
  const std::string& repr
  )
  : m_node{ node }, m_functor{ functor }, m_repr{ repr } {}

  /**
   * @copydoc Node::repr
   */
  std::string repr() const final {
    return m_repr;
  }

  /**
   * @copydoc Node::eval
   */
  R eval(const Context& context, const Arguments& args) const final {
    return m_functor(context, m_node->eval(context, args));
  }

  /**
   * @copydoc Node::visit
   */
  void visit(Visitor& visitor) const final {
    visitor.enter(this);
    m_node->visit(visitor);
    visitor.exit(this);
  }

private:
  std::shared_ptr<Node < T>> m_node;
  std::function<R(const Context&, T)> m_functor;
  std::string m_repr;
};

/**

 */
template<typename Signature>
class UnaryOperatorFactory;


/**
 * Specialization for functions that receive a context
 *
 * @tparam R
 *  Type corresponding to the created new Node
 * @tparam T
 *  Type corresponding to the received Node
 */
template<typename R, typename T>
class UnaryOperatorFactory<R(const Context&, T)> {

public:
  /**
   * Constructor
   * @param functor
   *    The functor that will be passed down to the created UnaryOperator nodes
   * @param repr
   *    Human readable representation of the operator
   */
  UnaryOperatorFactory(std::function<R(const Context&, T)> functor, const std::string& repr)
  : m_functor{functor}, m_repr{repr} {
  }

  /**
   * Callable that creates the Node
   * @details
   *    This is what gets called from Python when an operator is used. For instance `-a` will
   *    trigger a call `factory(a)`. Unlike the BinaryOperatorFactory, this will *not* be called
   *    is a is not of type Node, since there would be no reason to!
   */
  std::shared_ptr<Node<R>> operator() (const std::shared_ptr<Node<T>>& node) const {
    return std::make_shared<UnaryOperator<R, T>>(node, m_functor, m_repr);
  }

protected:
  UnaryOperatorFactory() {}

  std::function<R(const Context&, T)> m_functor;
  std::string m_repr;
};

/**
 * Specialization for functions that only receive one parameter, we hide the context
 * from them
 *
 * @tparam R
 *  Type corresponding to the created new Node
 * @tparam T
 *  Type corresponding to the received Node
 */
template<typename R, typename T>
class UnaryOperatorFactory<R(T)> : public UnaryOperatorFactory<R(const Context&, T)> {
public:
  /**
   * Constructor
   * @param functor
   *    The functor that will be passed down to the created UnaryOperator nodes
   * @param repr
   *    Human readable representation of the operator
   */
  UnaryOperatorFactory(std::function<R(T)> functor, const std::string& repr) {
    m_repr = repr;
    m_functor = [functor](const Context&, T v) {
      return functor(v);
    };
  }

protected:
  using UnaryOperatorFactory<R(const Context&, T)>::m_functor;
  using UnaryOperatorFactory<R(const Context&, T)>::m_repr;
};


} // end of namespace Pyston

#endif //PYSTON_UNARYOPERATOR_H
