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

#ifndef EVE_EXTRA_CONSTANTS_H_
#define EVE_EXTRA_CONSTANTS_H_

#include <eve/eve.hpp>

namespace eve_extra {

// Should dissapear
template <typename T, typename N, typename ABI>
auto iota(const eve::as_<eve::wide<T, N, ABI>>&) {
  return eve::wide<T, N, ABI>([](int i, int) { return i; });
}

}  // namespace eve_extra

#endif  // EVE_EXTRA_CONSTANTS_H_
