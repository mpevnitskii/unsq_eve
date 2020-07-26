
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

#include <random>

namespace bench {

template <typename T>
struct remove_params {
  std::vector<T> data;
  std::vector<T> buffer;  // buffer.size() == data.size()
  T x;
};

struct remove_driver {
  template <typename Slide, typename Alg, typename T>
  void operator()(Slide, benchmark::State&, Alg, remove_params<T>&) const;
};

template <typename Slide, typename Alg, typename T>
BENCH_NOINLINE void remove_driver::operator()(Slide slide,
                                              benchmark::State& state, Alg alg,
                                              remove_params<T>& params) const {
  bench::noop_slide(slide);

  auto& [data, buffer, x] = params;

  for (auto _ : state) {
    std::copy(data.begin(), data.end(), buffer.begin());
    alg(buffer.begin(), buffer.end(), x);
    benchmark::DoNotOptimize(buffer);
  }
}

// Benchmarks ------------------------------------------------------

template <typename TestType, typename... Algorithms>
struct remove_bench {
  const char* name() const { return "remove 0"; }

  remove_driver driver() const { return {}; }

  std::vector<std::size_t> sizes() const { return {40, 1000, 10'000}; }

  std::vector<std::size_t> percentage_points() const {
    return {0, 5, 20, 50, 80, 95, 100};
  }

  bench::type_list<Algorithms...> algorithms() const { return {}; }

  bench::type_list<TestType> types() const { return {}; }

  template <typename T>
  auto input(struct bench::type_t<T>, std::size_t size,
             std::size_t percentage) const {
    std::vector<T> data(size, 1);
    std::fill(data.begin(), data.begin() + size * percentage / 100, 0);
    std::shuffle(data.begin(), data.end(), std::mt19937{});

    std::size_t size_in_elements = size / sizeof(T);
    return remove_params<T>{data, std::vector<T>(size_in_elements), 0};
  }
};

}  // namespace bench
