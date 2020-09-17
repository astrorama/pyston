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

template<typename T, typename...Args>
static boost::python::object makeUnary(Args&& ...args) {
  return boost::python::make_function(
    UnaryOperatorFactory<T>(std::forward<Args>(args)...),
    boost::python::default_call_policies(),
    boost::mpl::vector<std::shared_ptr<Node<T>>, const std::shared_ptr<Node<T>>&>()
  );
}

template<typename T, typename...Args>
static boost::python::object makeBinary(Args&& ...args) {
  return boost::python::make_function(
    BinaryOperatorFactory<T>(std::forward<Args>(args)...),
    boost::python::default_call_policies(),
    boost::mpl::vector<std::shared_ptr<Node<T>>, const std::shared_ptr<Node<T>>&, const std::shared_ptr<Node<T>>&>()
  );
}
}

#endif //PYSTON_HELPERS_H
