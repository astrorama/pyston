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

#ifndef PYSTON_HELPERS_H
#define PYSTON_HELPERS_H

#include <memory>
#include <boost/python.hpp>
#include "Pyston/Graph/BinaryOperator.h"
#include "Pyston/Graph/UnaryOperator.h"
#include "Pyston/Graph/Cast.h"

namespace Pyston {

template<typename Signature>
struct makeUnaryHelper;

template<typename R, typename T>
struct makeUnaryHelper<R(T)> {
  static boost::python::object make(const std::string& repr, std::function<R(T)> functor) {
    auto op_factory = UnaryOperatorFactory<R(T)>(functor, repr);
    return boost::python::make_function(
      op_factory,
      boost::python::default_call_policies(),
      boost::mpl::vector<std::shared_ptr<Node<R>>, const std::shared_ptr<Node<T>>&>()
    );
  }
};

template<typename R, typename T>
struct makeUnaryHelper<R(const Context&, T)> {
  static boost::python::object make(const std::string& repr, std::function<R(const Context&, T)> functor) {
    auto op_factory = UnaryOperatorFactory<R(const Context&, T)>(functor, repr);
    return boost::python::make_function(
      op_factory,
      boost::python::default_call_policies(),
      boost::mpl::vector<std::shared_ptr<Node<R>>, const std::shared_ptr<Node<T>>&>()
    );
  }
};

/**
 * @details
 *  boost::python seems to have some trouble attaching functors as methods
 *  to python objects, so it requires some massaging in order to figure out the signature,
 *  keep the reference alive, etc.
 *  Since the code is a bit convolved, we isolate it here to make it easier to use
 * @tparam T
 *  Type corresponding to the Node to which we are adding the method
 * @tparam Functor
 *  A functor that receives a single parameter of the type the Node represents.
 *  This method infers the return type automatically, so it can be used to insert unary
 *  operators that return a type different to the one it receives
 * @param repr
 *  Literal representation of the node, for pretty-printing
 * @return
 *  A callable python object
 */
template<typename Signature>
static boost::python::object makeUnary(const std::string& repr, std::function<Signature> functor) {
  return makeUnaryHelper<Signature>::make(repr, functor);
}

/**
 * @copydetails makeUnary
 * @tparam TL
 *  Type of the left value
 * @tparam TR
 *  Type of the right value
 * @tparam Functor
 *  A functor that receives two parameters of the type the Node represents.
 *  This method infers the return type automatically, so it can be used to insert binary
 *  operators that return a type different to the one it receives (i.e. greater-than
 *  receives two floats and return one boolean)
 * @param repr
 *  Literal representation of the node, for pretty-printing
 * @return
 *  A callable python object
 * @note
 *  This implements the operation when both sides have the same type
 */
template<typename TL, typename TR, template<class> class Functor>
static auto makeBinary(const std::string& repr,
                       bool reverse = false) -> typename std::enable_if<std::is_same<TL, TR>::value, boost::python::object>::type {
  using T = TR;
  typedef decltype(std::declval<Functor<T>>()(T(), T())) R;

  return boost::python::make_function(
    BinaryOperatorFactory<R, TL>(Functor<T>(), repr, reverse),
    boost::python::default_call_policies(),
    boost::mpl::vector<std::shared_ptr<Node<R>>, const std::shared_ptr<Node<TL>>&, const std::shared_ptr<Node<TR>>&>()
  );
}

/**
 * @copydoc makeBinary
 * @note
 *  This implements the operation when a cast operation is required:
 *  the *left* value will be casted to the *right* type (TR)
 */
template<typename TL, typename TR, template<class> class Functor>
static auto makeBinary(const std::string& repr,
                       bool reverse = false) -> typename std::enable_if<!std::is_same<TL, TR>::value, boost::python::object>::type {
  typedef decltype(std::declval<Functor<TR>>()(TR(), TR())) R;

  // Operator itself
  BinaryOperatorFactory<R, TR> op(Functor<TR>(), repr, reverse);

  // Wrap on a lambda that casts TL into TR
  return boost::python::make_function(
    [op](const std::shared_ptr<Node<TL>>&left, const std::shared_ptr<Node<TR>>& right){
      auto cast_left = std::make_shared<Cast<TR, TL>>(left);
      return op(cast_left, right);
    },
    boost::python::default_call_policies(),
    boost::mpl::vector<std::shared_ptr<Node<R>>, const std::shared_ptr<Node<TL>>&, const std::shared_ptr<Node<TR>>&>()
  );
}

}

#endif //PYSTON_HELPERS_H
