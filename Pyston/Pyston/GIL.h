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

#ifndef PYSTON_GIL_H
#define PYSTON_GIL_H

#include <Python.h>

namespace Pyston {

/**
 * RAII for the Global Interlock: Acquires at construction and releases at destruction
 */
struct GILLocker {
  GILLocker() {
    m_state = PyGILState_Ensure();
  }

  ~GILLocker() {
    PyGILState_Release(m_state);
  }

  PyGILState_STATE m_state;
};

/**
 * RAII for the Global Interlock: Releases at construction and locks at destruction
 */
struct GILReleaser {
  GILReleaser(PyGILState_STATE& state) : m_state(state) {
    PyGILState_Release(m_state);
  }

  ~GILReleaser() {
    m_state = PyGILState_Ensure();
  }

  PyGILState_STATE& m_state;
};

}  // end of namespace Pyston

#endif  // PYSTON_GIL_H
