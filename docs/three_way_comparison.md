# Three-Way Comparison: the Spaceship Operator `<=>`

C++20 introduced the **three-way comparison operator** `<=>` (the "spaceship operator") and the `<compare>` header. Together they let you write **one** function to get all six relational operators (`<`, `>`, `<=`, `>=`, `==`, `!=`) for your type — instead of six.

See also [operator_overloading.md](operator_overloading.md) for the general operator-overloading rules this builds on.

## The problem before C++20: six functions per type

If you wanted your type to be fully comparable, you had to write each operator by hand:

```cpp
struct Version {
  int major, minor, patch;

  bool operator==(const Version& o) const {
    return major == o.major && minor == o.minor && patch == o.patch;
  }
  bool operator!=(const Version& o) const { return !(*this == o); }

  bool operator<(const Version& o) const {
    if (major != o.major) return major < o.major;
    if (minor != o.minor) return minor < o.minor;
    return patch < o.patch;
  }
  bool operator>(const Version& o) const  { return o < *this; }
  bool operator<=(const Version& o) const { return !(o < *this); }
  bool operator>=(const Version& o) const { return !(*this < o); }
};
```

Six functions, each a chance to introduce inconsistency. Forget `!=` and your type can't be used with some containers. Get the lexicographic chain wrong in `<` and you have a silent bug.

## The fix: `auto operator<=>(const T&) const = default;`

```cpp
#include <compare>

struct Version {
  int major, minor, patch;

  auto operator<=>(const Version&) const = default;
};
```

That's it. The compiler **synthesizes** all six relational operators from the defaulted spaceship. `<=>` does a memberwise lexicographic comparison in declaration order — exactly what you'd write by hand for a struct of plain values.

```cpp
int main() {
  Version a{1, 2, 3}, b{1, 2, 4};
  std::cout << (a < b)  << '\n';   // 1
  std::cout << (a == b) << '\n';   // 0
  std::cout << (a >= b) << '\n';   // 0
}
```

## What does `<=>` return? Comparison categories

`<=>` does not return `bool` or `int`. It returns one of three "ordering" types from `<compare>`:

| Category | When to use | Values |
|---|---|---|
| `std::strong_ordering`  | Equal values are *interchangeable* (substitutable in any context) | `less`, `equal`, `greater` |
| `std::weak_ordering`    | Equal values compare equal but are *distinguishable* (e.g., differ in case) | `less`, `equivalent`, `greater` |
| `std::partial_ordering` | Some values are not comparable at all (e.g., NaN) | `less`, `equivalent`, `greater`, `unordered` |

These categories form a hierarchy: `strong` is convertible to `weak` is convertible to `partial`. The compiler picks the strongest category every member supports.

### `strong_ordering` — integers, plain structs

```cpp
auto r = 3 <=> 5;            // std::strong_ordering::less
bool b = (r < 0);            // true
```

### `partial_ordering` — anything with NaN (i.e. floats)

```cpp
double x = std::nan("");
auto r = 1.0 <=> x;          // std::partial_ordering::unordered
bool lt = (r < 0);           // false
bool gt = (r > 0);           // false
bool eq = (r == 0);          // false — none of the relations hold
```

A defaulted `<=>` on a struct with a `double` member returns `partial_ordering`:

```cpp
struct Point {
  double x, y;
  auto operator<=>(const Point&) const = default;   // std::partial_ordering
};
```

### `weak_ordering` — equivalent but not identical

Case-insensitive strings: `"FOO"` and `"foo"` should compare equal for ordering purposes, but they're clearly different strings. That's `weak_ordering`.

```cpp
struct CIString {
  std::string s;

  std::weak_ordering operator<=>(const CIString& o) const {
    auto lower = [](char c) { return std::tolower((unsigned char)c); };
    return std::lexicographical_compare_three_way(
        s.begin(), s.end(), o.s.begin(), o.s.end(),
        [&](char a, char b) { return lower(a) <=> lower(b); });
  }
  bool operator==(const CIString& o) const {
    return (*this <=> o) == 0;
  }
};
```

