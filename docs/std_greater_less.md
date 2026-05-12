# Comparator function objects: `std::less`, `std::greater`, `std::not_fn`

The standard library ships a handful of small function-object templates whose only job is to produce a callable from an operator. They show up wherever an algorithm or container takes a `Compare` or `Predicate`: `std::sort`, `std::priority_queue`, `std::set`, `std::map`, `std::count_if`, `std::partition`, etc.

- [`std::less` / `std::greater`](#stdless--stdgreater)
- [Transparent comparators (C++14)](#transparent-comparators-c14)
- [Comparators with user-defined types](#comparators-with-user-defined-types)
- [Negating a predicate: `std::not_fn` (and the deprecated `std::not1` / `std::unary_negate`)](#negating-a-predicate-stdnot_fn-and-the-deprecated-stdnot1--stdunary_negate)

---

## `std::less` / `std::greater`

`std::less<T>` calls `operator<` on its two arguments, `std::greater<T>` calls `operator>`. Both live in `<functional>`.

```cpp
#include <algorithm>
#include <functional>
#include <vector>

std::vector<int> v = {3, 7, 4, 9, 2, 5, 8, 1, 6};

std::sort(v.begin(), v.end());                          // ascending (uses operator<)
std::sort(v.begin(), v.end(), std::greater<int>{});     // descending
```

The same templates select container behavior:

```cpp
std::set<int, std::greater<int>>            scores;     // largest first
std::priority_queue<int,
                    std::vector<int>,
                    std::greater<int>>      min_heap;   // min-heap (default is max-heap)
```

`std::priority_queue<int>` defaults to a max-heap because the underlying comparator is `std::less` — the element that is **not less than** any other floats to the top. Flipping to `std::greater` turns it into a min-heap, which is exactly what you want for Dijkstra, A\*, or "process the smallest deadline first" scheduling.

## Transparent comparators (C++14)

Writing `std::less<>` (with empty angle brackets, equivalent to `std::less<void>`) gives you a **transparent comparator** that deduces the operand types at the call site. This enables heterogeneous lookup on `std::set` / `std::map`:

```cpp
std::set<std::string, std::less<>> dict;
dict.insert("hello");

// Look up with a string_view — no temporary std::string is constructed.
auto it = dict.find(std::string_view{"hello"});
```

Without the transparent comparator, `find` would force the lookup key into a `std::string`, allocating.

## Comparators with user-defined types

`std::less` / `std::greater` work on any type that defines `operator<` / `operator>`:

```cpp
struct Person {
    std::string name;
    int         age;
};

bool operator<(const Person& a, const Person& b) { return a.age < b.age; }
bool operator>(const Person& a, const Person& b) { return a.age > b.age; }

std::vector<Person> people = {{"Alice", 30}, {"Bob", 25}, {"Charlie", 35}};

std::sort(people.begin(), people.end(), std::less<Person>{});      // youngest first
std::sort(people.begin(), people.end(), std::greater<Person>{});   // oldest first
```

In C++20 you can usually just default the spaceship operator and skip writing `operator<` / `operator>` by hand:

```cpp
struct Person {
    std::string name;
    int         age;
    auto operator<=>(const Person&) const = default;
};
```

See [three_way_comparison.md](three_way_comparison.md).

## Negating a predicate: `std::not_fn` (and the deprecated `std::not1` / `std::unary_negate`)

Sometimes you have a predicate and want its logical negation. C++17 introduced `std::not_fn`, which wraps any callable and returns one that negates the result:

```cpp
#include <algorithm>
#include <functional>
#include <vector>

std::vector<int> v = {2, 5, 8, 1, 7, 4};
auto is_small = [](int x) { return x < 5; };

int n_big = std::count_if(v.begin(), v.end(), std::not_fn(is_small));
// counts elements where !is_small(x) — i.e. x >= 5
```

`std::not_fn` works with any callable (lambdas, function objects, member functions via `std::mem_fn`) and any arity. It replaces the old C++98 helpers:

- `std::not1(pred)` — unary negation, returned a `std::unary_negate`. **Deprecated in C++17, removed in C++20.**
- `std::not2(pred)` — binary negation. Also removed in C++20.

You may still see `std::not1` in older codebases; the migration is mechanical (`std::not1(p)` → `std::not_fn(p)`), and the new form is strictly more flexible because it doesn't require the predicate to inherit from the now-removed `std::unary_function`.

A modern lambda is often clearer than either form:

```cpp
std::count_if(v.begin(), v.end(), [](int x) { return x >= 5; });
```

Use `std::not_fn` when the predicate already exists and you don't want to duplicate its logic.
