// Companion to docs/algorithms.md. Each function corresponds to one section of
// the doc; main() runs them in order so the binary produces meaningful output.
//
// Build target: algorithms_library  (C++20, pthread)
// Examples are framed around robotics: joint errors, lidar scans, motor torque,
// path-planning cells.

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <execution>
#include <functional>
#include <iostream>
#include <iterator>
#include <list>
#include <numeric>
#include <queue>
#include <random>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

// ---------- small printing helpers ----------

template <typename Range> void print_range(std::string_view label, const Range &r) {
  std::cout << label;
  for (const auto &x : r) std::cout << x << ' ';
  std::cout << '\n';
}

template <typename PQ> void drain_pq(std::string_view label, PQ q) {
  std::cout << label;
  while (!q.empty()) { std::cout << q.top() << ' '; q.pop(); }
  std::cout << '\n';
}

// =====================================================================
// Section 1. Non-modifying sequence operations
// =====================================================================
void non_modifying() {
  std::cout << "\n--- 1. Non-modifying ---\n";

  // count_if: how many joints exceed 1 degree of tracking error.
  std::vector<double> error_deg = {0.2, -1.5, 0.4, 2.0, -0.8, 1.1};
  auto bad = std::count_if(error_deg.begin(), error_deg.end(),
                           [](double e) { return std::abs(e) > 1.0; });
  std::cout << "joints with |err| > 1 deg: " << bad << '\n';

  // find / find_if on an unsorted vector.
  std::vector<int> nums = {2, 3, 1, 0, -4, 7, 5, 3};
  auto it = std::find(nums.begin(), nums.end(), 3);
  std::cout << "first 3 at index " << std::distance(nums.begin(), it) << '\n';
  auto it2 = std::find_if(nums.begin(), nums.end(),
                          [](int x) { return x > 3; });
  std::cout << "first > 3 at index " << std::distance(nums.begin(), it2) << '\n';

  // find_first_of: first element from set B inside A.
  std::vector<int> targets = {87, 5, 43};
  auto f = std::find_first_of(nums.begin(), nums.end(),
                              targets.begin(), targets.end());
  std::cout << "first of {87,5,43} at index "
            << std::distance(nums.begin(), f) << '\n';

  // adjacent_find with a predicate (n followed by n+2).
  std::vector<int> seq = {-1, -2, 3, 4, 6, -4, 7, 5, 3};
  auto a = std::adjacent_find(seq.begin(), seq.end(),
                              [](int x, int y) { return y == x + 2; });
  std::cout << "first (n, n+2) at index "
            << std::distance(seq.begin(), a) << '\n';

  // search / search_n.
  std::vector<int> log = {2, 3, 3, 0, -4, 7, 5, 3};
  auto s1 = std::search_n(log.begin(), log.end(), 2, 3);
  std::cout << "two consecutive 3s at index "
            << std::distance(log.begin(), s1) << '\n';
  std::vector<int> sub = {0, -4, 7};
  auto s2 = std::search(log.begin(), log.end(), sub.begin(), sub.end());
  std::cout << "subseq {0,-4,7} at index "
            << std::distance(log.begin(), s2) << '\n';

  // equal vs is_permutation.
  std::vector<int> a1 = {-1, -2, 3, 4, 0};
  std::vector<int> a2 = {-1, -2, 3, 4, 0};
  std::vector<int> a3 = {-2, -1, 3, 4, 0};
  std::cout << "equal a1==a2: "
            << std::equal(a1.begin(), a1.end(), a2.begin()) << '\n';
  std::cout << "is_permutation a1<->a3: "
            << std::is_permutation(a1.begin(), a1.end(), a3.begin()) << '\n';

  // mismatch.
  std::vector<int> b1 = {-1, -2,  3, 4, 0};
  std::vector<int> b2 = {-1, -2, -3, 4, 0};
  auto m = std::mismatch(b1.begin(), b1.end(), b2.begin());
  std::cout << "first mismatch at index "
            << std::distance(b1.begin(), m.first)
            << " (" << *m.first << " vs " << *m.second << ")\n";

  // any_of / all_of / none_of quantifiers.
  std::vector<double> torques = {0.1, 0.2, 0.05, 0.4};
  bool any_overload = std::any_of(torques.begin(), torques.end(),
                                  [](double t) { return t > 1.0; });
  bool all_positive = std::all_of(torques.begin(), torques.end(),
                                  [](double t) { return t >= 0.0; });
  std::cout << "any overload: " << any_overload
            << ", all non-negative: " << all_positive << '\n';

  // for_each: the lambda's return value is discarded.
  std::vector<int> v = {0, 1, 2, 3, 4, 5};
  std::for_each(v.begin(), v.end(), [](int x) { return x * 3; }); // no-op
  print_range("for_each return-discarded: ", v);
  std::for_each(v.begin(), v.end(), [](int &x) { ++x; });          // mutates
  print_range("for_each by-ref:           ", v);
}

