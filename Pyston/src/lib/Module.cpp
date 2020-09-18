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
#include "Pyston/Functors.h"
#include "Pyston/Helpers.h"
#include "Pyston/AST/Cast.h"
#include "Pyston/AST/Constant.h"
#include "Pyston/AST/Placeholder.h"

#if BOOST_VERSION < 105600
#include <boost/units/detail/utility.hpp>
using boost::units::detail::demangle;
#else
using boost::core::demangle;
#endif

namespace py = boost::python;

namespace Pyston {

template<typename T>
struct NodeConverter {
  static void *isConvertible(PyObject *obj_ptr) {
    // Primitive numeric types are ok
    if (PyFloat_Check(obj_ptr) || PyLong_Check(obj_ptr) || PyBool_Check(obj_ptr))
      return obj_ptr;

    // Try other Node types
    py::handle<PyObject> handle(py::borrowed(obj_ptr));
    py::object object(handle);

    py::extract<Node<bool>> bool_extract(object);
    //py::extract<Node<double>> double_extract(object);
    //py::extract<Node<int64_t>> int_extract(object);

    if (bool_extract.check())
      return obj_ptr;

    // No idea
    return nullptr;
  }

  static bool
  fromPrimitive(PyObject *obj_ptr, void *storage) {
    if (!PyFloat_Check(obj_ptr) && !PyLong_Check(obj_ptr) && !PyBool_Check(obj_ptr))
      return false;

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

  template<typename From>
  static bool createCastNode(py::object& object, void *storage) {
    py::extract<std::shared_ptr<Node<From>>> extractor(object);
    if (!extractor.check())
      return false;

    new(storage)std::shared_ptr<Node<T>>(new Cast<T, From>(extractor));

    return true;
  }

  static bool castNode(PyObject *obj_ptr, void *storage) {
    py::handle<> handle(py::borrowed(obj_ptr));
    py::object object(handle);

    return createCastNode<bool>(object, storage) ||
      createCastNode<int64_t>(object, storage) ||
      createCastNode<double>(object, storage);
  }

  static void construct(PyObject *obj_ptr, py::converter::rvalue_from_python_stage1_data *data) {
    void *storage = ((py::converter::rvalue_from_python_storage<std::shared_ptr<Node<T>>> *) data)->storage.bytes;

    // Abort if can not convert, because isConvertible hasn't done its job
    if (!fromPrimitive(obj_ptr, storage) && !castNode(obj_ptr, storage))
      abort();

    data->convertible = storage;
  }
};


template<typename T>
struct RegisterNode {

  template<typename Y>
  static void specialized(py::class_<Node<Y>, boost::noncopyable>& node, void *);

  template<typename Y>
  static void specialized(py::class_<Node<Y>, boost::noncopyable>& node,
                          typename std::enable_if<std::is_floating_point<Y>::value>::type * = nullptr) {
    node
      .def("__pow__", makeBinary<T, Pow>("^"))
      .def("__rpow__", makeBinary<T, Pow>("^", true))
      .def("__round__", makeUnary<T, Round>("round"))
      .def("__abs__", makeUnary<T, Abs>("abs"));

    // Functions
    // When using numpy methods, numpy will delegate to these
    // Taken from here, although there are a bunch not implemented:
    // https://numpy.org/devdocs/reference/ufuncs.html
    node
      .def("exp", makeUnary<T, Exp>("exp"))
      .def("exp2", makeUnary<T, Exp2>("exp2"))
      .def("log", makeUnary<T, Log>("log"))
      .def("log2", makeUnary<T, Log2>("log2"))
      .def("log10", makeUnary<T, Log10>("log10"))
      .def("sqrt", makeUnary<T, Sqrt>("sqrt"))
      .def("sin", makeUnary<T, Sin>("sin"))
      .def("cos", makeUnary<T, Cos>("cos"))
      .def("tan", makeUnary<T, Tan>("tan"))
      .def("arcsin", makeUnary<T, ArcSin>("arcsin"))
      .def("arccos", makeUnary<T, ArcCos>("arccos"))
      .def("arctan", makeUnary<T, ArcTan>("arctan"))
      .def("sinh", makeUnary<T, Sinh>("sinh"))
      .def("cosh", makeUnary<T, Cosh>("cosh"))
      .def("tanh", makeUnary<T, Tanh>("tanh"))
      .def("arcsinh", makeUnary<T, ArcSinh>("arcsinh"))
      .def("arccosh", makeUnary<T, ArcCosh>("arccosh"))
      .def("arctanh", makeUnary<T, ArcTanh>("arctanh"));
  }

