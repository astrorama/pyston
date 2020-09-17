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

#ifndef PYSTON_PLACEHOLDER_H
#define PYSTON_PLACEHOLDER_H

#include "Node.h"

namespace Pyston {

template<typename T>
class Placeholder : public Node<T> {
public:
  Placeholder(const size_t id) : m_id{id} {
  }

  std::string repr() const override {
    return "_" + std::to_string(m_id);
  }

  T eval() const final {
    return m_value;
  }

  Placeholder& operator = (T value) {
    m_value = value;
    return *this;
  }

private:
  size_t m_id;
  T m_value;
};

} // end of namespace Pyston

#endif //PYSTON_PLACEHOLDER_H
