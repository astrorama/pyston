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

#ifndef PYSTON_FUNCTION_H
#define PYSTON_FUNCTION_H

#include <boost/python/object.hpp>
#include <boost/python/list.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/extract.hpp>
#include "Pyston/Graph/Placeholder.h"
#include "Pyston/GIL.h"
#include "Pyston/Exceptions.h"


namespace Pyston {

/**
 * Class that wraps a Python object pointing to a callable:
 * - First, it will call it with a set of placeholders, as given by Args, and extract
 *   the root node with the type given by R. If it succeeds, this node evaluation method
 *   will be used on future calls, avoiding the trip to Python
 * - If it fails to do this (i.e. the function is too complicated, or one of the
 *   variables is used on a flow control statement), it will call Python when itself is called.
 *
 * Since the fallback mechanism can impact the performance, on the second case a flag (usesFallback)
 * is set so the caller method can either log a warning, abort execution, or see whatever it sees
 * fit
 * @tparam R
 *  Return type
 * @tparam Args
 *  Parameter types
 */
template<typename R, typename ...Args>
class Function {
public:
  /**
   * Destructor
   */
  virtual ~Function() = default;

  /**
   * Constructor
   * @param pyfunc
   *    Python callable to translate, or wrap if needed
   */
  Function(boost::python::object pyfunc) : m_fallback{false} {
    GILLocker gil_ensure;
    // Try building a computing graph
    try {
      boost::python::list placeholders;
      placeholderHelper<0, Args...>(placeholders);
      auto py_comp = pyfunc(*boost::python::tuple(placeholders));
      boost::python::extract<std::shared_ptr<Node<R>>> extractor(py_comp);
      auto comp = extractor();
      m_functor = [comp](Args ...args) {
        return comp->eval(args...);
      };
      m_compiled = comp;
    }
    // If failed to do so (i.e. placeholder used on a control flow),
    // wrap the call to python
    catch (const boost::python::error_already_set&) {
      PyErr_Clear();
      m_fallback = true;
      m_functor = [pyfunc](Args ...args) -> R {
        GILLocker inner_gil_ensure;
        try {
          auto res = pyfunc(args...);
          return boost::python::extract<R>(res)();
        }
        catch (const boost::python::error_already_set&) {
          throw Exception();
        }
      };
    }
  }

  /**
   * Copy constructor
   */
  Function(const Function&) = default;

  /**
   * Move constructor
   */
  Function(Function&&) = default;

  /**
   * @return
   *    true if the method is being called via Python
   */
  bool usesFallback() const {
    return m_fallback;
  }

  /**
   * Call the wrapped function
   * @param args
   *    Arguments for the function
   * @return
   *    Return value from the function
   */
  R operator()(Args&& ... args) {
    return m_functor(std::forward<Args>(args)...);
  }

  /**
   * Expose the compiled graph
   */
  std::shared_ptr<Node<R>> getCompiled() const {
    return m_compiled;
  }

private:
  unsigned m_nargs;
  bool m_fallback;
  std::function<R(Args...)> m_functor;
  std::shared_ptr<Node<R>> m_compiled;

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
};

} // end of namespace Pyston

#endif //PYSTON_FUNCTION_H
