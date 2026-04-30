# `std::format`, `std::print`, and `std::println`

## Why `std::format` exists

Before C++20, you had two ways to produce formatted text and both had real flaws:

- **`printf`-family** is concise but **not type-safe**. The compiler can't verify that `%d` matches an `int`. A mismatch is undefined behavior:
  ```c
  printf("%d\n", "hello");   // UB — silently broken or crashes
  ```
- **`iostream`** is type-safe but **verbose** and slow to write. Formatting a single number with width and precision needs a forest of manipulators:
  ```cpp
  std::cout << std::setw(10) << std::setfill('0') << std::fixed
            << std::setprecision(3) << 3.14 << '\n';
  ```

C++20 introduced `<format>` to give you the best of both worlds: Python-style `{}` placeholders, **type safety checked at compile time**, and one-line readability.

```cpp
#include <format>
std::string s = std::format("{:0>10.3f}", 3.14);   // "000003.140"
```

## Basic usage

`std::format` returns a `std::string`. Each `{}` is replaced by the next argument, converted via the appropriate formatter.

```cpp
#include <format>
#include <iostream>

int main() {
  int a = 7, b = 35;
  std::string s = std::format("{} + {} = {}", a, b, a + b);
  std::cout << s << '\n';   // 7 + 35 = 42
}
```

If the number of `{}` placeholders doesn't match the argument count, you get a **compile-time error** (the format string is parsed at compile time when it's a constant expression). That's the type-safety win over `printf`.

## Format specifiers

The general syntax inside the braces is `{[index]:[fill][align][sign][#][0][width][.precision][type]}`. You rarely use them all at once.

### Width, alignment, fill

```cpp
std::format("{:>10}", "hi");    // "        hi"   right-align, width 10
std::format("{:<10}", "hi");    // "hi        "   left-align
std::format("{:^10}", "hi");    // "    hi    "   center
std::format("{:*^10}", "hi");   // "****hi****"   fill with '*'
```

### Precision (for floats and strings)

```cpp
std::format("{:.3f}",  3.14159);   // "3.142"        3 fractional digits
std::format("{:.3e}",  12345.6);   // "1.235e+04"    scientific
std::format("{:.5}",   "abcdefg"); // "abcde"        truncate string to 5 chars
```

### Numeric base and padding

```cpp
std::format("{:#x}",  255);   // "0xff"     hex with '0x' prefix
std::format("{:#o}",   8);    // "010"      octal with leading 0
std::format("{:08b}", 13);    // "00001101" binary, zero-padded width 8
std::format("{:+d}",  42);    // "+42"      always show sign
std::format("{:,}",   1000000); // "1,000,000" (C++23: locale-independent grouping)
```

### Combining them

```cpp
double pi = 3.14159265;
std::format("|{:>15.4f}|", pi);   // "|         3.1416|"
std::format("|{:0>15.4f}|", pi);  // "|0000000003.1416|"
```

## Positional arguments

Inside the `{}` you can put a zero-based index to pick which argument goes there. Useful when one argument appears multiple times, or for translations where word order changes.

```cpp
std::format("{0} {1} {0}", "hi", "world");   // "hi world hi"
std::format("{1} loves {0}", "Alice", "Bob"); // "Bob loves Alice"
```

You may not mix positional and automatic indexing in the same format string.

## Chrono support

`<format>` knows about `std::chrono` types. The `%` codes match `strftime`.

```cpp
#include <chrono>
#include <format>

auto now = std::chrono::system_clock::now();
std::format("{:%Y-%m-%d %H:%M:%S}", now);   // "2026-04-30 09:14:22"
std::format("{:%A}",                now);   // "Thursday"
std::format("{:%j}",                now);   // day-of-year, e.g. "120"
```

Durations and time points format directly — no `std::put_time` or stream gymnastics.

```cpp
using namespace std::chrono_literals;
std::format("{}", 250ms);          // "250ms"
std::format("{:%Q %q}", 1500ms);   // "1500 ms"
```

## Custom types: specialize `std::formatter<T>`

To make your own type work with `std::format`, specialize `std::formatter<T>`. The minimum is two functions: `parse` (read the spec inside `{}`) and `format` (produce the output).

```cpp
#include <format>
#include <string>

struct Point {
  double x, y;
};

template <>
struct std::formatter<Point> {
  // Accept an empty spec ("{}") for a minimal example.
  constexpr auto parse(std::format_parse_context& ctx) {
    return ctx.begin();
  }

  auto format(const Point& p, std::format_context& ctx) const {
    return std::format_to(ctx.out(), "({}, {})", p.x, p.y);
  }
};

int main() {
  Point p{1.5, 2.5};
  std::string s = std::format("p = {}", p);   // "p = (1.5, 2.5)"
}
```

For richer specs (e.g. forwarding a `:.2f` precision down to the members), inherit from `std::formatter<double>` and reuse its `parse` — that's the standard idiom but beyond the minimum.

## C++23: `std::print` and `std::println`

`std::format` builds a `std::string`, which you then have to send to `std::cout`. C++23's `<print>` cuts that step out.

```cpp
#include <print>

int main() {
  int x = 42;
  std::print("x = {}\n", x);     // writes to stdout, no newline added
  std::println("x = {}", x);     // appends '\n' for you
  std::println(stderr, "oops");  // first arg can be a FILE*
}
```

Same format-string syntax as `std::format`. No more `std::cout << ... << '\n';` boilerplate, no synchronization-with-stdio quirks, and the implementation can write to the OS handle directly without going through `std::ostream`.

## `printf` vs `iostream` vs `std::format`

| Aspect              | `printf`              | `std::ostream`                  | `std::format` / `std::print`         |
|---------------------|-----------------------|---------------------------------|--------------------------------------|
| Type safety         | None (UB on mismatch) | Yes (overload resolution)       | **Yes, checked at compile time**     |
| Format string       | One string, terse     | None — operators in code        | One string, terse and readable       |
| Locale-aware        | Optional (`%'d`)      | Yes (default)                   | Opt-in (`{:L}`)                      |
| User-defined types  | Impossible            | Overload `operator<<`           | Specialize `std::formatter<T>`       |
| Performance         | Fast                  | Slowest of the three            | Generally beats both                 |
| Threading           | Not synchronized      | Synchronized with C streams     | `std::print` writes atomically per call |
| Header              | `<cstdio>`            | `<iostream>`                    | `<format>`, `<print>` (C++23)        |
| Standard            | C89                   | C++98                           | C++20 (`format`), C++23 (`print`)    |

Rule of thumb in modern C++: **prefer `std::println` for output, `std::format` for building strings.** Reach for `iostream` only when you need stream semantics (e.g. user-defined `operator<<` for a third-party type) and for `printf` only in C-interop code.

## Related docs

- [basic_IO_operation.md](basic_IO_operation.md) — full tour of `iostream`, manipulators, file I/O, and stringstream
- [string.md](string.md) — `std::string`, conversions, and `std::string_view`

## References

- [cppreference: std::format](https://en.cppreference.com/w/cpp/utility/format/format)
- [cppreference: std::formatter](https://en.cppreference.com/w/cpp/utility/format/formatter)
- [cppreference: std::print](https://en.cppreference.com/w/cpp/io/print)
- [Format specification mini-language](https://en.cppreference.com/w/cpp/utility/format/spec)