// =====================================================================
// Section 2. Modifying sequence operations
// =====================================================================
void modifying() {
  std::cout << "\n--- 2. Modifying ---\n";

  // copy / copy_n / copy_backward
  std::vector<int> src = {1, 2, 3};
  std::vector<int> dst(4, 0);
  std::copy(src.begin(), src.end(), dst.begin());
  print_range("copy:        ", dst);

  // transform: unary and binary.
  std::vector<int> in = {0, 1, 2, 3, 4};
  std::vector<int> sq(in.size());
  std::transform(in.begin(), in.end(), sq.begin(),
                 [](int x) { return x * x; });
  print_range("transform sq: ", sq);

  std::vector<int> sum(in.size());
  std::transform(in.begin(), in.end(), sq.begin(), sum.begin(),
                 [](int a, int b) { return a + b; });
  print_range("transform sum:", sum);

  // replace / replace_if.
  std::vector<int> r = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::replace(r.begin(), r.end(), 0, 1);
  std::replace_if(r.begin(), r.end(),
                  [](int x) { return x > 2; }, -1);
  print_range("replace_if:   ", r);

  // Erase-remove idiom vs C++20 std::erase_if.
  std::vector<int> v1 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  v1.erase(std::remove_if(v1.begin(), v1.end(),
                          [](int x) { return x % 2 == 0; }),
           v1.end());
  print_range("erase-remove odd: ", v1);

  std::vector<int> v2 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::erase_if(v2, [](int x) { return x % 2 == 0; }); // C++20
  print_range("erase_if odd:     ", v2);

  // unique: only collapses consecutive duplicates; sort first for true dedup.
  std::vector<int> u = {2, 1, 5, 2, 2, 3, 1, 1, 7};
  auto last = std::unique(u.begin(), u.end());
  u.erase(last, u.end());
  print_range("unique on unsorted (consecutive only): ", u);

  std::vector<int> u2 = {2, 1, 5, 2, 2, 3, 1, 1, 7};
  std::sort(u2.begin(), u2.end());
  u2.erase(std::unique(u2.begin(), u2.end()), u2.end());
  print_range("sort+unique (true dedup):              ", u2);

  // Collapse consecutive whitespace.
  std::string s = "wanna go    to      space?";
  auto e = std::unique(s.begin(), s.end(),
                       [](char l, char r) { return std::isspace(static_cast<unsigned char>(l))
                                                && std::isspace(static_cast<unsigned char>(r)); });
  std::cout << "compact spaces: \"" << std::string(s.begin(), e) << "\"\n";

  // rotate.
  std::vector<int> rot = {1, 2, 3, 4};
  std::rotate(rot.begin(), rot.begin() + 1, rot.end());
  print_range("rotate left:  ", rot);
  std::rotate(rot.rbegin(), rot.rbegin() + 1, rot.rend());
  print_range("rotate right: ", rot);

  // shift_left as a sliding IMU window: drop the oldest, append the newest.
  std::array<double, 8> imu_window{1, 2, 3, 4, 5, 6, 7, 8};
  std::shift_left(imu_window.begin(), imu_window.end(), 1);
  imu_window.back() = 9.0;
  print_range("imu_window after shift_left+append: ", imu_window);

  // fill / generate.
  std::vector<int> f(6, 0);
  std::fill(f.begin(), f.end(), 1);
  std::fill_n(f.begin(), 2, 0);
  print_range("fill+fill_n:  ", f);

  std::mt19937 rng(42);
  std::uniform_int_distribution<int> dist(0, 99);
  std::vector<int> g(5);
  std::generate(g.begin(), g.end(), [&]() { return dist(rng); });
  print_range("generate:     ", g);

  // swap two vectors.
  std::vector<int> x = {1, 2, 3}, y = {9, 8, 7, 6};
  std::swap(x, y);
  print_range("after swap x: ", x);
  print_range("after swap y: ", y);
}

