# Algorithms Library

The `<algorithm>` header (plus `<numeric>`, `<ranges>`, `<execution>`) covers the
generic operations the STL provides over iterator ranges. This page is a quick
reference grouped by purpose, with robotics-flavored examples and a few sharp
edges to watch out for.

Conventions used below:

- `n` is the size of the input range.
- "Sorted-range" algorithms assume the input is already sorted by the same
  comparator you pass in — getting that wrong is a common bug.
- Code shown is C++20 unless a `// C++23` marker is on the line.

---

## 1. Non-modifying sequence operations

| Function | Notes |
| --- | --- |
| `for_each`, `for_each_n` | Side-effect loop. Result is **not** propagated unless your callable captures or takes by reference. |
| `count`, `count_if` | Returns `iterator_traits::difference_type` (signed). |
| `find`, `find_if`, `find_if_not` | Linear scan; use `lower_bound` on sorted data. |
| `find_first_of` | Finds the first element from set B inside A. |
| `adjacent_find` | First position `i` where `pred(a[i], a[i+1])` holds. |
| `search`, `search_n` | Subsequence search; pass a `std::boyer_moore_searcher` for fast substring scans. |
| `mismatch` | First index where two ranges differ — pair of iterators. |
| `equal`, `is_permutation` | Whole-range equality / unordered equality. |
| `all_of`, `any_of`, `none_of` | Predicate quantifiers. |
| `contains`, `contains_subrange` | C++23. Returns `bool` — clearer than `find != end()`. |
| `starts_with`, `ends_with` | C++23, in `std::ranges::`. Works on any range, not just `string`. |

```cpp
// Count joints whose tracking error exceeds 1°.
std::vector<double> error_deg = read_joint_errors();
auto bad = std::count_if(error_deg.begin(), error_deg.end(),
                         [](double e){ return std::abs(e) > 1.0; });
```

`std::for_each` returns nothing useful unless the lambda captures by reference —
a returned value from the lambda is silently discarded:

```cpp
std::vector<int> v = {0,1,2,3,4,5};
std::for_each(v.begin(), v.end(), [](int x){ return x * 3; }); // no-op
std::for_each(v.begin(), v.end(), [](int& x){ x++; });          // mutates v
```

---

## 2. Modifying sequence operations

| Function | Notes |
| --- | --- |
| `copy`, `copy_if`, `copy_n`, `copy_backward` | Pre-size the destination, or pass `std::back_inserter`. |
| `move`, `move_backward` | Same as `copy*` but invokes move ctor/assign. Source range left in a *valid but unspecified* state. |
| `fill`, `fill_n`, `generate`, `generate_n` | Bulk assignment / generator-based fill. |
| `transform` | `Output = f(input)` or `Output = f(input1, input2)`. |
| `replace`, `replace_if`, `replace_copy*` | Substitute matching values. |
| `remove`, `remove_if`, `unique` | **Do not shrink** the container — pair with `erase` (or use `std::erase`/`std::erase_if`, C++20). |
| `reverse`, `rotate`, `shuffle` | In-place rearrangements. |
| `shift_left`, `shift_right` | C++20. Discards values that fall off the edge — handy for fixed-size sample windows. |
| `sample` | C++17. Random sample of size `k` without replacement. |
| `swap`, `iter_swap`, `swap_ranges` | Element-wise swap. |
| `exchange` | Returns the old value while assigning the new one — useful inside move constructors. |

### `remove` / `unique` — the erase-remove idiom

`std::remove` does not actually remove anything; it overwrites the kept
elements at the front and returns the new logical end. You almost always
follow it with `erase`, or just use `std::erase_if` (C++20):

```cpp
std::vector<int> v = {0,1,2,3,4,5,6,7,8,9};

// classic erase-remove
v.erase(std::remove_if(v.begin(), v.end(),
                       [](int x){ return x % 2 == 0; }),
        v.end());

// C++20 one-liner — same effect
std::erase_if(v, [](int x){ return x % 2 == 0; });
```

`std::unique` only collapses *consecutive* duplicates — sort first if you want
true deduplication.

### Sliding a sample buffer with `shift_left`

```cpp
std::array<double, 8> imu_window{};
// ... new sample arrives ...
std::shift_left(imu_window.begin(), imu_window.end(), 1);
imu_window.back() = latest_sample;
```

### `std::exchange` in move ops

```cpp
struct Handle {
    int fd;
    Handle(Handle&& other) noexcept : fd(std::exchange(other.fd, -1)) {}
};
```

---

## 3. Partitioning

