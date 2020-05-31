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

#ifndef UNSQ_EVE_ITERATION_TRAITS_H_
#define UNSQ_EVE_ITERATION_TRAITS_H_

#include <iterator>
#include <utility>

#include "eve_extra/eve_extra.h"
#include "unsq_eve/concepts.h"

namespace unsq_eve {

template <std::size_t _chunk_size, std::size_t _unroll>
struct iteration_traits {
  static constexpr indx_c<_chunk_size> chunk_size{};
  static constexpr indx_c<_unroll> unroll{};

  template <typename T>
  static auto previous_aligned_address(T* addr) {
    return eve_extra::previous_aligned_address<sizeof(T) * chunk_size()>(addr);
  }
};

template <typename T, std::size_t register_bit_width, std::size_t _unroll>
struct algorithm_traits {
  using width_type = eve::fixed<register_bit_width / sizeof(T) / 8>;
  static constexpr width_type width{};
  static constexpr indx_c<_unroll> unroll{};

  using wide = eve::wide<T, width_type>;
  using iteration_traits =
      unsq_eve::iteration_traits<wide::static_size, unroll()>;
};

template <typename ATraits>
using iteration_traits_t = typename ATraits::iteration_traits;

template <typename I>
using Pointer = typename std::iterator_traits<I>::pointer;

template <contigious_iterator I>
Pointer<I> drill_down(I f) {
  return &*f;
}

template <contigious_iterator I>
std::pair<Pointer<I>, Pointer<I>> drill_down_range(I f, I l) {
  auto _f = drill_down(f);
  return {_f, _f + (l - f)};
}

template <contigious_iterator I, typename T>
// require ContigiousIterator<I>
I undo_drill_down(I f, T* ptr) {
  return f + (ptr - drill_down(f));
}

enum class StopReason {
  No,
  EndReached,
  Terminated,
};

}  // namespace unsq_eve

#endif  // UNSQ_EVE_ITERATION_TRAITS_H_