  template<typename Y>
  static void specialized(py::class_<Node<Y>, boost::noncopyable>&,
                          typename std::enable_if<std::is_same<Y, bool>::value>::type * = nullptr) {
  }

  template<typename Y>
  static void specialized(py::class_<Node<Y>, boost::noncopyable>& node,
                          typename std::enable_if<std::is_integral<Y>::value &&
                                                  !std::is_same<Y, bool>::value>::type * = nullptr) {
    node
      .def("__abs__", makeUnary<T, Abs>("abs"));
  }

  static void general(py::class_<Node<T>, boost::noncopyable>& node) {
    // https://docs.python.org/3/reference/datamodel.html#basic-customization
    node
      .def("__lt__", makeBinary<T, std::less>("<"))
      .def("__le__", makeBinary<T, std::less_equal>("<="))
      .def("__eq__", makeBinary<T, std::equal_to>("=="))
      .def("__ne__", makeBinary<T, std::not_equal_to>("!="))
      .def("__gt__", makeBinary<T, std::greater>(">"))
      .def("__ge__", makeBinary<T, std::greater_equal>(">="));

    // https://docs.python.org/3/reference/datamodel.html#emulating-numeric-types
    node
      .def("__add__", makeBinary<T, std::plus>("+"))
      .def("__sub__", makeBinary<T, std::minus>("-"))
      .def("__mul__", makeBinary<T, std::multiplies>("*"))
      .def("__truediv__", makeBinary<T, std::divides>("/"))
      .def("__radd__", makeBinary<T, std::plus>("+", true))
      .def("__rsub__", makeBinary<T, std::minus>("-", true))
      .def("__rmul__", makeBinary<T, std::multiplies>("*", true))
      .def("__rtruediv__", makeBinary<T, std::divides>("/", true))
      .def("__neg__", makeUnary<T, std::negate>("-"))
      .def("__pos__", makeUnary<T, Identity>("+"));

    // Can not be used in conditionals!
    node.def("__bool__", py::make_function(
      ExceptionRaiser<T>("Can not use variable placeholders in conditionals"),
      py::default_call_policies(),
      boost::mpl::vector<void, const std::shared_ptr<Node<T>>>()
    ));
  }

  static void Do() {
    auto node_name = std::string("Node<") + demangle(typeid(T).name()) + ">";
    py::class_<Node<T>, boost::noncopyable> node(node_name.c_str(), "AST Node", py::no_init);

    // Operators and method applicable to all types
    general(node);

    // Operators and method applicable only to a given type
    // i.e. __floordiv__ is not applicable to float
    specialized<T>(node);

    // Register convertion between shared pointer and Node
    py::register_ptr_to_python<std::shared_ptr<Node<T>>>();

    // Custom conversion so primitive values can be converted to a node
    py::converter::registry::push_back(&NodeConverter<T>::isConvertible,
                                       &NodeConverter<T>::construct,
                                       py::type_id<std::shared_ptr<Node<T>>>());

    // Triggers the building of a tree
    auto placeholder_name = std::string("Placeholder<") + demangle(typeid(T).name()) + ">";
    py::class_<Placeholder<T>, py::bases<Node<T>>>(
      placeholder_name.c_str(), "Variable placeholder", py::no_init);
    py::register_ptr_to_python<std::shared_ptr<Placeholder<T>>>();
  }
};

BOOST_PYTHON_MODULE (libPyston) {
  RegisterNode<double>::Do();
  RegisterNode<int64_t>::Do();
  RegisterNode<bool>::Do();
}

} // end of namespace Pyston