// std::exchange in a move constructor (doc's "Handle" example for a fd).
struct Handle {
  int fd;
  explicit Handle(int f) : fd(f) {}
  Handle(Handle &&other) noexcept : fd(std::exchange(other.fd, -1)) {}
  Handle &operator=(Handle &&other) noexcept {
    if (this != &other) fd = std::exchange(other.fd, -1);
    return *this;
  }
};

void exchange_demo() {
  std::cout << "\n--- exchange in move ctor ---\n";
  Handle h1{42};
  Handle h2{std::move(h1)};
  std::cout << "h1.fd=" << h1.fd << "  h2.fd=" << h2.fd << '\n';
}

// =====================================================================
// Section 3. Partitioning
// =====================================================================
void partitioning() {
  std::cout << "\n--- 3. Partitioning ---\n";

  // Treat 'range < 5' as the obstacle predicate.
  struct Point { double range; };
  std::vector<Point> points = {{2.1}, {7.3}, {0.9}, {12.0}, {4.5}, {6.2}, {3.0}};
  auto bound = std::partition(points.begin(), points.end(),
                              [](const Point &p) { return p.range < 5.0; });
  std::cout << "close-range points: ";
  for (auto it = points.begin(); it != bound; ++it) std::cout << it->range << ' ';
  std::cout << "\nfar points:         ";
  for (auto it = bound; it != points.end(); ++it) std::cout << it->range << ' ';
  std::cout << '\n';

  std::cout << "is_partitioned: "
            << std::is_partitioned(points.begin(), points.end(),
                                   [](const Point &p) { return p.range < 5.0; })
            << '\n';
}

// =====================================================================
// Section 4. Sorting
// =====================================================================
void sorting() {
  std::cout << "\n--- 4. Sorting ---\n";

  std::vector<int> v = {0, 1, 3, 6, 9, 8, 7};
  std::sort(v.begin(), v.end()); // default std::less<>
  print_range("sort ascending: ", v);

  std::vector<int> w = {10, 20, 50, 30, 40};
  auto stop = std::is_sorted_until(w.begin(), w.end());
  std::cout << "is_sorted_until index: "
            << std::distance(w.begin(), stop) << '\n';

  // Top-K closest obstacles, sorted by range (partial_sort).
  struct Obstacle { int id; double range; };
  std::vector<Obstacle> obs = {{1, 9.5}, {2, 0.4}, {3, 6.1},
                               {4, 2.0}, {5, 1.3}, {6, 7.7}};
  constexpr int K = 4;
  std::partial_sort(obs.begin(), obs.begin() + K, obs.end(),
                    [](const Obstacle &a, const Obstacle &b) {
                      return a.range < b.range;
                    });
  std::cout << "4 closest obstacles: ";
  for (int i = 0; i < K; ++i) std::cout << obs[i].id << "(" << obs[i].range << ") ";
  std::cout << '\n';

  // Median via nth_element — the O(n) average pick for median filters.
  std::vector<int> samples = {3, 7, 4, 9, 2, 5, 8, 1, 6};
  auto mid = samples.begin() + samples.size() / 2;
  std::nth_element(samples.begin(), mid, samples.end());
  std::cout << "median sample: " << *mid << '\n';

  // stable_partition preserves original order inside each side.
  std::vector<int> sp = {5, 3, 9, 1, 4, 8, 2, 7};
  std::stable_partition(sp.begin(), sp.end(),
                        [](int x) { return x < 5; });
  print_range("stable_partition(<5): ", sp);
}

