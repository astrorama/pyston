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

#include <boost/test/unit_test.hpp>
#include "Pyston/ExpressionTreeBuilder.h"
#include "Pyston/Graph/Functors.h"
#include "PythonFixture.h"

using namespace Pyston;
namespace py = boost::python;

BOOST_AUTO_TEST_SUITE(FunctionWrapper_test)

/**
 * Wrap a simple lambda expression. It should be able to
 * translate directly, so no more calls to Python are required
 */
BOOST_FIXTURE_TEST_CASE(Wrapper_test, PythonFixture) {
  ExpressionTreeBuilder builder;
  auto py_func = py::eval("lambda x, y: x**2 + y");

  std::function<double(double, double)> transparent;
  bool compiled;
  // Call directly
  {
    std::tie(compiled, transparent) = builder.build<double(double, double)>(py_func);
    BOOST_CHECK(compiled);
  }
  // Original is out of scope, std::function should still be alive
  BOOST_CHECK_EQUAL(transparent(3, 2), 11);
}

/**
 * Wrap a lambda expression that can *not* be translated, since one of the
 * variables is used on a flow control statement (if).
 * It should still be callable, even if through the interpreter. The flag
 * must be set accordingly in any case.
 */
BOOST_FIXTURE_TEST_CASE(WrapperFallback_test, PythonFixture) {
  ExpressionTreeBuilder builder;
  auto py_func = py::eval("lambda x, y, z: x ** 2 + y if z > 0.5 else z", main_namespace);

  std::function<double(double, double, double)> transparent;
  bool compiled;
  // Call directly
  {
    std::tie(compiled, transparent) = builder.build<double(double, double, double)>(py_func);
    BOOST_CHECK(!compiled);
  }
  // Original is out of scope, std::function should still be alive
  BOOST_CHECK_EQUAL(transparent(1, 2, 0.6), 3);
  BOOST_CHECK_EQUAL(transparent(1, 2, 0.4), 0.4);
}

/**
 * Wrap a lambda expression that can *not* be translated, and that,
 * on top of it, will raise an exception in some cases. The wrapper must
 * be able to catch and translate into a C++ exception.
 */
BOOST_FIXTURE_TEST_CASE(WrapperException_test, PythonFixture) {
  ExpressionTreeBuilder builder;
  py::exec(R"PYCODE(
def raises_exception(x, y, z):
  if z > 0.5:
    return x **2 + y
  else:
    raise ValueError('Invalid Z value')
)PYCODE", main_namespace);

  auto py_func = main_namespace["raises_exception"];
  std::function<double(double, double, double)> transparent;
  bool compiled;
  {
    std::tie(compiled, transparent) = builder.build<double(double, double, double)>(py_func);
    BOOST_CHECK(!compiled);
  }

  // Call that succeeds
  BOOST_CHECK_EQUAL(transparent(1, 2, 0.6), 3);

  // Call that raises exception
  try {
    transparent(1, 2, 0.4);
    BOOST_FAIL("Call should have raised an exception");
  }
  catch (const Exception& ex) {
    BOOST_CHECK_EQUAL(std::string(ex.what()), "Invalid Z value");
    BOOST_CHECK_GT(ex.getTraceback().size(), 0);
    bool func_in_trace = false;
    for (auto& trace : ex.getTraceback()) {
      BOOST_TEST_MESSAGE(trace.filename << ": " << trace.funcname << " line " << trace.lineno);
      func_in_trace |= trace.funcname == "raises_exception";
    }
    BOOST_CHECK(func_in_trace);
  }
}

/**
 * Register custom functions
 */
double world2pixel(double x) {
  return std::sin(x) * 10;
}

double mishmash(double x, double y) {
  return std::asinh(x) - std::log(y / 2);
}

template<typename T>
using World2Pixel = UnaryWrapper<T, T, world2pixel>;

template<typename T>
using Mismash = BinaryWrapper<T, T, mishmash>;

BOOST_FIXTURE_TEST_CASE(AddUnaryFunction_test, PythonFixture) {
  ExpressionTreeBuilder builder;
  builder.registerFunction<double, World2Pixel>("world2pixel");

  py::exec(R"PYCODE(
def uses_function(x, y):
  return pyston.world2pixel(x + y)
)PYCODE", main_namespace);

  auto py_func = main_namespace["uses_function"];
  std::function<double(double, double)> transparent;
  bool compiled;
  std::tie(compiled, transparent) = builder.build<double(double, double)>(py_func);
  BOOST_CHECK(compiled);

  double r = transparent(10, 20);
  BOOST_CHECK_CLOSE(r, world2pixel(10 + 20), 1e-8);
}

BOOST_FIXTURE_TEST_CASE(AddBinaryFunction_test, PythonFixture) {
  ExpressionTreeBuilder builder;
  builder.registerFunction<double, double, Mismash>("mishmash");

  py::exec(R"PYCODE(
def uses_function(x, y):
  return pyston.mishmash(x * 2, y)
)PYCODE", main_namespace);

  auto py_func = main_namespace["uses_function"];
  std::function<double(double, double)> transparent;
  bool compiled;
  std::tie(compiled, transparent) = builder.build<double(double, double)>(py_func);
  BOOST_CHECK(compiled);

  double r = transparent(10, 20);
  BOOST_CHECK_CLOSE(r, mishmash(10 * 2, 20), 1e-8);
}

BOOST_AUTO_TEST_SUITE_END()
