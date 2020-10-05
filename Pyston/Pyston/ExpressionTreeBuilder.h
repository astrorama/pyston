//
// Created by aalvarez on 05/10/2020.
//

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

namespace Pyston {

class ExpressionTreeBuilder {
public:

  template<typename R, typename ...Args>
  std::pair<bool, std::function<R(Args...)>> build(const boost::python::object& pyfunc) const {
    GILLocker gil_ensure;
    std::function<R(Args...)> functor;
    bool compiled;

    // Try building a computing graph
    try {
      boost::python::list placeholders;
      placeholderHelper<0, Args...>(placeholders);
      auto py_comp = pyfunc(*boost::python::tuple(placeholders));
      boost::python::extract<std::shared_ptr<Node<R>>> extractor(py_comp);
      auto comp = extractor();
      functor = [comp](Args ...args) {
        return comp->eval(args...);
      };
      compiled = true;
    }
    // If failed to do so (i.e. placeholder used on a control flow),
    // wrap the call to python
    catch (const boost::python::error_already_set&) {
      PyErr_Clear();
      functor = [pyfunc](Args ...args) -> R {
        GILLocker inner_gil_ensure;
        try {
          auto res = pyfunc(args...);
          return boost::python::extract<R>(res)();
        }
        catch (const boost::python::error_already_set&) {
          throw Exception();
        }
      };
      compiled = false;
    }
    return std::make_pair(compiled, functor);
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
};

} // end of namespace Pyston

#endif //PYSTON_EXPRESSIONTREEBUILDER_H
