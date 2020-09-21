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
#include "Pyston/Graph/Node.h"
#include "Pyston/Graph/Placeholder.h"
#include "PythonFixture.h"

using namespace Pyston;
namespace py = boost::python;

BOOST_AUTO_TEST_SUITE(UnaryOperator_test)

BOOST_FIXTURE_TEST_CASE(OpBoolFloat_test, PythonFixture) {
  auto function = py::eval("lambda x, y: (x > 0.) * y", main_namespace);
  auto X = std::make_shared<Placeholder<double>>("x");
  auto Y = std::make_shared<Placeholder<double>>("y");

  auto py_comp = function(X, Y);
  py::extract<std::shared_ptr<Node<double>>> comp(py_comp);

  BOOST_CHECK_EQUAL(comp()->eval({{"x", 4.}, {"y", 2.}}), 2.);
  BOOST_CHECK_EQUAL(comp()->eval({{"x", -4.}, {"y", 2.}}), 0.);
}

BOOST_FIXTURE_TEST_CASE(OpFloatBool_test, PythonFixture) {
  auto function = py::eval("lambda x, y: y * (x > 0.)", main_namespace);
  auto X = std::make_shared<Placeholder<double>>("x");
  auto Y = std::make_shared<Placeholder<double>>("y");

  auto py_comp = function(X, Y);
  py::extract<std::shared_ptr<Node<double>>> comp(py_comp);

  BOOST_CHECK_EQUAL(comp()->eval({{"x", 4.}, {"y", 2.}}), 2.);
  BOOST_CHECK_EQUAL(comp()->eval({{"x", -4.}, {"y", 2.}}), 0.);
}

BOOST_FIXTURE_TEST_CASE(OpInt_test, PythonFixture) {
  auto function = py::eval("lambda x, y: 5 * (x > 0.) + y", main_namespace);
  auto X = std::make_shared<Placeholder<double>>("x");
  auto Y = std::make_shared<Placeholder<double>>("y");

  auto py_comp = function(X, Y);
  py::extract<std::shared_ptr<Node<double>>> comp(py_comp);

  BOOST_CHECK_EQUAL(comp()->eval({{"x", 4.}, {"y", 2.}}), 7.);
  BOOST_CHECK_EQUAL(comp()->eval({{"x", -4.}, {"y", 2.}}), 2.);
}

BOOST_FIXTURE_TEST_CASE(OpIntReversed_test, PythonFixture) {
  auto function = py::eval("lambda x, y: y + (x > 0.) * 5", main_namespace);
  auto X = std::make_shared<Placeholder<double>>("x");
  auto Y = std::make_shared<Placeholder<double>>("y");

  auto py_comp = function(X, Y);
  py::extract<std::shared_ptr<Node<double>>> comp(py_comp);

  BOOST_CHECK_EQUAL(comp()->eval({{"x", 4.}, {"y", 2.}}), 7.);
  BOOST_CHECK_EQUAL(comp()->eval({{"x", -4.}, {"y", 2.}}), 2.);
}

BOOST_FIXTURE_TEST_CASE(AsBool_test, PythonFixture) {
  auto function = py::eval("lambda x: 0 if x else 1");
  auto X = std::make_shared<Placeholder<double>>("x");
  BOOST_CHECK_THROW(function(X), py::error_already_set);
}

BOOST_FIXTURE_TEST_CASE(Visit_test, PythonFixture) {
  auto function = py::eval("lambda x, y: y + (x > 0.) * 5", main_namespace);
  auto X = std::make_shared<Placeholder<double>>("x");
  auto Y = std::make_shared<Placeholder<double>>("y");

  auto py_comp = function(X, Y);
  py::extract<std::shared_ptr<Node<double>>> comp(py_comp);

  comp()->visit(text_visitor);
  BOOST_CHECK_EQUAL(text_stream.str(), "(y + double((long((x > 0.000000)) * 5)))");
}

BOOST_AUTO_TEST_SUITE_END()