| Function | Notes |
| --- | --- |
| `partition` | Rearranges so all `pred==true` come first. Not stable. |
| `stable_partition` | Stable but allocates. |
| `partition_copy` | Splits into two output ranges. |
| `partition_point` | Binary search for the boundary on an already-partitioned range. |
| `is_partitioned` | Predicate check. |

```cpp
// Separate obstacle points (close range) from background — order inside each
// group does not matter, so use the non-stable form.
auto bound = std::partition(points.begin(), points.end(),
                            [](const auto& p){ return p.range < 5.0; });
// [begin, bound) are close-range, [bound, end) are far.
```

---

## 4. Sorting

| Function | Notes |
| --- | --- |
| `sort` | O(n log n) introsort. Unstable. Requires random-access iterators. |
| `stable_sort` | Stable, may allocate. |
| `partial_sort` | First `k` elements sorted; rest is unspecified. O(n log k). |
| `partial_sort_copy` | Same but writes to a separate output range. |
| `nth_element` | Places one element in its sorted position; partitions around it. O(n) average. |
| `is_sorted`, `is_sorted_until` | Predicate checks. |

> **Comparator must be a strict weak ordering.** That means `cmp(a, a)` is
> always `false`. Passing `std::greater_equal<>{}` or `std::less_equal<>{}`
> to `sort`, `nth_element`, `make_heap`, etc. is undefined behavior — even
> if it looks like it works. Use `std::less<>{}` or `std::greater<>{}`.

### Top-K with `partial_sort`

```cpp
// Find the 4 closest obstacles, sorted by range.
constexpr int K = 4;
std::vector<Obstacle> obs = scan();
std::partial_sort(obs.begin(), obs.begin() + K, obs.end(),
                  [](const auto& a, const auto& b){ return a.range < b.range; });
// obs[0..K) holds the K closest, in order.
```

### Median (or any quantile) with `nth_element`

```cpp
std::vector<int> v = {3,7,4,9,2,5,8,1,6};
auto mid = v.begin() + v.size()/2;
std::nth_element(v.begin(), mid, v.end());
// *mid is the median; elements left/right are < / > but unsorted.
```

`nth_element` is the right tool for **median filtering** of sensor data — it's
O(n) average versus O(n log n) for a full sort.

---

## 5. Binary search (sorted ranges only)

| Function | Returns |
| --- | --- |
| `lower_bound` | First position where `value` could be inserted (`>= value`). |
| `upper_bound` | First position strictly greater than `value`. |
| `equal_range` | Pair `{lower_bound, upper_bound}`. |
| `binary_search` | `bool` only — usually prefer `lower_bound` and compare. |

```cpp
std::vector<double> ts = sorted_timestamps();
// Index of the first sample at or after t_query.
auto it = std::lower_bound(ts.begin(), ts.end(), t_query);
auto idx = std::distance(ts.begin(), it);
```

**Pitfall:** if the range is unsorted, results are undefined. These do log(n)
comparisons but O(n) iterator steps on non-random-access iterators — so on
`std::list` they degrade to linear and you should use the container's own
`find`/`lower_bound` member, or just convert.

---

## 6. Set operations (sorted ranges only)

`std::merge`, `std::set_union`, `std::set_intersection`, `std::set_difference`,
`std::set_symmetric_difference`, `std::includes`.

```cpp
// Merge two time-sorted sensor streams into one timeline.
std::vector<Sample> imu, encoder, fused;
fused.reserve(imu.size() + encoder.size());
std::merge(imu.begin(), imu.end(),
           encoder.begin(), encoder.end(),
           std::back_inserter(fused),
           [](const auto& a, const auto& b){ return a.t < b.t; });
```

---

## 7. Heap operations

`make_heap`, `push_heap`, `pop_heap`, `sort_heap`, `is_heap`, `is_heap_until`.

By default these form a **max-heap** (largest at `front`). Pass
`std::greater<>{}` for a min-heap. (Again: `greater_equal` is UB.)

```cpp
std::vector<int> v = {6, 10, 7, 17, 10, 15};
std::make_heap(v.begin(), v.end());                 // max-heap
v.push_back(100); std::push_heap(v.begin(), v.end());
std::pop_heap(v.begin(), v.end()); v.pop_back();    // remove the max
std::sort_heap(v.begin(), v.end());                 // ascending; no longer a heap
```

### `std::priority_queue` for A\* / Dijkstra

```cpp
struct Cell { int idx; float cost; };
struct ByCost {
    bool operator()(const Cell& a, const Cell& b) const {
        return a.cost > b.cost;          // min-heap on cost
    }
};

std::priority_queue<Cell, std::vector<Cell>, ByCost> open;
open.push({start_idx, 0.f});
```

Lambda comparator (the type is unnamed, so `decltype`):