## Custom (non-defaulted) `<=>`

When the default memberwise comparison isn't what you want, write your own. Pick the return category to match your semantics.

```cpp
struct Employee {
  std::string name;
  int         id;
  double      salary;

  // Order by id only, ignoring name and salary.
  std::strong_ordering operator<=>(const Employee& o) const {
    return id <=> o.id;
  }
  bool operator==(const Employee& o) const { return id == o.id; }
};

int main() {
  Employee a{"Ana", 7, 100.0}, b{"Ben", 7, 200.0};
  std::cout << (a == b) << '\n';   // 1 — same id, regardless of name/salary
  std::cout << (a < b)  << '\n';   // 0
}
```

## The asymmetry: `==` is not synthesized from `<=>`

This trips everyone up: `a < b`, `a > b`, `a <= b`, `a >= b` are all rewritten to use `<=>`, but `a == b` and `a != b` are **not**.

Why? Because `==` is often *much cheaper* than `<=>`. Comparing two strings for equality can short-circuit on length; computing the full ordering cannot. Forcing `==` to go through `<=>` would silently make equality slower for large classes of types. So the rule is:

- A **defaulted** `<=>` *also* implicitly defaults `==` for you.
- A **user-defined** (non-defaulted) `<=>` does **not** give you `==` — you must define `==` separately (or default it).

```cpp
struct A {
  int x;
  auto operator<=>(const A&) const = default;
  // == is implicitly defaulted too — a == b works.
};

struct B {
  std::string s;
  std::strong_ordering operator<=>(const B& o) const {
    return s.size() <=> o.s.size();           // order by length
  }
  // No ==!  `b1 == b2` will NOT compile.
  // You'd add: bool operator==(const B&) const = default;
  //   ...or a custom one matching your ordering semantics.
};
```

The mental model: `<=>` synthesizes the four *ordering* operators (`<`, `>`, `<=`, `>=`); `==` synthesizes its own pair (`==`, `!=`). They're independent except in the defaulted case.

## Using a comparison result: `std::is_lt`, `std::is_eq`, ...

The result of `<=>` is an ordering object, not a number. You can compare it against literal `0` (`r < 0`, `r == 0`, etc.), or use the named helpers from `<compare>`:

| Helper | Equivalent to |
|---|---|
| `std::is_lt(r)`  | `r < 0`  |
| `std::is_lteq(r)`| `r <= 0` |
| `std::is_gt(r)`  | `r > 0`  |
| `std::is_gteq(r)`| `r >= 0` |
| `std::is_eq(r)`  | `r == 0` |
| `std::is_neq(r)` | `r != 0` |

```cpp
#include <compare>

void demo() {
  auto r = 3 <=> 5;
  if (std::is_lt(r))  std::cout << "less\n";
  if (std::is_eq(r))  std::cout << "equal\n";
  if (std::is_gt(r))  std::cout << "greater\n";
}
```

The named helpers are useful in generic code where the ordering category isn't fixed, and in `partial_ordering` cases where `is_eq(r)` reads more clearly than `r == 0` (which is also false for `unordered`).

## Summary

- One defaulted `<=>` replaces six hand-written operators.
- Pick the return category by semantics: `strong` (interchangeable), `weak` (equivalent but distinct), `partial` (some pairs unordered, e.g. NaN).
- Defaulted `<=>` implicitly defaults `==` too. **User-defined** `<=>` does not — define `==` yourself.
- Compare results against `0` or use `std::is_lt` / `std::is_eq` / etc.

# References

- [cppreference: three-way comparison](https://en.cppreference.com/w/cpp/language/operator_comparison#Three-way_comparison)
- [cppreference: `<compare>`](https://en.cppreference.com/w/cpp/header/compare)
- [operator_overloading.md](operator_overloading.md)
