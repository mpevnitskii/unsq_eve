/*
 * Copyright 2020 Denis Yaroshevskiy
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef EVE_EXTRA_FIRST_TRUE_ARRAY_H_
#define EVE_EXTRA_FIRST_TRUE_ARRAY_H_

#include <algorithm>
#include <array>
#include <optional>

#include <eve/detail/top_bits.hpp>
#include <eve/eve.hpp>
#include <eve/function/any.hpp>
#include <eve/function/logical_or.hpp>

#include "eve_extra/concepts.h"

namespace eve_extra {

/*
Compiler does not generate this properly, instead of doing things one
after another if you ask it.
*/

template <typename T, typename Op>
T segment_reduction(const std::array<T, 1>& arr, Op) {
  return arr[0];
}

template <typename T, typename Op>
T segment_reduction(const std::array<T, 2>& arr, Op op) {
  return op(arr[0], arr[1]);
}

template <typename T, typename Op>
T segment_reduction(const std::array<T, 4>& arr, Op op) {
  T x = op(arr[0], arr[2]);
  T y = op(arr[1], arr[3]);
  return op(x, y);
}

template <typename T, typename Op>
T segment_reduction(const std::array<T, 8>& arr, Op op) {
  T x1 = op(arr[0], arr[4]);
  T x2 = op(arr[1], arr[5]);
  T x3 = op(arr[2], arr[6]);
  T x4 = op(arr[3], arr[7]);
  x1 = op(x1, x2);
  x3 = op(x3, x4);
  return op(x1, x3);
}

template <eve_logical Logical, std::size_t N>
bool any_array(const std::array<Logical, N>& xs) {
  return eve::any(segment_reduction(xs, eve::logical_or));
}

namespace _first_true {

template <eve_logical Logical, std::size_t N>
auto move_masks(const std::array<Logical, N>& regs) {
  std::array<eve::detail::top_bits<Logical>, N> mmasks;

  std::transform(regs.begin(), regs.end(), mmasks.begin(),
                 [](auto reg) { return eve::detail::top_bits(reg); });

  return mmasks;
}

}  // namespace _first_true

template <eve_logical Logical, typename Ignore>
bool any(const Logical& logical, Ignore ignore) {
  eve::detail::top_bits mmask(logical, ignore);
  return eve::detail::any(mmask);
}

template <eve_logical Logical, typename Ignore>
std::optional<std::ptrdiff_t> first_true(Logical logical, Ignore ignore) {
  eve::detail::top_bits mmask(logical, ignore);
  return eve::detail::first_true(mmask);
}

template <eve_logical Logical>
std::optional<std::ptrdiff_t> first_true_array(
    const std::array<Logical, 1>& xs) {
  return first_true(xs[0], eve::ignore_none);
}

template <eve_logical Logical, std::size_t N>
std::optional<std::ptrdiff_t> first_true_array(
    const std::array<Logical, N>& xs) {
  if (!any_array(xs)) return {};

  auto mmasks = _first_true::move_masks(xs);

  std::uint32_t offset;
  eve::detail::top_bits<Logical> mmask;

  if constexpr (N == 2) {
    offset = 1;
    mmask = mmasks[1];
  } else {
    offset = 3;
    mmask = mmasks[3];

    if (mmasks[2]) {
      offset = 2;
      mmask = mmasks[2];
    }
    if (mmasks[1]) {
      offset = 1;
      mmask = mmasks[1];
    }
  }

  if (mmasks[0]) {
    offset = 0;
    mmask = mmasks[0];
  }

  offset = mmask.static_size * offset;
  return *eve::detail::first_true(mmask) + offset;
}

}  // namespace eve_extra

#endif  // EVE_EXTRA_FIRST_TRUE_ARRAY_H_