```cpp
auto by_cost = [](const Cell& a, const Cell& b){ return a.cost > b.cost; };
std::priority_queue<Cell, std::vector<Cell>, decltype(by_cost)> open(by_cost);
```

Or, easiest of all, give `Cell` an `operator<` and use the default:

```cpp
struct Cell {
    int idx; float cost;
    bool operator<(const Cell& o) const { return cost < o.cost; } // max-heap on cost
};
std::priority_queue<Cell> q;
```

---

## 8. Min / Max / Clamp

| Function | Notes |
| --- | --- |
| `min`, `max`, `minmax` | Scalar or initializer-list overloads. `minmax` returns a `pair`. |
| `min_element`, `max_element`, `minmax_element` | Iterator versions. |
| `clamp` (C++17) | `clamp(v, lo, hi)` — saturates `v` into `[lo, hi]`. |

```cpp
// Saturate a motor command before sending it on the CAN bus.
double cmd = std::clamp(controller_output, -MAX_TORQUE, MAX_TORQUE);
```

**Lifetime trap:** `std::min`/`std::max` return references. Don't bind to
`auto&`/`const auto&` for an `initializer_list` overload — the temporaries die
at the semicolon.

```cpp
const auto& m = std::min({a, b, c}); // dangling — DON'T
auto        m = std::min({a, b, c}); // fine
```

---

## 9. Comparison

- `std::equal` — element-wise equality of two ranges.
- `std::lexicographical_compare` — dictionary order; also has a `_three_way`
  variant (C++20) that returns a `std::strong_ordering`.

---

## 10. Permutation, shuffle, and sampling

- `std::next_permutation`, `std::prev_permutation` — iterate permutations
  lexicographically.
- `std::shuffle(first, last, urbg)` — uniform random shuffle. Pass a real
  generator like `std::mt19937{std::random_device{}()}`.
  `std::random_shuffle` was deprecated in C++14 and **removed in C++17**.
- `std::sample(first, last, out, k, urbg)` — sample `k` elements without
  replacement (C++17).

```cpp
// Random downsample of a dense lidar cloud, no duplicates.
std::vector<Point> sparse;
sparse.reserve(2000);
std::sample(cloud.begin(), cloud.end(),
            std::back_inserter(sparse), 2000,
            std::mt19937{std::random_device{}()});
```

---

## 11. Numeric operations (`<numeric>`)

| Function | Notes |
| --- | --- |
| `iota` | Fill with successive values starting from `v0`. |
| `accumulate` | Left fold, **sequential**, requires associativity for sane reordering. |
| `reduce` (C++17) | Like `accumulate` but allowed to reorder — needs commutative+associative op. Accepts execution policies. |
| `transform_reduce` | Map-then-reduce in one pass. |
| `inner_product` | Generalized dot product. |
| `partial_sum`, `inclusive_scan`, `exclusive_scan` | Prefix sums; the scans accept execution policies. |
| `adjacent_difference` | `out[i] = in[i] - in[i-1]`. |
| `gcd`, `lcm` (C++17) | |
| `midpoint` (C++20) | Overflow-safe midpoint. |
| `ranges::fold_left`, `ranges::fold_right` (C++23) | Proper left/right fold; can return a different type than the elements. |

```cpp
std::vector<double> x(N);
std::iota(x.begin(), x.end(), 0.0);                // 0, 1, 2, ..., N-1

double sum   = std::reduce(std::execution::par, x.begin(), x.end(), 0.0);
double sumsq = std::transform_reduce(x.begin(), x.end(), 0.0,
                                     std::plus<>{},
                                     [](double v){ return v*v; });
```

C++23 `fold_left` is preferred over `accumulate` when the accumulator type
differs from the element type — it handles that without surprising decay:

```cpp
auto labels = scan_labels();          // std::vector<std::string_view>
auto joined = std::ranges::fold_left( // C++23
    labels, std::string{},
    [](std::string acc, std::string_view s){ acc += s; return acc; });
```

---

## 12. C++20 ranges algorithms

Every classic algorithm has a `std::ranges::` counterpart that:

1. Takes a **range** directly (no `begin()/end()` boilerplate).
2. Supports **projections** — a function applied to each element before the
   predicate or comparator sees it.
3. Returns richer result types (`ranges::*_result` aggregates) and Sentinel-aware
   iterators.

```cpp
struct Robot { std::string name; double battery; };
std::vector<Robot> fleet = ...;

// Sort by battery descending — no comparator needed, just a projection.
std::ranges::sort(fleet, std::greater<>{}, &Robot::battery);

// First robot below 20% — same pattern.
auto low = std::ranges::find_if(fleet,
                                [](double b){ return b < 0.2; },
                                &Robot::battery);
```

