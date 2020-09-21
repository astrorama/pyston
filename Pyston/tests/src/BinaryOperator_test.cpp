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
#include "Pyston/AST/Node.h"
#include "Pyston/AST/Placeholder.h"
#include "PythonFixture.h"

using namespace Pyston;
namespace py = boost::python;

BOOST_AUTO_TEST_SUITE(UnaryOperator_test)

BOOST_FIXTURE_TEST_CASE(AdditionFloat_test, PythonFixture) {
  auto add = py::eval("lambda x, y: x + y");
  auto X = std::make_shared<Placeholder<double>>("x");
  auto Y = std::make_shared<Placeholder<double>>("y");

  auto py_comp = add(X, Y);
  py::extract<std::shared_ptr<Node<double>>> comp(py_comp);

  BOOST_CHECK_EQUAL(comp()->eval({{"x", 22.},
                                  {"y", 55.}}), 77.);
  BOOST_CHECK_EQUAL(comp()->eval({{"x", 22.},
                                  {"y", -55.}}), -33.);
}

BOOST_FIXTURE_TEST_CASE(SubstractionFloat_test, PythonFixture) {
  auto minus = py::eval("lambda x, y: x - y");
  auto X = std::make_shared<Placeholder<double>>("x");
  auto Y = std::make_shared<Placeholder<double>>("y");

  auto py_comp = minus(X, Y);
  py::extract<std::shared_ptr<Node<double>>> comp(py_comp);

  BOOST_CHECK_EQUAL(comp()->eval({{"x", 22.},
                                  {"y", 55.}}), -33.);
  BOOST_CHECK_EQUAL(comp()->eval({{"x", 22.},
                                  {"y", -55.}}), 77.);
  BOOST_CHECK_EQUAL(comp()->eval({{"y", 22.},
                                  {"x", -55.}}), -77.);
}

BOOST_FIXTURE_TEST_CASE(ProductFloat_test, PythonFixture) {
  auto prod = py::eval("lambda x, y: x * y");
  auto X = std::make_shared<Placeholder<double>>("x");
  auto Y = std::make_shared<Placeholder<double>>("y");

  auto py_comp = prod(X, Y);
  py::extract<std::shared_ptr<Node<double>>> comp(py_comp);

  BOOST_CHECK_EQUAL(comp()->eval({{"x", 22.},
                                  {"y", 55.}}), 1210.);
  BOOST_CHECK_EQUAL(comp()->eval({{"x", 22.},
                                  {"y", -55.}}), -1210.);
}

BOOST_FIXTURE_TEST_CASE(DivFloat_test, PythonFixture) {
  auto div = py::eval("lambda x, y: x / y");
  auto X = std::make_shared<Placeholder<double>>("x");
  auto Y = std::make_shared<Placeholder<double>>("y");

  auto py_comp = div(X, Y);
  py::extract<std::shared_ptr<Node<double>>> comp(py_comp);

  BOOST_CHECK_CLOSE(comp()->eval({{"x", 22.},
                                  {"y", 55.}}), 0.4, 1e-4);
  BOOST_CHECK_CLOSE(comp()->eval({{"y", 22.},
                                  {"x", 55.}}), 2.5, 1e-4);
}

BOOST_FIXTURE_TEST_CASE(PowFloat_test, PythonFixture) {
  auto pow = py::eval("lambda x, y: x ** y");
  auto X = std::make_shared<Placeholder<double>>("x");
  auto Y = std::make_shared<Placeholder<double>>("y");

  auto py_comp = pow(X, Y);
  py::extract<std::shared_ptr<Node<double>>> comp(py_comp);

  BOOST_CHECK_CLOSE(comp()->eval({{"x", 2.5}, {"y", 6.}}), 244.140625, 1e-4);
  BOOST_CHECK_CLOSE(comp()->eval({{"y", 2.5}, {"x", 6.}}), 88.18164, 1e-4);
}

BOOST_FIXTURE_TEST_CASE(PowFloatConstant_test, PythonFixture) {
  auto pow = py::eval("lambda x: x ** 10");
  auto X = std::make_shared<Placeholder<double>>("x");

  auto py_comp = pow(X);
  py::extract<std::shared_ptr<Node<double>>> comp(py_comp);

  BOOST_CHECK_CLOSE(comp()->eval({{"x", 6.}}), 60466176., 1e-4);
}

BOOST_FIXTURE_TEST_CASE(Gt_test, PythonFixture) {
  auto gt = py::eval("lambda x, y: x > y");
  auto X = std::make_shared<Placeholder<double>>("x");
  auto Y = std::make_shared<Placeholder<double>>("y");

  auto py_comp = gt(X, Y);
  py::extract<std::shared_ptr<Node<bool>>> comp(py_comp);

  BOOST_CHECK_EQUAL(comp()->eval({{"x", 2.5}, {"y", 6.}}), false);
  BOOST_CHECK_EQUAL(comp()->eval({{"y", 2.5}, {"x", 6.}}), true);
}

BOOST_FIXTURE_TEST_CASE(GtCast_test, PythonFixture) {
  auto gt = py::eval("lambda x, y: x > y");
  auto X = std::make_shared<Placeholder<double>>("x");
  auto Y = std::make_shared<Placeholder<double>>("y");

  auto py_comp = gt(X, Y);
  py::extract<std::shared_ptr<Node<double>>> comp(py_comp);

  BOOST_CHECK_EQUAL(comp()->eval({{"x", 2.5}, {"y", 6.}}), 0.);
  BOOST_CHECK_EQUAL(comp()->eval({{"y", 2.5}, {"x", 6.}}), 1.);
}

BOOST_AUTO_TEST_SUITE_END()
