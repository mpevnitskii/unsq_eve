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

#include <array>
#include <iostream>

#include "eve_extra/eve_extra.h"

#include "unsq_eve/iteration_unguarded.h"

namespace unsq_eve {
namespace _find {

template <typename Traits, typename I, typename PV>
// require ContigiousIterator<I> && VectorPredicate<PV, ValueType<I>>
struct find_if_body {
  using wide = eve::wide<ValueType<I>, eve::fixed<Traits::width()>>;

  PV p;
  I found;

  std::array<wide, Traits::unroll()> regs;

  find_if_body(PV p, I found) : p(p), found(found) {}

  template <typename Ptr, std::size_t idx, typename Ignore>
  bool operator()(Ptr from, indx_c<idx>, Ignore ignore) {
    regs[idx] = eve_extra::load_unsafe(from, eve::as_<wide>{});

    const std::optional match = first_true(p(regs[idx]), ignore);
    if (!match) return false;

    found = &*from + *match;
    return true;
  }
};

template <typename Traits, typename I, typename U>
auto equal_to(const U& y) {
  using T = ValueType<I>;
  using wide = eve::wide<T, eve::fixed<Traits::width()>>;
  wide ys{static_cast<T>(y)};
  return [ys](const wide& xs) { return xs == ys; };
}

}  // namespace _find

template <typename Traits, typename I, typename PV>
// require ContigiousIterator<I> && VectorPredicate<PV, ValueType<I>>
I find_if_unguarded(I f, PV p) {
  _find::find_if_body<Traits, I, PV> body{p, f};
  return iteration_aligned_unguarded<Traits>(f, body).found;
}

template <typename Traits, typename I, typename T>
// require ContigiousIterator<I> && Convertible<T, ValueType<P>>
I find_unguarded(I f, const T& x) {
  return unsq_eve::find_if_unguarded<Traits>(f, _find::equal_to<Traits, I>(x));
}

}  // namespace unsq_eve
