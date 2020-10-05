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

#ifndef PYSTON_EXPRESSIONTREEBUILDER_H
#define PYSTON_EXPRESSIONTREEBUILDER_H

#include <functional>
#include <tuple>
#include <boost/python/list.hpp>
#include <boost/python/object.hpp>
#include <boost/python/tuple.hpp>

#include "Pyston/Exceptions.h"
#include "Pyston/GIL.h"
#include "Pyston/Graph/Placeholder.h"
#include "Pyston/Helpers.h"

namespace Pyston {

/**
 * Builds an expression tree from a Python function, given its signature.
 * If it is not possible to do so (for instance, due to the use of branching conditional
 * on some placeholder), it will wrap the Python function in a compatible manner
 */
class ExpressionTreeBuilder {
public:

  /**
   * Build an expression tree, or wrap the Python function in a compatible manner
   * @tparam Signature
   *    Function signature (i.e. double(double,double))
   * @param pyfunc
   *    Python object pointing to a callable. Its signature must match the template.
   * @return
   *    A pair, where the first value is a boolean set to `true` if an expression tree could
   *    be built, false otherwise. The second value is the wrapping functor.
   */
  template<typename Signature>
  std::pair<bool, std::function<Signature>> build(const boost::python::object& pyfunc) const {
    return buildHelper<Signature>::build(pyfunc);
  }

  /**
   * Register an unary function
   * @tparam T
   *    Parameter type
   * @tparam Functor
   *    Functor *type*
   * @param repr
   *    Function name
   * @note
   *    The return type is deduced from the signature of Functor<T>::operator()
   * @see
   *    UnaryWrapper for wrapping regular functions
   */
  template<typename T, template<class> class Functor>
  void registerFunction(const std::string& repr) {
    auto function = makeUnary<T, Functor>(repr);
    auto ns = boost::python::import("pyston");
    ns.attr(repr.c_str()) = function;
  }

  /**
   * Register a binary function
   * @tparam TL
   *    Left parameter type
   * @tparam TR
   *    Right parameter type
   * @tparam Functor
   *    Functor *type*
   * @param repr
   *    Function name
   * @note
   *    The return type is deduced from the signature of Functor<TL, TR>::operator()
   * @see
   *    BinaryWrapper for wrapping regular functions
   */
  template<typename TL, typename TR, template<class> class Functor>
  void registerFunction(const std::string& repr) {
    auto function = makeBinary<TL, TR, Functor>(repr);
    auto ns = boost::python::import("pyston");
    ns.attr(repr.c_str()) = function;
  }

private:
  template<unsigned pos>
  static void placeholderHelper(boost::python::list&) {
  }

  template<unsigned pos, typename A0>
  static void placeholderHelper(boost::python::list& placeholders) {
    placeholders.append(std::make_shared<Placeholder<A0>>(pos));
  }

  template<unsigned pos, typename A0, typename A1, typename ...AN>
  static void placeholderHelper(boost::python::list& placeholders) {
    placeholders.append(std::make_shared<Placeholder<A0>>(pos));
    placeholders.append(std::make_shared<Placeholder<A1>>(pos + 1));
    placeholderHelper<pos + 2, AN...>(placeholders);
  }

  /**
   * Required to support function signatures
   */
  template<typename Signature>
  struct buildHelper;

  /**
   * Specialization that "unwraps" the function signature into return type and arguments
   */
  template<typename R, typename... Args>
  struct buildHelper<R(Args...)> {
    static std::pair<bool, std::function<R(Args...)>> build(const boost::python::object& pyfunc);
  };
};

} // end of namespace Pyston

#define PYSTON_EXPRESSIONTREEBUILDER_IMPL

#include "Pyston/_impl/ExpressionTreeBuilder.icpp"

#undef PYSTON_EXPRESSIONTREEBUILDER_IMPL

#endif //PYSTON_EXPRESSIONTREEBUILDER_H
