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

BOOST_AUTO_TEST_SUITE(FullChain_test)

BOOST_FIXTURE_TEST_CASE(OperatorChain_test, PythonFixture) {
  auto chain = py::eval("lambda x, y, z: 2 * x + y * 1.5 - z / 3", main_namespace);
  auto X = std::make_shared<Placeholder<double>>("x");
  auto Y = std::make_shared<Placeholder<double>>("y");
  auto Z = std::make_shared<Placeholder<double>>("z");

  auto py_comp = chain(X, Y, Z);
  py::extract<std::shared_ptr<Node<double>>> comp(py_comp);

  BOOST_CHECK_CLOSE(comp()->eval({{"x", 1.}, {"y", 2.}, {"z", 3.}}), 4.0, 1e-5);
}

BOOST_FIXTURE_TEST_CASE(FuncChain_test, PythonFixture) {
  auto chain = py::eval("lambda x, y, z: 2 ** np.log(x) + np.cos(y * 1.5) - np.exp(z) / 3",
                        main_namespace);
  auto X = std::make_shared<Placeholder<double>>("x");
  auto Y = std::make_shared<Placeholder<double>>("y");
  auto Z = std::make_shared<Placeholder<double>>("z");

  auto py_comp = chain(X, Y, Z);
  py::extract<std::shared_ptr<Node<double>>> comp(py_comp);

  BOOST_CHECK_CLOSE(comp()->eval({{"x", 4.}, {"y", 2.}, {"z", 3.}}), -5.0711076556, 1e-5);
}

BOOST_FIXTURE_TEST_CASE(ChainWithCast_test, PythonFixture) {
  auto chain = py::eval("lambda x, y, z: x * (z > 0.) + y * (z <= 0.)", main_namespace);
  auto X = std::make_shared<Placeholder<double>>("x");
  auto Y = std::make_shared<Placeholder<double>>("y");
  auto Z = std::make_shared<Placeholder<double>>("z");

  auto py_comp = chain(X, Y, Z);
  py::extract<std::shared_ptr<Node<double>>> comp(py_comp);

  BOOST_CHECK_EQUAL(comp()->eval({{"x", 4.}, {"y", 2.}, {"z", 3.}}), 4.);
  BOOST_CHECK_EQUAL(comp()->eval({{"x", 4.}, {"y", 2.}, {"z", -3.}}), 2.);
}

BOOST_FIXTURE_TEST_CASE(Visit_test, PythonFixture) {
  auto chain = py::eval("lambda x, y, z: 2 ** np.log(x) + np.cos(y * 1.5) - np.exp(z) / 3",
                        main_namespace);
  auto X = std::make_shared<Placeholder<double>>("x");
  auto Y = std::make_shared<Placeholder<double>>("y");
  auto Z = std::make_shared<Placeholder<double>>("z");

  auto py_comp = chain(X, Y, Z);
  py::extract<std::shared_ptr<Node<double>>> comp(py_comp);

  comp()->visit(text_visitor);
  BOOST_CHECK_EQUAL(text_stream.str(), "(((2.000000 ^ log(x)) + cos((y * 1.500000))) - (exp(z) / 3.000000))");
}

BOOST_AUTO_TEST_SUITE_END()
