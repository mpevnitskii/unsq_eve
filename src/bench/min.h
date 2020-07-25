
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

#include "bench/bench.h"

#include <numeric>

namespace bench {

template <typename T>
struct min_parameters {
  std::vector<T> data;
};

struct min_driver {
  template <typename Slide, typename Alg, typename T>
  void operator()(Slide, benchmark::State&, Alg, min_parameters<T>&) const;
};

template <typename Slide, typename Alg, typename T>
BENCH_NOINLINE void min_driver::operator()(Slide slide, benchmark::State& state,
                                           Alg alg,
                                           min_parameters<T>& params) const {
  bench::noop_slide(slide);

  auto& data = params.data;

  for (auto _ : state) {
    auto v = alg(data.begin(), data.end());
    benchmark::DoNotOptimize(data);
    benchmark::DoNotOptimize(v);
  }
}

// Benchmarks ------------------------------------------------------
template <typename TestType, typename... Algorithms>
struct min_bench {
  const char* name() const { return "min"; }

  min_driver driver() const { return {}; }

  std::vector<std::size_t> sizes() const { return {40, 1000, 10'000}; }

  std::vector<std::size_t> percentage_points() const { return {100}; }

  bench::type_list<Algorithms...> algorithms() const { return {}; }

  bench::type_list<TestType> types() const { return {}; }

  template <typename T>
  auto input(struct bench::type_t<T>, std::size_t size,
             std::size_t /*percentage*/) const {
    std::size_t size_in_elements = size / sizeof(T);
    min_parameters<T> res;
    res.data.resize(size_in_elements);
    std::iota(res.data.rbegin(), res.data.rend(), T{0});
    return res;
  }
};

}  // namespace bench