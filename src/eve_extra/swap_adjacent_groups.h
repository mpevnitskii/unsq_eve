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

#ifndef EVE_EXTRA_SWAP_ADJACENT_H_
#define EVE_EXTRA_SWAP_ADJACENT_H_

#include <eve/eve.hpp>

#include <algorithm>
#include "eve_extra/concepts.h"
#include "eve_extra/map_array.h"

namespace eve_extra {
namespace _swap_adjacent_groups {

inline __m128i swap_adjacent_16_bytes_mask() {
  return _mm_set_epi8(14, 15, 12, 13, 10, 11, 8, 9, 6, 7, 4, 5, 2, 3, 0, 1);
}

// Gets optimized out to just loading a constant
inline __m256i swap_adjacent_32_bytes_mask() {
  return _mm256_set_m128i(swap_adjacent_16_bytes_mask(),
                          swap_adjacent_16_bytes_mask());
}

struct error_t {};

template <std::size_t byte_width, typename Register>
Register swap_adjacent_register(Register x) {
  static constexpr auto two_elements_4_parts_shuffle = _MM_SHUFFLE(1, 0, 3, 2);
  static constexpr auto four_element_shuffle = _MM_SHUFFLE(2, 3, 0, 1);

  if constexpr (byte_width == 1 && sizeof(Register) == 16) {
    return _mm_shuffle_epi8(x, swap_adjacent_16_bytes_mask());
  } else if constexpr (byte_width == 1 && sizeof(Register) == 32) {
    return _mm256_shuffle_epi8(x, swap_adjacent_32_bytes_mask());
  } else if constexpr (byte_width == 2 && sizeof(Register) == 16) {
    // Optimized to one load + shuffle
    x = _mm_shufflehi_epi16(x, four_element_shuffle);
    return _mm_shufflelo_epi16(x, four_element_shuffle);
  } else if constexpr (byte_width == 2 && sizeof(Register) == 32) {
    // Optimized to one load + shuffle
    x = _mm256_shufflehi_epi16(x, four_element_shuffle);
    return _mm256_shufflelo_epi16(x, four_element_shuffle);
  } else if constexpr (byte_width == 4 && sizeof(Register) == 16) {
    return _mm_shuffle_epi32(x, four_element_shuffle);
  } else if constexpr (byte_width == 4 && sizeof(Register) == 32) {
    return _mm256_shuffle_epi32(x, four_element_shuffle);
  } else if constexpr (byte_width == 8 && sizeof(Register) == 16) {
    return _mm_shuffle_epi32(x, two_elements_4_parts_shuffle);
  } else if constexpr (byte_width == 8 && sizeof(Register) == 32) {
    return _mm256_permute4x64_epi64(x, four_element_shuffle);
  } else if constexpr (byte_width == 16 && sizeof(Register) == 32) {
    return _mm256_permute4x64_epi64(x, two_elements_4_parts_shuffle);
  } else {
    return error_t{};
  }
}

}  // namespace _swap_adjacent_groups

template <std::size_t group_size, native_wide Wide>
Wide swap_adjacent_groups(const Wide& x) {
  using T = typename Wide::value_type;
  return Wide{
      _swap_adjacent_groups::swap_adjacent_register<group_size * sizeof(T)>(
          x.storage())};
}

template <std::size_t group_size, composite_wide Wide>
Wide swap_adjacent_groups(const Wide& x) {
  Wide y;

  const auto& [xl, xr] = x.storage().segments;
  auto& [yl, yr] = y.storage().segments;

  if constexpr (group_size == Wide::static_size / 2) {
    yl = xr;
    yr = xl;
  } else {
    yl = swap_adjacent_groups<group_size>(xl);
    yr = swap_adjacent_groups<group_size>(xr);
  }

  return y;
}

}  // namespace eve_extra

#endif  // EVE_EXTRA_SWAP_ADJACENT_H_