// =====================================================================
// Section 5. Binary search on sorted ranges
// =====================================================================
void binary_search_demo() {
  std::cout << "\n--- 5. Binary search ---\n";

  std::vector<double> ts = {0.1, 0.2, 0.35, 0.5, 0.5, 0.7, 1.0};
  double t_query = 0.5;
  auto lo = std::lower_bound(ts.begin(), ts.end(), t_query);
  auto hi = std::upper_bound(ts.begin(), ts.end(), t_query);
  std::cout << "lower_bound(0.5) at " << std::distance(ts.begin(), lo)
            << ", upper_bound(0.5) at " << std::distance(ts.begin(), hi) << '\n';
  std::cout << "binary_search(0.35): "
            << std::binary_search(ts.begin(), ts.end(), 0.35) << '\n';

  auto eq = std::equal_range(ts.begin(), ts.end(), 0.5);
  std::cout << "equal_range(0.5): ["
            << std::distance(ts.begin(), eq.first) << ", "
            << std::distance(ts.begin(), eq.second) << ")\n";
}

// =====================================================================
// Section 6. Set operations on sorted ranges
// =====================================================================
void set_operations() {
  std::cout << "\n--- 6. Set operations ---\n";

  struct Sample { double t; int src; };
  std::vector<Sample> imu     = {{0.10, 0}, {0.30, 0}, {0.50, 0}};
  std::vector<Sample> encoder = {{0.15, 1}, {0.20, 1}, {0.55, 1}};
  std::vector<Sample> fused;
  fused.reserve(imu.size() + encoder.size());
  std::merge(imu.begin(), imu.end(),
             encoder.begin(), encoder.end(),
             std::back_inserter(fused),
             [](const Sample &a, const Sample &b) { return a.t < b.t; });
  std::cout << "merged timeline: ";
  for (auto &s : fused) std::cout << '(' << s.t << ',' << s.src << ") ";
  std::cout << '\n';

  std::vector<int> a = {1, 3, 5, 7, 9};
  std::vector<int> b = {2, 3, 5, 8};
  std::vector<int> out;
  std::set_intersection(a.begin(), a.end(), b.begin(), b.end(),
                        std::back_inserter(out));
  print_range("set_intersection: ", out);

  std::vector<int> probe = {3, 5};
  std::cout << "includes({3,5} in a): "
            << std::includes(a.begin(), a.end(),
                             probe.begin(), probe.end())
            << '\n';
}

// =====================================================================
// Section 7. Heap and priority_queue
// =====================================================================
struct Cell {
  int idx;
  float cost;
  bool operator<(const Cell &o) const { return cost < o.cost; } // max-heap on cost
};

void heap_demo() {
  std::cout << "\n--- 7. Heap ---\n";

  std::vector<int> v = {6, 10, 7, 17, 10, 15};
  std::make_heap(v.begin(), v.end()); // max-heap by default; std::less<> is OK
  print_range("max-heap:           ", v);
  std::cout << "top: " << v.front() << '\n';

  v.push_back(100);
  std::push_heap(v.begin(), v.end());
  print_range("after push 100:     ", v);
  std::pop_heap(v.begin(), v.end());
  v.pop_back();
  print_range("after pop:          ", v);
  std::sort_heap(v.begin(), v.end());
  print_range("sort_heap ascending:", v);

  // A* / Dijkstra open set as a min-heap on cost.
  struct ByCost {
    bool operator()(const Cell &a, const Cell &b) const { return a.cost > b.cost; }
  };
  std::priority_queue<Cell, std::vector<Cell>, ByCost> open;
  open.push({1, 5.0f});
  open.push({2, 2.5f});
  open.push({3, 7.5f});
  std::cout << "A* open pops (cheapest first): ";
  while (!open.empty()) {
    std::cout << '(' << open.top().idx << ',' << open.top().cost << ") ";
    open.pop();
  }
  std::cout << '\n';

  // Lambda comparator with decltype.
  auto by_cost = [](const Cell &a, const Cell &b) { return a.cost > b.cost; };
  std::priority_queue<Cell, std::vector<Cell>, decltype(by_cost)> open2(by_cost);
  open2.push({10, 4.0f});
  open2.push({11, 1.0f});
  std::cout << "lambda PQ top: " << open2.top().idx << '\n';

  // Default operator< -> max-heap on cost.
  std::priority_queue<Cell> pq;
  pq.push({1, 5.0f});
  pq.push({2, 9.0f});
  pq.push({3, 3.0f});
  std::cout << "max-PQ top (cost): " << pq.top().cost << '\n';
}

