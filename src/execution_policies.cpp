// Companion to docs/execution_policies.md.
// Demonstrates std::execution policies with standard algorithms.
//
// Policies covered by the doc:
//   std::execution::seq        -- sequential
//   std::execution::par        -- parallel  (needs TBB linked on libstdc++)
//   std::execution::par_unseq  -- parallel + vectorised (needs TBB too)
//
// This system has no TBB installed, so only `seq` is called at runtime.
// The par / par_unseq snippets are shown in a #if 0 block so the file
// still compiles and links cleanly. Enable the block once libtbb-dev is
// installed and the target is linked against TBB::tbb.

#include <algorithm>
#include <execution>
#include <iostream>
#include <numeric>
#include <vector>

int main() {
  std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  // --- std::execution::seq: sequential ---
  int seq_sum = std::reduce(std::execution::seq, numbers.begin(), numbers.end(), 0);
  std::cout << "[seq]  reduce sum = " << seq_sum << '\n';

  // for_each with seq: prints in order
  std::cout << "[seq]  for_each: ";
  std::for_each(std::execution::seq, numbers.begin(), numbers.end(),
                [](int x) { std::cout << x << ' '; });
  std::cout << '\n';

  // transform with seq: double each element
  std::vector<int> doubled(numbers.size());
  std::transform(std::execution::seq, numbers.begin(), numbers.end(),
                 doubled.begin(), [](int x) { return x * 2; });
  std::cout << "[seq]  transform doubled[0..2] = " << doubled[0] << ' '
            << doubled[1] << ' ' << doubled[2] << '\n';

  // sort with seq
  std::vector<int> shuffled = {5, 2, 9, 1, 7, 3, 8, 4, 6, 0};
  std::sort(std::execution::seq, shuffled.begin(), shuffled.end());
  std::cout << "[seq]  sort first=" << shuffled.front()
            << " last=" << shuffled.back() << '\n';

  // --- par / par_unseq: require TBB at link time on libstdc++ ---
  // Uncomment the block below after installing libtbb-dev and linking
  // the target with TBB::tbb (find_package(TBB REQUIRED) in CMake).
#if 0
  int par_sum = std::reduce(std::execution::par, numbers.begin(), numbers.end(), 0);
  std::cout << "[par]  reduce sum = " << par_sum << '\n';

  int par_unseq_sum =
      std::reduce(std::execution::par_unseq, numbers.begin(), numbers.end(), 0);
  std::cout << "[par_unseq] reduce sum = " << par_unseq_sum << '\n';

  // par sort
  std::vector<int> v = {5, 2, 9, 1, 7, 3, 8, 4, 6, 0};
  std::sort(std::execution::par, v.begin(), v.end());
  std::cout << "[par]  sort first=" << v.front() << " last=" << v.back() << '\n';
#endif

  std::cout << "(par / par_unseq disabled: TBB not linked in this build)\n";
  return 0;
}