Common counterparts you'll reach for:
`ranges::sort`, `ranges::stable_sort`, `ranges::find`, `ranges::find_if`,
`ranges::count_if`, `ranges::transform`, `ranges::copy`, `ranges::copy_if`,
`ranges::min_element`, `ranges::max_element`, `ranges::for_each`,
`ranges::any_of`, `ranges::all_of`, `ranges::none_of`,
`ranges::unique`, `ranges::partition`, `ranges::merge`,
`ranges::lower_bound`, `ranges::binary_search`.

C++23 additions: `ranges::contains`, `ranges::contains_subrange`,
`ranges::starts_with`, `ranges::ends_with`, `ranges::fold_left`,
`ranges::fold_right`, `ranges::find_last`, `ranges::find_last_if`.

### `std::back_inserter` with ranges

```cpp
std::vector<int> in = {1,2,3,4}, out;
std::ranges::transform(in, std::back_inserter(out),
                       [](int x){ return x * 2; });
```

---

## 13. C++20/23 views — lazy pipelines

Views are non-owning, lazily evaluated adaptors that you compose with `|`.
They never materialize an intermediate container. Perfect for streaming
sensor data.

```cpp
namespace rv = std::views;

auto fresh = readings
           | rv::filter([](const Reading& r){ return r.valid; })
           | rv::transform(&Reading::value)
           | rv::take(100);

for (double v : fresh) { /* ... */ }
```

Useful views:

- C++20: `filter`, `transform`, `take`, `take_while`, `drop`, `drop_while`,
  `reverse`, `keys`, `values`, `elements`, `split`, `join`, `common`,
  `iota`, `single`, `empty`.
- C++23: `zip`, `zip_transform`, `enumerate`, `adjacent<N>`, `slide`,
  `chunk`, `chunk_by`, `stride`, `join_with`, `as_const`, `as_rvalue`,
  `cartesian_product`, `repeat`.

```cpp
// C++23: walk pairs of consecutive joint positions to compute velocities.
auto vel = std::views::zip(positions, positions | std::views::drop(1))
         | std::views::transform([dt](auto p){
               auto [a, b] = p; return (b - a) / dt;
           });

// C++23: index your samples without a manual counter.
for (auto [i, sample] : std::views::enumerate(samples)) { /* ... */ }

// C++23: process the scan in 16-point windows.
for (auto window : scan | std::views::chunk(16)) { /* ... */ }
```

### `std::ranges::to` (C++23) — materialize a view into a container

```cpp
auto positives = readings
               | std::views::transform(&Reading::value)
               | std::views::filter([](double v){ return v > 0; })
               | std::ranges::to<std::vector>();   // C++23
```

Before C++23, use `std::ranges::copy(view, std::back_inserter(vec))` instead.

---

## 14. Parallel and unsequenced execution

Most numeric and many `<algorithm>` functions accept an execution policy as the
first argument:

```cpp
#include <execution>
std::sort(std::execution::par_unseq, v.begin(), v.end());
double s = std::reduce(std::execution::par, v.begin(), v.end(), 0.0);
```

Policies:

- `seq` — sequential, well-defined order.
- `par` — may run on multiple threads; element-access must be thread-safe.
- `par_unseq` — may also vectorize; your callable **must not** take locks,
  call `new`/`delete`, or anything that can't be interleaved.
- `unseq` (C++20) — vectorization without threading.

See [execution_policies.md](execution_policies.md) for the longer write-up. On
Linux with libstdc++, parallel policies require linking against TBB
(`-ltbb`).

---

## 15. Common pitfalls

- **Strict weak ordering.** `sort`, `nth_element`, `make_heap`, `priority_queue`
  comparators must return `false` for equal elements. `greater_equal` /
  `less_equal` are UB even if your test cases happen to pass.
- **Sorted-range preconditions.** `binary_search`, `lower_bound`, `merge`,
  `set_*`, `includes` all assume sorted input under the *same* comparator you
  pass.
- **`remove` does not shrink.** Pair with `erase`, or just use `std::erase` /
  `std::erase_if` (C++20).
- **`for_each` discards return values** from the callable.
- **`min`/`max` return references.** Don't bind to `const auto&` for the
  `initializer_list` overload — the temporaries die at the semicolon.
- **Iterator categories.** `sort`, `nth_element`, `shuffle` require random-access
  iterators — they won't compile on `std::list` (use `list::sort`).
- **Container size changes.** Algorithms operate on iterator ranges; they
  never insert. Use `back_inserter`/`inserter` for outputs that need to grow.
- **Dangling views.** Views are non-owning. Never return a view that adapts a
  local container.

---

[code](../src/algorithms_library.cpp)
