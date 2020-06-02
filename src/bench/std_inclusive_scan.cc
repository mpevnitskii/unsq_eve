
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

#include "bench/sum.h"

#include <numeric>

namespace {

template <typename SumType>
struct std_inclusive_scan {
  std::string name() const {
    return std::string("std::inclusive_scan/sum_type:") +
           bench::type_name<SumType>{}();
  }

  template <typename I>
  SumType operator()(I f, I l) {
    std::inclusive_scan(f, l, f);
    return 0;
  }
};

}  // namespace

int main(int argc, char** argv) {
  bench::bench_main<bench::sum_bench<char, std_inclusive_scan<char>>,
                    bench::sum_bench<short, std_inclusive_scan<short>>,
                    bench::sum_bench<int, std_inclusive_scan<int>>>(argc, argv);
}
