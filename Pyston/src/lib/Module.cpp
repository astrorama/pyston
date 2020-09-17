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

#include <boost/python.hpp>
#include "Pyston/ExceptionRaiser.h"
#include "Pyston/Helpers.h"
#include "Pyston/AST/Constant.h"
#include "Pyston/AST/Placeholder.h"


namespace py = boost::python;

namespace Pyston {

template<typename T>
struct NodeConverter {
  static void *isConvertible(PyObject *obj_ptr) {
    if (PyFloat_Check(obj_ptr) || PyLong_Check(obj_ptr))
      return obj_ptr;
    return nullptr;
  }

  static void construct(PyObject *obj_ptr, py::converter::rvalue_from_python_stage1_data *data) {
    T value;
    if (PyFloat_Check(obj_ptr))
      value = PyFloat_AsDouble(obj_ptr);
    else if (PyLong_Check(obj_ptr))
      value = PyLong_AsDouble(obj_ptr);
    else
      abort(); // Should not happen, isConvertible takes care of this
    void *storage = ((py::converter::rvalue_from_python_storage<std::shared_ptr<Node<T>>> *) data)->storage.bytes;
    new(storage) std::shared_ptr<Node<T>>(new Constant<T>(value));
    data->convertible = storage;
  }
};


template<typename T>
static T identity(T val) {
  return val;
}

template<typename T>
static void registerNode() {
  // Force casting to the proper type
  T (*_pow)(T, T) = std::pow;
  T (*_abs)(T) = std::abs;
  T (*_round)(T) = std::round;
  T (*_exp)(T) = std::exp;
  T (*_exp2)(T) = std::exp2;
  T (*_log)(T) = std::log;
  T (*_log2)(T) = std::log2;
  T (*_log10)(T) = std::log10;
  T (*_sqrt)(T) = std::sqrt;
  T (*_sin)(T) = std::sin;
  T (*_cos)(T) = std::cos;
  T (*_tan)(T) = std::tan;
  T (*_arcsin)(T) = std::asin;
  T (*_arccos)(T) = std::acos;
  T (*_arctan)(T) = std::atan;
  T (*_sinh)(T) = std::sinh;
  T (*_cosh)(T) = std::cosh;
  T (*_tanh)(T) = std::tanh;
  T (*_arcsinh)(T) = std::asinh;
  T (*_arccosh)(T) = std::acosh;
  T (*_arctanh)(T) = std::atanh;

  py::class_<Node<T>, boost::noncopyable> node("Node", "AST Node", py::no_init);

  // https://docs.python.org/3/reference/datamodel.html#emulating-numeric-types
  node
    .def("__add__", makeBinary<T>(std::plus<T>(), "+"))
    .def("__sub__", makeBinary<T>(std::minus<T>(), "-"))
    .def("__mul__", makeBinary<T>(std::multiplies<T>(), "*"))
    .def("__truediv__", makeBinary<T>(std::divides<T>(), "/"))
    .def("__pow__", makeBinary<T>(_pow, "^"))
    .def("__radd__", makeBinary<T>(std::plus<T>(), "+", true))
    .def("__rsub__", makeBinary<T>(std::minus<T>(), "-", true))
    .def("__rmul__", makeBinary<T>(std::multiplies<T>(), "*", true))
    .def("__rtruediv__", makeBinary<T>(std::divides<T>(), "/", true))
    .def("__rpow__", makeBinary<T>(_pow, "^", true))
    .def("__neg__", makeUnary<T>(std::negate<T>(), "-"))
    .def("__pos__", makeUnary<T>(&identity<T>, "+"))
    .def("__abs__", makeUnary<T>(_abs, "abs"))
    .def("__round__", makeUnary<T>(_round, "round"));

  // https://docs.python.org/3/reference/datamodel.html#basic-customization
  node
    .def("__lt__", makeBinary<T>(std::less<T>(), "<"))
    .def("__le__", makeBinary<T>(std::less_equal<T>(), "<="))
    .def("__eq__", makeBinary<T>(std::equal_to<T>(), "=="))
    .def("__ne__", makeBinary<T>(std::not_equal_to<T>(), "!="))
    .def("__gt__", makeBinary<T>(std::greater<T>(), ">"))
    .def("__ge__", makeBinary<T>(std::greater_equal<T>(), ">="));

  // Functions
  // When using numpy methods, numpy will delegate to these
  // Taken from here, although there are a bunch not implemented:
  // https://numpy.org/devdocs/reference/ufuncs.html
  node
    .def("exp", makeUnary<T>(_exp, "exp"))
    .def("exp2", makeUnary<T>(_exp2, "exp2"))
    .def("log", makeUnary<T>(_log, "log"))
    .def("log2", makeUnary<T>(_log2, "log2"))
    .def("log10", makeUnary<T>(_log10, "log10"))
    .def("sqrt", makeUnary<T>(_sqrt, "sqrt"))
    .def("sin", makeUnary<T>(_sin, "sin"))
    .def("cos", makeUnary<T>(_cos, "cos"))
    .def("tan", makeUnary<T>(_tan, "tan"))
    .def("arcsin", makeUnary<T>(_arcsin, "arcsin"))
    .def("arccos", makeUnary<T>(_arccos, "arccos"))
    .def("arctan", makeUnary<T>(_arctan, "arctan"))
    .def("sinh", makeUnary<T>(_sinh, "sinh"))
    .def("cosh", makeUnary<T>(_cosh, "cosh"))
    .def("tanh", makeUnary<T>(_tanh, "tanh"))
    .def("arcsinh", makeUnary<T>(_arcsinh, "arcsinh"))
    .def("arccosh", makeUnary<T>(_arccosh, "arccosh"))
    .def("arctanh", makeUnary<T>(_arctanh, "arctanh"));

  // Can not be used in conditionals!
  node.def("__bool__", py::make_function(
    ExceptionRaiser<T>("Can not use variable placeholders in conditionals"),
    py::default_call_policies(),
    boost::mpl::vector<void, const std::shared_ptr<Node<T>>>()
  ));

  // Register convertion between shared pointer and Node
  py::register_ptr_to_python<std::shared_ptr<Node<T>>>();

  // Custom conversion so primitive values can be converted to a node
  py::converter::registry::push_back(&NodeConverter<T>::isConvertible, &NodeConverter<T>::construct,
                                     py::type_id<std::shared_ptr<Node<T>>>());

  // Triggers the building of a tree
  py::class_<Placeholder<T>, py::bases<Node<T>>>("Placeholder", "Variable placeholder",
                                                 py::no_init);
  py::register_ptr_to_python<std::shared_ptr<Placeholder<T>>>();
}

BOOST_PYTHON_MODULE (libPyston) {
  registerNode<float>();
  registerNode<double>();
}

} // end of namespace Pyston
