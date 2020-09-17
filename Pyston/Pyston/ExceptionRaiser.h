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

#ifndef PYSTON_EXCEPTIONRAISER_H
#define PYSTON_EXCEPTIONRAISER_H

#include <memory>
#include <string>
#include <boost/python/errors.hpp>
#include "AST/Node.h"

namespace Pyston {

template<typename T>
class ExceptionRaiser {
public:
  ExceptionRaiser(const std::string& msg) : m_msg{msg} {}

  void operator()(const std::shared_ptr<Node<T>>&) {
    PyErr_SetString(PyExc_TypeError, m_msg.c_str());
    throw boost::python::error_already_set();
  }

private:
  std::string m_msg;
};

} // end of namespace Pyston

#endif //PYSTON_EXCEPTIONRAISER_H
