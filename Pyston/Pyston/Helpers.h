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

template<typename T, template<class> class Functor>
static boost::python::object makeUnary(const std::string& repr) {
  typedef decltype(std::declval<Functor<T>>()(T())) R;

  return boost::python::make_function(
    UnaryOperatorFactory<R, T>(Functor<T>(), repr),
    boost::python::default_call_policies(),
    boost::mpl::vector<std::shared_ptr<Node<T>>, const std::shared_ptr<Node<T>>&>()
  );
}

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
