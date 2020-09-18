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
#include "AST/Node.h"
#include "Pyston/AST/BinaryOperator.h"
#include "Pyston/AST/UnaryOperator.h"

namespace Pyston {

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
template<typename T, template<class> class Functor>
static boost::python::object makeUnary(const std::string& repr) {
  typedef decltype(std::declval<Functor<T>>()(T())) R;

  return boost::python::make_function(
    UnaryOperatorFactory<R, T>(Functor<T>(), repr),
    boost::python::default_call_policies(),
    boost::mpl::vector<std::shared_ptr<Node<R>>, const std::shared_ptr<Node<T>>&>()
  );
}

/**
 * @copydetails makeUnary
 * @tparam T
 *  Type corresponding to the Node to which we are adding the method
 * @tparam Functor
 *  A functor that receives two parameters of the type the Node represents.
 *  This method infers the return type automatically, so it can be used to insert binary
 *  operators that return a type different to the one it receives (i.e. greater-than
 *  receives two floats and return one boolean)
 * @param repr
 *  Literal representation of the node, for pretty-printing
 * @return
 *  A callable python object
 */
template<typename T, template<class> class Functor>
static boost::python::object makeBinary(const std::string& repr, bool reverse = false) {
  typedef decltype(std::declval<Functor<T>>()(T(), T())) R;

  return boost::python::make_function(
    BinaryOperatorFactory<R, T>(Functor<T>(), repr, reverse),
    boost::python::default_call_policies(),
    boost::mpl::vector<std::shared_ptr<Node<R>>, const std::shared_ptr<Node<T>>&, const std::shared_ptr<Node<T>>&>()
  );
}
}

#endif //PYSTON_HELPERS_H
