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

#ifndef PYSTON_PYTHONFIXTURE_H
#define PYSTON_PYTHONFIXTURE_H

#include <iomanip>
#include <boost/python/object.hpp>
#include <boost/python/import.hpp>
#include <boost/python/exec.hpp>
#include <boost/python/extract.hpp>
#include "Pyston/Module.h"
#include "Pyston/Util/TextReprVisitor.h"

namespace Pyston {

struct PythonFixture {

  struct Singleton {
    Singleton() {
      if (PyImport_AppendInittab("pyston", &PyInit_libPyston) == -1)
        abort();
      Py_Initialize();
      PyEval_InitThreads();
      PyEval_SaveThread();
    }
  };

  boost::python::object main_namespace;
  std::stringstream text_stream;
  TextReprVisitor text_visitor;
  PyGILState_STATE gil_state;

  PythonFixture() : text_visitor{text_stream} {
    static Singleton singleton;
    gil_state = PyGILState_Ensure();
    auto main_module = boost::python::import("__main__");
    main_namespace = main_module.attr("__dict__");
    boost::python::import("pyston");
    main_namespace["np"] = boost::python::import("numpy");
    main_namespace["np"] = boost::python::import("numpy");
  }

  ~PythonFixture() {
    PyGILState_Release(gil_state);
  }
};

} // end of namespace Pyston

#endif //PYSTON_PYTHONFIXTURE_H
