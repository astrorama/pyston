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
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include "Pyston/ExceptionRaiser.h"
#include "Pyston/Helpers.h"
#include "Pyston/NodeConverter.h"
#include "Pyston/Graph/Cast.h"
#include "Pyston/Graph/Constant.h"
#include "Pyston/Graph/Functors.h"
#include "Pyston/Graph/Placeholder.h"

#if BOOST_VERSION < 105600
#include <boost/units/detail/utility.hpp>
using boost::units::detail::demangle;
#else
using boost::core::demangle;
#endif

namespace py = boost::python;

namespace Pyston {


template<typename T>
struct RegisterNode {

  /**
   * Define operations where the other value has a different type (To)
   * i.e. if __add__ is called on a boolean, and the other value (To) is a float,
   * self has to be upcasted
   */
  template<typename To>
  static void defCastOperations(py::class_<Node<T>, boost::noncopyable>& node) {
    /*
    node
      .def("__add__", makeFunction("+", std::plus<T>()))
      .def("__sub__", makeFunction("-", std::minus<T>()))
      .def("__mul__", makeBinary<T, To, std::multiplies>("*"))
      .def("__truediv__", makeBinary<T, To, std::divides>("/"))
      .def("__radd__", makeBinary<T, To, std::plus>("+", true))
      .def("__rsub__", makeBinary<T, To, std::minus>("-", true))
      .def("__rmul__", makeBinary<T, To, std::multiplies>("*", true))
      .def("__rtruediv__", makeBinary<T, To, std::divides>("/", true));
      */
  }

  /**
   * Methods for specific types
   */
  template<typename Y>
  static void specialized(py::class_<Node<Y>, boost::noncopyable>& node, void *);

  /**
   * Methods for floating point types
   */
  template<typename Y>
  static void specialized(py::class_<Node<Y>, boost::noncopyable>& node,
                          typename std::enable_if<std::is_floating_point<Y>::value>::type * = nullptr) {
    node
      .def("__pow__", makeFunction<T(T, T)>("^", Pow<T>()))
      .def("__rpow__", makeFunction<T(T, T)>("^", Reversed<T>(Pow<T>())))
      .def("__round__", makeFunction<T(T)>("round", Round<T>()))
      .def("__abs__", makeFunction<T(T)>("abs", Abs<T>()));

    // Functions
    // When using numpy methods, numpy will delegate to these
    // Taken from here, although there are a bunch not implemented:
    // https://numpy.org/devdocs/reference/ufuncs.html
    node
      .def("exp", makeFunction<T(T)>("exp", Exp<T>()))
      .def("exp2", makeFunction<T(T)>("exp2", Exp2<T>()))
      .def("log", makeFunction<T(T)>("log", Log<T>()))
      .def("log2", makeFunction<T(T)>("log2", Log2<T>()))
      .def("log10", makeFunction<T(T)>("log10", Log10<T>()))
      .def("sqrt", makeFunction<T(T)>("sqrt", Sqrt<T>()))
      .def("sin", makeFunction<T(T)>("sin", Sin<T>()))
      .def("cos", makeFunction<T(T)>("cos", Cos<T>()))
      .def("tan", makeFunction<T(T)>("tan", Tan<T>()))
      .def("arcsin", makeFunction<T(T)>("arcsin", ArcSin<T>()))
      .def("arccos", makeFunction<T(T)>("arccos", ArcCos<T>()))
      .def("arctan", makeFunction<T(T)>("arctan", ArcTan<T>()))
      .def("sinh", makeFunction<T(T)>("sinh", Sinh<T>()))
      .def("cosh", makeFunction<T(T)>("cosh", Cosh<T>()))
      .def("tanh", makeFunction<T(T)>("tanh", Tanh<T>()))
      .def("arcsinh", makeFunction<T(T)>("arcsinh", ArcSinh<T>()))
      .def("arccosh", makeFunction<T(T)>("arccosh", ArcCosh<T>()))
      .def("arctanh", makeFunction<T(T)>("arctanh", ArcTanh<T>()));
  }

  /**
   * Methods for the boolean type
   */
  template<typename Y>
  static void specialized(py::class_<Node<Y>, boost::noncopyable>& node,
                          typename std::enable_if<std::is_same<Y, bool>::value>::type * = nullptr) {
    // Upcast to double and int
    defCastOperations<double>(node);
    defCastOperations<int64_t>(node);
  }

  /**
   * Methods for integral types, except bool
   */
  template<typename Y>
  static void specialized(py::class_<Node<Y>, boost::noncopyable>& node,
                          typename std::enable_if<std::is_integral<Y>::value &&
                                                  !std::is_same<Y, bool>::value>::type * = nullptr) {
    node
      .def("__abs__", makeFunction<T(T)>("abs", Abs<T>()));
    // Upcast to double
    /*
    defCastOperations<double>(node);
    node
      .def("__pow__", makeFunction<T>("^"))
      .def("__rpow__", makeBinary<T, double, Pow>("^", true));
      */
  }

  static void general(py::class_<Node<T>, boost::noncopyable>& node) {
    // https://docs.python.org/3/reference/datamodel.html#basic-customization
    node
      .def("__lt__", makeFunction<bool(T, T)>("<", std::less<T>()))
      .def("__le__", makeFunction<bool(T, T)>("<=", std::less_equal<T>()))
      .def("__eq__", makeFunction<bool(T, T)>("==", std::equal_to<T>()))
      .def("__ne__", makeFunction<bool(T, T)>("!=", std::not_equal_to<T>()))
      .def("__gt__", makeFunction<bool(T, T)>(">", std::greater<T>()))
      .def("__ge__", makeFunction<bool(T, T)>(">=", std::greater_equal<T>()));

    // https://docs.python.org/3/reference/datamodel.html#emulating-numeric-types
    node
      .def("__add__", makeFunction<T(T, T)>("+", std::plus<T>()))
      .def("__sub__", makeFunction<T(T, T)>("-", std::minus<T>()))
      .def("__mul__", makeFunction<T(T, T)>("*", std::multiplies<T>()))
      .def("__truediv__", makeFunction<T(T, T)>("/", std::divides<T>()))
      .def("__radd__", makeFunction<T(T, T)>("+", Reversed<T>(std::plus<T>())))
      .def("__rsub__", makeFunction<T(T, T)>("-", Reversed<T>(std::minus<T>())))
      .def("__rmul__", makeFunction<T(T, T)>("*", Reversed<T>(std::multiplies<T>())))
      .def("__rtruediv__", makeFunction<T(T, T)>("/", Reversed<T>(std::divides<T>())))
      .def("__neg__", makeFunction<T(T)>("-", std::negate<T>()))
      .def("__pos__", makeFunction<T(T)>("+", (Identity<T>())));

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

  // Vector types
  py::class_<std::vector<double>>("_DoubleVector")
    .def(py::vector_indexing_suite<std::vector<double>>());

  py::class_<std::vector<int64_t>>("_IntVector")
    .def(py::vector_indexing_suite<std::vector<int64_t>>());
}

} // end of namespace Pyston
