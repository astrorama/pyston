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

#ifndef PYSTON_CONSTANT_H
#define PYSTON_CONSTANT_H

#include "Node.h"

namespace Pyston {

template <typename T>
class Constant: public Node<T> {
public:
  Constant(T value): m_value{value} {}

  std::string repr() const override {
    return std::to_string(m_value);
  }

  T eval() const final {
    return m_value;
  }

private:
  T m_value;
};

} // end of namespace Pyston

#endif //PYSTON_CONSTANT_H