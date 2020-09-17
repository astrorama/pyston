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

#ifndef PYSTON_FUNCTORS_H
#define PYSTON_FUNCTORS_H

#include <cmath>

namespace Pyston {

template<typename T>
struct Identity {
  T operator()(T value) {
    return value;
  }
};

template<typename R, typename T, R(*wrapped)(T)>
struct UnaryWrapper {
  R operator()(T value) {
    return wrapped(value);
  }
};

template<typename R, typename T, R(*wrapped)(T, T)>
struct BinaryWrapper {
  R operator()(T left, T right) {
    return wrapped(left, right);
  }
};

template<typename T>
using Pow = BinaryWrapper<T, T, std::pow>;

template<typename T>
using Abs = UnaryWrapper<T, T, std::abs>;

template<typename T>
using Round = UnaryWrapper<T, T, std::round>;

template<typename T>
using Exp = UnaryWrapper<T, T, std::exp>;

template<typename T>
using Exp2 = UnaryWrapper<T, T, std::exp2>;

template<typename T>
using Log = UnaryWrapper<T, T, std::log>;

template<typename T>
using Log2 = UnaryWrapper<T, T, std::log2>;

template<typename T>
using Log10 = UnaryWrapper<T, T, std::log10>;

template<typename T>
using Sqrt = UnaryWrapper<T, T, std::sqrt>;

template<typename T>
using Sin = UnaryWrapper<T, T, std::sin>;

template<typename T>
using Cos = UnaryWrapper<T, T, std::cos>;

template<typename T>
using Tan = UnaryWrapper<T, T, std::tan>;

template<typename T>
using ArcSin = UnaryWrapper<T, T, std::asin>;

template<typename T>
using ArcCos = UnaryWrapper<T, T, std::acos>;

template<typename T>
using ArcTan = UnaryWrapper<T, T, std::atan>;

template<typename T>
using Sinh = UnaryWrapper<T, T, std::sinh>;

template<typename T>
using Cosh = UnaryWrapper<T, T, std::cosh>;

template<typename T>
using Tanh = UnaryWrapper<T, T, std::tanh>;

template<typename T>
using ArcSinh = UnaryWrapper<T, T, std::asinh>;

template<typename T>
using ArcCosh = UnaryWrapper<T, T, std::acosh>;

template<typename T>
using ArcTanh = UnaryWrapper<T, T, std::atanh>;

}

#endif //PYSTON_FUNCTORS_H
