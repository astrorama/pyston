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

#ifndef PYSTON_NODECONVERTER_H
#define PYSTON_NODECONVERTER_H

#include <boost/python.hpp>
#include "Pyston/Graph/Constant.h"

namespace Pyston {

/**
 * Implements the conversion logic from python primitives into Node
 * @tparam T
 *  Node type *into* which types can be converted
 */
template<typename T>
struct NodeConverter {
  /**
   * Check if the python object can be converted to a known type
   * @param obj_ptr
   *    Python object
   * @return
   *    obj_ptr if it can be converted, NULL otherwise
   */
  static void *isConvertible(PyObject *obj_ptr) {
    using boost::python::handle;
    using boost::python::object;
    using boost::python::borrowed;
    using boost::python::extract;
    using boost::python::converter::registry::query;

    // Primitive numeric types are ok
    if (PyFloat_Check(obj_ptr) || PyLong_Check(obj_ptr) || PyBool_Check(obj_ptr))
      return obj_ptr;

    return nullptr;
  }

  /**
   * Create a new Constant Node from a python primitive type: python floats, longs or booleans
   * @param obj_ptr
   *    Python object
   * @param storage
   *    Memory area, handled by boost::python, where to store the new object
   * @return
   *    true if it could be converted
   */
  static bool fromPrimitive(PyObject *obj_ptr, void *storage) {
    if (!PyFloat_Check(obj_ptr) && !PyLong_Check(obj_ptr) && !PyBool_Check(obj_ptr))
      return false;

    // Rely on the casting done by C++
    T value = 0;
    if (PyFloat_Check(obj_ptr))
      value = PyFloat_AsDouble(obj_ptr);
    else if (PyLong_Check(obj_ptr))
      value = PyLong_AsLong(obj_ptr);
    else if (PyBool_Check(obj_ptr))
      value = (obj_ptr == Py_True);

    new(storage) std::shared_ptr<Node<T>>(new Constant<T>(value));
    return true;
  }

  /**
   * Construct a new Node type from the given python object
   * @param obj_ptr
   *    Python object
   * @param data
   *    boost python data required to construct the new object
   */
  static void
  construct(PyObject *obj_ptr, boost::python::converter::rvalue_from_python_stage1_data *data) {
    // Memory area where to store the new type
    void *storage = ((boost::python::converter::rvalue_from_python_storage<std::shared_ptr<Node<T>>> *) data)->storage.bytes;

    // Abort if can not convert, because isConvertible hasn't done its job
    if (!fromPrimitive(obj_ptr, storage))
      abort();

    data->convertible = storage;
  }
};

} // end of namespace Pyston

#endif //PYSTON_NODECONVERTER_H
