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
#include <boost/python.hpp>
#include "Pyston/AST/Node.h"
#include "Pyston/AST/Placeholder.h"
#include "PythonFixture.h"

using namespace Pyston;
namespace py = boost::python;

BOOST_AUTO_TEST_SUITE(UnaryOperator_test)

BOOST_FIXTURE_TEST_CASE(UnaryDouble_test, PythonFixture) {
  auto negative = py::eval("lambda x: -x", main_namespace);
  auto identity = py::eval("lambda x: +x", main_namespace);
  auto X = std::make_shared<Placeholder<double>>("x");

  auto py_negative_comp = negative(X);
  auto py_identiy_comp = identity(X);

  py::extract<std::shared_ptr<Node<double>>> negative_comp(py_negative_comp);
  py::extract<std::shared_ptr<Node<double>>> identity_comp(py_identiy_comp);

  BOOST_CHECK_EQUAL(negative_comp()->eval({{"x", 22.}}), -22.);
  BOOST_CHECK_EQUAL(negative_comp()->eval({{"x", 48.5}}), -48.5);

  BOOST_CHECK_EQUAL(identity_comp()->eval({{"x", 22.}}), 22.);
  BOOST_CHECK_EQUAL(identity_comp()->eval({{"x", 48.5}}), 48.5);
}

BOOST_FIXTURE_TEST_CASE(UnaryInt_test, PythonFixture) {
  auto negative = py::eval("lambda x: -x", main_namespace);
  auto identity = py::eval("lambda x: +x", main_namespace);
  auto X = std::make_shared<Placeholder<int64_t>>("x");

  auto py_negative_comp = negative(X);
  auto py_identiy_comp = identity(X);

  py::extract<std::shared_ptr<Node<int64_t>>> negative_comp(py_negative_comp);
  py::extract<std::shared_ptr<Node<int64_t>>> identity_comp(py_identiy_comp);

  BOOST_CHECK_EQUAL(negative_comp()->eval({{"x", 22l}}), -22);
  BOOST_CHECK_EQUAL(negative_comp()->eval({{"x", 48l}}), -48);

  BOOST_CHECK_EQUAL(identity_comp()->eval({{"x", 22l}}), 22);
  BOOST_CHECK_EQUAL(identity_comp()->eval({{"x", 48l}}), 48);
}

BOOST_FIXTURE_TEST_CASE(CastUnary_test, PythonFixture) {
  auto negative = py::eval("lambda x: -x", main_namespace);
  auto identity = py::eval("lambda x: +x", main_namespace);
  auto X = std::make_shared<Placeholder<int64_t>>("x");

  auto py_negative_comp = negative(X);
  auto py_identiy_comp = identity(X);

  py::extract<std::shared_ptr<Node<double>>> negative_comp(py_negative_comp);
  py::extract<std::shared_ptr<Node<double>>> identity_comp(py_identiy_comp);

  BOOST_CHECK_EQUAL(negative_comp()->eval({{"x", 22l}}), -22.);
  BOOST_CHECK_EQUAL(negative_comp()->eval({{"x", 48l}}), -48.);

  BOOST_CHECK_EQUAL(identity_comp()->eval({{"x", 22l}}), 22.);
  BOOST_CHECK_EQUAL(identity_comp()->eval({{"x", 48l}}), 48.);
}

BOOST_FIXTURE_TEST_CASE(Functions_test, PythonFixture) {
  auto log = py::eval("lambda x: np.log(x)", main_namespace);
  auto abs = py::eval("lambda x: np.abs(x)", main_namespace);
  auto cos = py::eval("lambda x: np.cos(x)", main_namespace);
  auto X = std::make_shared<Placeholder<double>>("x");

  auto py_log_comp = log(X);
  auto py_abs_comp = abs(X);
  auto py_cos_comp = cos(X);

  py::extract<std::shared_ptr<Node<double>>> log_comp(py_log_comp);
  py::extract<std::shared_ptr<Node<double>>> abs_comp(py_abs_comp);
  py::extract<std::shared_ptr<Node<double>>> cos_comp(py_cos_comp);

  BOOST_CHECK_CLOSE(log_comp()->eval({{"x", 100.}}), 4.6052, 1e-3);
  BOOST_CHECK(std::isnan(log_comp()->eval({{"x", -10.}})));
  BOOST_CHECK_CLOSE(abs_comp()->eval({{"x", 100.}}), 100., 1e-3);
  BOOST_CHECK_CLOSE(abs_comp()->eval({{"x", -543.}}), 543., 1e-3);
  BOOST_CHECK_CLOSE(cos_comp()->eval({{"x", 0.}}), 1., 1e-3);
  BOOST_CHECK_CLOSE(cos_comp()->eval({{"x", M_PI}}), -1., 1e-3);
}

BOOST_AUTO_TEST_SUITE_END()