// =====================================================================
// Section 8. Min / Max / Clamp
// =====================================================================
void min_max_clamp() {
  std::cout << "\n--- 8. Min/Max/Clamp ---\n";

  std::vector<int> nums = {20, 30, 10, 0, -40, 7, 5, 3};
  auto mn = std::min_element(nums.begin(), nums.end());
  auto mx = std::max_element(nums.begin(), nums.end());
  auto mm = std::minmax_element(nums.begin(), nums.end());
  std::cout << "min=" << *mn << " max=" << *mx
            << " minmax=(" << *mm.first << "," << *mm.second << ")\n";

  // Saturate a motor command before sending it on the CAN bus.
  constexpr double MAX_TORQUE = 5.0;
  double controller_output = 8.7;
  double cmd = std::clamp(controller_output, -MAX_TORQUE, MAX_TORQUE);
  std::cout << "clamp(8.7, +/-5) = " << cmd << '\n';

  // Lifetime trap: take min by value, not by const-ref, for the init-list overload.
  auto m_ok = std::min({1, 2, 3});
  std::cout << "min({1,2,3}) by value: " << m_ok << '\n';
}

// =====================================================================
// Section 9. Comparison
// =====================================================================
void comparison() {
  std::cout << "\n--- 9. Comparison ---\n";

  std::vector<int> s1 = {1, 2, 3};
  std::vector<int> s2 = {1, 2, 4};
  bool less = std::lexicographical_compare(s1.begin(), s1.end(),
                                           s2.begin(), s2.end());
  std::cout << "{1,2,3} < {1,2,4}: " << less << '\n';

  s2 = {1, 3};
  less = std::lexicographical_compare(s1.begin(), s1.end(),
                                      s2.begin(), s2.end());
  std::cout << "{1,2,3} < {1,3}:   " << less << '\n';
}

// =====================================================================
// Section 10. Permutation / shuffle / sample
// =====================================================================
void permutation_shuffle_sample() {
  std::cout << "\n--- 10. Permutation / shuffle / sample ---\n";

  int arr[] = {1, 2, 3};
  std::sort(std::begin(arr), std::end(arr));
  std::cout << "all 3! permutations:\n";
  do {
    std::cout << "  " << arr[0] << ' ' << arr[1] << ' ' << arr[2] << '\n';
  } while (std::next_permutation(std::begin(arr), std::end(arr)));

  // shuffle with a real generator.
  std::vector<int> v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::mt19937 g{std::random_device{}()};
  std::shuffle(v.begin(), v.end(), g);
  print_range("shuffled: ", v);

  // Downsample a dense lidar cloud — k samples without replacement.
  std::vector<int> cloud(50);
  std::iota(cloud.begin(), cloud.end(), 0);
  std::vector<int> sparse;
  sparse.reserve(10);
  std::sample(cloud.begin(), cloud.end(),
              std::back_inserter(sparse), 10, g);
  print_range("sample(k=10):  ", sparse);
}

// =====================================================================
// Section 11. Numeric operations
// =====================================================================
void numeric_ops() {
  std::cout << "\n--- 11. Numeric ---\n";

  constexpr int N = 8;
  std::vector<double> x(N);
  std::iota(x.begin(), x.end(), 0.0);
  print_range("iota: ", x);

  double acc = std::accumulate(x.begin(), x.end(), 0.0);
  double red = std::reduce(x.begin(), x.end(), 0.0);
  double sumsq = std::transform_reduce(x.begin(), x.end(), 0.0,
                                       std::plus<>{},
                                       [](double v) { return v * v; });
  std::cout << "accumulate=" << acc
            << " reduce=" << red
            << " sum-of-squares=" << sumsq << '\n';

  // inner_product: generalized dot product.
  std::vector<double> y(N, 2.0);
  double dot = std::inner_product(x.begin(), x.end(), y.begin(), 0.0);
  std::cout << "inner_product(x, 2*ones) = " << dot << '\n';

  // partial_sum / inclusive_scan / exclusive_scan.
  std::vector<int> in = {1, 2, 3, 4, 5};
  std::vector<int> ps(in.size());
  std::partial_sum(in.begin(), in.end(), ps.begin());
  print_range("partial_sum:    ", ps);
  std::vector<int> is(in.size());
  std::inclusive_scan(in.begin(), in.end(), is.begin());
  print_range("inclusive_scan: ", is);
  std::vector<int> es(in.size());
  std::exclusive_scan(in.begin(), in.end(), es.begin(), 0);
  print_range("exclusive_scan: ", es);

  // adjacent_difference: out[i] = in[i] - in[i-1].
  std::vector<int> ad(in.size());
  std::adjacent_difference(in.begin(), in.end(), ad.begin());
  print_range("adjacent_diff:  ", ad);

  // gcd, lcm, midpoint.
  std::cout << "gcd(24,18)=" << std::gcd(24, 18)
            << " lcm(4,6)=" << std::lcm(4, 6)
            << " midpoint(2,9)=" << std::midpoint(2, 9) << '\n';
}

