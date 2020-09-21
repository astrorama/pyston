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
#include "Pyston/Function.h"
#include "PythonFixture.h"

using namespace Pyston;
namespace py = boost::python;

BOOST_AUTO_TEST_SUITE(FunctionWrapper_test)


BOOST_FIXTURE_TEST_CASE(Wrapper_test, PythonFixture) {
  auto py_func = py::eval("lambda x, y: x**2 + y");

  std::function<double(double, double)> transparent;
  // Call directly
  {
    Function<double, double, double> func(py_func);
    BOOST_CHECK(!func.usesFallback());
    BOOST_CHECK_EQUAL(func(2, 4), 8);
    transparent = func;
  }
  // Original is out of scope, std::function should still be alive
  BOOST_CHECK_EQUAL(transparent(3, 2), 11);
}

BOOST_FIXTURE_TEST_CASE(WrapperFallback_test, PythonFixture) {
  auto py_func = py::eval("lambda x, y, z: x ** 2 + y if z > 0.5 else z", main_namespace);

  std::function<double(double, double, double)> transparent;
  // Call directly
  {
    Function<double, double, double, double> func(py_func);
    BOOST_CHECK(func.usesFallback());
    BOOST_CHECK_EQUAL(func(1., 2., 0.6), 3.);
    BOOST_CHECK_CLOSE(func(1., 2., 0.4), 0.4, 1e-5);
    transparent = func;
  }
  // Original is out of scope, std::function should still be alive
  BOOST_CHECK_EQUAL(transparent(1, 2, 0.6), 3);
  BOOST_CHECK_EQUAL(transparent(1, 2, 0.4), 0.4);
}

BOOST_AUTO_TEST_SUITE_END()
