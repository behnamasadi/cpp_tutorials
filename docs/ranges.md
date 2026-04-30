# C++20 Ranges and Views

Ranges are the biggest STL change since C++98. They replace the `(begin, end)` iterator-pair API with composable, lazy pipelines that read top-to-bottom and allocate nothing along the way.

- [1. What ranges solve](#1-what-ranges-solve)
- [2. The pipe syntax](#2-the-pipe-syntax)
- [3. Common views](#3-common-views)
- [4. Lazy evaluation](#4-lazy-evaluation)
- [5. Materializing into a container](#5-materializing-into-a-container)
- [6. Range-aware algorithms](#6-range-aware-algorithms)
- [7. Gotcha: dangling views](#7-gotcha-dangling-views)

---

# 1. What ranges solve

Pre-C++20 algorithms always took two iterators:

```cpp
#include <algorithm>
#include <vector>

std::vector<int> v = {3, 1, 4, 1, 5, 9, 2, 6};
std::sort(v.begin(), v.end());                   // verbose, easy to mismatch
auto it = std::find(v.begin(), v.end(), 4);
```

This is fine for one call, but try to **chain** "filter even, then double, then take the first three." You end up with temporary vectors, hand-written loops, or `std::transform` + `std::copy_if` gymnastics.

Ranges fix two things:

1. **A `range` is a single object** carrying its own begin/end. Algorithms can take it whole: `std::ranges::sort(v)`.
2. **Views are lazy adaptors** you compose with `|`. No intermediate containers; no allocations.

```cpp
#include <ranges>
#include <vector>
#include <iostream>

int main() {
  std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8};

  auto pipeline = v | std::views::filter([](int x){ return x % 2 == 0; })
                    | std::views::transform([](int x){ return x * x; });

  for (int x : pipeline) std::cout << x << ' ';   // 4 16 36 64
}
```

# 2. The pipe syntax

The `|` is just function application read left-to-right. These two are equivalent:

```cpp
auto a = v | std::views::filter(pred) | std::views::transform(f);
auto b = std::views::transform(std::views::filter(v, pred), f);
```

Pipe form scales: each stage is a self-contained adaptor and you can drop one in or out without rewriting the call.

# 3. Common views

All in `<ranges>`, namespace `std::views` (which is an alias for `std::ranges::views`).

| View | Purpose | Since |
|------|---------|-------|
| `filter(pred)`     | keep elements where `pred(x)` is true | C++20 |
| `transform(f)`     | yield `f(x)` for each element          | C++20 |
| `take(n)`          | first `n` elements                     | C++20 |
| `drop(n)`          | skip the first `n` elements            | C++20 |
| `reverse`          | iterate back-to-front                  | C++20 |
| `iota(a [, b])`    | infinite or `[a, b)` integer sequence  | C++20 |
| `zip(a, b, ...)`   | tuple of elements, one from each range | **C++23** |
| `enumerate`        | `(index, value)` pairs                 | **C++23** |
| `join`             | flatten range-of-ranges                | C++20 |
| `split(delim)`     | split a range by a delimiter           | C++20 |
| `chunk(n)`         | non-overlapping windows of size `n`    | **C++23** |

Worked example using only C++20 views:

```cpp
#include <ranges>
#include <iostream>

int main() {
  // squares of the first 5 even numbers >= 10
  auto r = std::views::iota(10)                                   // 10, 11, 12, ...
         | std::views::filter([](int x){ return x % 2 == 0; })    // 10, 12, 14, ...
         | std::views::transform([](int x){ return x * x; })      // 100, 144, ...
         | std::views::take(5);

  for (int x : r) std::cout << x << ' ';   // 100 144 196 256 324
}
```

`iota(10)` is unbounded — it would loop forever on its own. The lazy `take(5)` is what makes the pipeline terminate.

C++23 `zip` and `enumerate`:

```cpp
#include <ranges>
#include <vector>
#include <string>
#include <iostream>

int main() {
  std::vector<std::string> names = {"alice", "bob", "carol"};
  std::vector<int>         ages  = {30, 25, 41};

  // C++23: pair them up
  for (auto [n, a] : std::views::zip(names, ages))
    std::cout << n << " is " << a << '\n';

  // C++23: index + value
  for (auto [i, n] : std::views::enumerate(names))
    std::cout << i << ": " << n << '\n';
}
```

Both `zip` and `enumerate` require **C++23** (`-std=c++23` in GCC 13+ / Clang 17+).

# 4. Lazy evaluation

Views **don't allocate** and don't store transformed elements. They're zero-overhead iterator adaptors: `filter` holds a pointer-to-source and a predicate; `transform` holds a pointer-to-source and a function. Every step happens *as you iterate*.

```cpp
auto r = v | std::views::transform([](int x){
              std::cout << "called with " << x << '\n';
              return x * 2;
            });

// Nothing is printed yet. The lambda runs only when we iterate:
for (int x : r) { (void)x; }
```

Practical consequences:

- **No copy of `v`.** A view stores a *reference* to its source.
- **Composing views is free.** `v | filter | transform | take | drop` is still O(1) memory.
- **Each stage runs once per consumed element.** `filter` followed by `take(3)` stops after producing three matches — it doesn't scan the rest of the input.

# 5. Materializing into a container

A view is not a container. You can't `.size()` most of them, you can't pass them to a function that wants `std::vector<int>`, and you can't index them with `[]` unless they're random-access.

When you actually need a container, **materialize** the view:

```cpp
// C++23: std::ranges::to
#include <ranges>
#include <vector>

auto vec = v | std::views::filter(pred)
             | std::views::transform(f)
             | std::ranges::to<std::vector>();
```

The C++20 workaround (before `to<>` existed):

```cpp
#include <ranges>
#include <vector>

auto view = v | std::views::filter(pred) | std::views::transform(f);

std::vector<int> vec;
// reserve if you know the size; otherwise just push_back through the view
for (int x : view) vec.push_back(x);

// Or, slightly tighter:
std::vector<int> vec2(view.begin(), view.end());   // works if the view is at least input
```

Use `ranges::to` whenever you can — it picks the best constructor, can deduce the container's value type, and supports `to<std::map>`, `to<std::set>`, etc.

# 6. Range-aware algorithms

Every classic algorithm in `<algorithm>` has a sibling in `std::ranges::` that takes a single range argument and (often) supports **projections** — a per-element function applied before the comparator.

```cpp
#include <algorithm>
#include <ranges>
#include <vector>
#include <string>

struct Person { std::string name; int age; };

int main() {
  std::vector<Person> people = {{"alice",30}, {"bob",25}, {"carol",41}};

  // Old way:
  std::sort(people.begin(), people.end(),
            [](auto& a, auto& b){ return a.age < b.age; });

  // New way: pass the range whole, and project on `.age`
  std::ranges::sort(people, {}, &Person::age);

  // {} means "default comparator" (std::less). The third arg is the projection.
}
```

The projection lets you drop most one-off comparator lambdas. Other useful range algorithms work the same way: `ranges::find`, `ranges::count_if`, `ranges::min_element`, `ranges::for_each`, `ranges::all_of`, ...

See [algorithms.md](algorithms.md) for the full classic-algorithm catalog and [iterator_loop.md](iterator_loop.md) for the iterator categories that ranges build on.

# 7. Gotcha: dangling views

A view borrows from its source. If the source dies first, the view dangles — same hazard as a dangling pointer.

```cpp
auto bad() {
  std::vector<int> v = {1,2,3,4,5};
  return v | std::views::filter([](int x){ return x % 2; });   // BUG
  // `v` is destroyed at end of scope; the returned view points into freed memory.
}
```

The library protects you in some cases. Range algorithms applied to a temporary container return `std::ranges::dangling` instead of an iterator, so the bug is caught at compile time:

```cpp
auto it = std::ranges::find(std::vector<int>{1,2,3}, 2);
// it is std::ranges::dangling, not an iterator. Dereferencing won't compile.
```

But views built from temporaries through `|` are still your responsibility. Two fixes:

1. **Don't return views from functions** — return the materialized container.
2. **Use `std::views::owning_view`** (created automatically when you pipe a temporary) — it takes ownership, but only certain views support it. Easier to just materialize.

Rule of thumb: a view's lifetime must be a strict subset of every range it transitively references.

---

# References

- [cppreference: Ranges library](https://en.cppreference.com/w/cpp/ranges)
- [cppreference: std::ranges algorithms](https://en.cppreference.com/w/cpp/algorithm/ranges)
- [Eric Niebler: A Plea for Lazy Functional Programming in C++](https://ericniebler.com/2014/04/27/range-comprehensions/) — the design that became C++20 ranges
- Related docs: [iterator_loop.md](iterator_loop.md), [algorithms.md](algorithms.md)