// =====================================================================
// Section 12. C++20 ranges algorithms (projections!)
// =====================================================================
struct Robot { std::string name; double battery; };

void ranges_algorithms() {
  std::cout << "\n--- 12. Ranges algorithms ---\n";

  std::vector<Robot> fleet = {
      {"alpha", 0.85}, {"beta", 0.12}, {"gamma", 0.55}, {"delta", 0.18},
  };

  // Sort by battery descending using a projection — no comparator needed.
  std::ranges::sort(fleet, std::greater<>{}, &Robot::battery);
  std::cout << "fleet by battery desc:\n";
  for (const auto &r : fleet)
    std::cout << "  " << r.name << " " << r.battery << '\n';

  // First robot below 20% battery using a projection.
  auto low = std::ranges::find_if(
      fleet, [](double b) { return b < 0.2; }, &Robot::battery);
  if (low != fleet.end())
    std::cout << "first below 20%: " << low->name << '\n';

  // back_inserter + ranges::transform.
  std::vector<int> in = {1, 2, 3, 4}, out;
  std::ranges::transform(in, std::back_inserter(out),
                         [](int x) { return x * 2; });
  print_range("ranges::transform*2: ", out);
}

// =====================================================================
// Section 13. C++20 views — lazy pipelines
// =====================================================================
struct Reading { double value; bool valid; };

void views_demo() {
  std::cout << "\n--- 13. Views ---\n";
  namespace rv = std::views;

  std::vector<Reading> readings = {
      {1.0, true}, {2.0, false}, {3.5, true}, {4.2, true},
      {5.0, false}, {6.1, true}, {7.0, true}};

  auto fresh = readings
             | rv::filter([](const Reading &r) { return r.valid; })
             | rv::transform(&Reading::value)
             | rv::take(3);

  std::cout << "first 3 valid readings: ";
  for (double v : fresh) std::cout << v << ' ';
  std::cout << '\n';

  // Materialize a view back into a vector (pre-C++23 way).
  std::vector<double> positives;
  auto positives_view = readings
                      | rv::transform(&Reading::value)
                      | rv::filter([](double v) { return v > 0; });
  std::ranges::copy(positives_view, std::back_inserter(positives));
  print_range("positives: ", positives);
}

// =====================================================================
// Section 14. Parallel and unsequenced execution
// =====================================================================
void parallel_demo() {
  std::cout << "\n--- 14. Parallel execution ---\n";

  std::vector<double> v(1000);
  std::iota(v.begin(), v.end(), 0.0);

  // Parallel policies on libstdc++ need TBB linked. Use seq for portability.
  std::sort(std::execution::seq, v.begin(), v.end());
  double s = std::reduce(std::execution::seq, v.begin(), v.end(), 0.0);
  std::cout << "reduce(seq) sum [0..999] = " << s << '\n';
}

// =====================================================================
// main: exercise each section in order.
// =====================================================================
int main() {
  non_modifying();
  modifying();
  exchange_demo();
  partitioning();
  sorting();
  binary_search_demo();
  set_operations();
  heap_demo();
  min_max_clamp();
  comparison();
  permutation_shuffle_sample();
  numeric_ops();
  ranges_algorithms();
  views_demo();
  parallel_demo();
  return 0;
}
