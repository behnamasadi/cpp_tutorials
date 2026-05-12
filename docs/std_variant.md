# `std::variant` and `std::visit`

`std::variant` (C++17, `<variant>`) is a **type-safe union**: a value that, at any moment, holds exactly one of a fixed set of alternative types. Unlike the C `union`, the variant knows which alternative is active and a wrong `get` is detected at compile time or throws.

`std::visit` is the partner function that dispatches to the right overload for whichever alternative is currently active. Together they replace most uses of inheritance-based "discriminated union" hierarchies.

- [1. Basic usage](#1-basic-usage)
- [2. Querying the active alternative](#2-querying-the-active-alternative)
- [3. `std::visit` — the right way to consume a variant](#3-stdvisit--the-right-way-to-consume-a-variant)
- [4. Real-world examples](#4-real-world-examples)
- [5. `variant`, `optional`, `expected` — picking the right one](#5-variant-optional-expected--picking-the-right-one)
- [6. Edge cases and pitfalls](#6-edge-cases-and-pitfalls)

---

## 1. Basic usage

```cpp
#include <variant>
#include <string>

std::variant<int, std::string> v;   // default-initialized — holds int{0}
v = 42;                              // now holds int
v = std::string{"hello"};            // now holds std::string
```

Assignment destroys the old alternative and constructs the new one in place. The variant is never empty (except for the rare "valueless by exception" state — see §6).

To construct directly with a specific alternative when types overlap:

```cpp
std::variant<int, double, int> v{std::in_place_index<2>, 7};   // pick the second int
std::variant<int, std::string> w{std::in_place_type<std::string>, "hi"};
```

## 2. Querying the active alternative

```cpp
std::variant<int, std::string> v = 42;

if (std::holds_alternative<int>(v))            // true / false
    std::cout << std::get<int>(v);             // throws std::bad_variant_access if wrong
v.index();                                     // 0 (returns size_t — index into the type list)

if (auto p = std::get_if<int>(&v))             // pointer, nullptr if wrong alternative
    std::cout << *p;                           // no exception path
```

`std::get_if` is the right choice when "wrong alternative" is a normal outcome you want to branch on; `std::get` is for when you've already verified the alternative.

## 3. `std::visit` — the right way to consume a variant

Manually chaining `holds_alternative` / `get` is verbose and easy to get wrong (forget a type, add a new alternative and miss a branch). `std::visit` solves both: you pass a callable, and it's invoked with whichever alternative is currently active.

### 3.1 Generic lambda

```cpp
std::variant<int, double, std::string> v = 3.14;

std::visit([](auto&& x) {
    std::cout << x << '\n';
}, v);                                // prints 3.14 — operator<< picked for double
```

`auto&&` makes the lambda a template that accepts any alternative. If every alternative supports the operation (here, `operator<<`), nothing more is needed.

### 3.2 Branching on type with `if constexpr`

When alternatives need different handling, branch on the deduced type:

```cpp
std::visit([](auto&& x) {
    using T = std::decay_t<decltype(x)>;
    if constexpr (std::is_same_v<T, int>)
        std::cout << "int: "    << x;
    else if constexpr (std::is_same_v<T, double>)
        std::cout << "double: " << x;
    else if constexpr (std::is_same_v<T, std::string>)
        std::cout << "string: " << x;
}, v);
```

`if constexpr` means only the matching branch is instantiated — branches that wouldn't compile for the other alternatives (e.g. `x.size()` on an `int`) are fine.

### 3.3 Overload set (the cleanest idiom)

A small `overloaded` helper turns several lambdas into one callable, and `std::visit` picks the matching overload:

```cpp
template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;   // CTAD, C++17

std::visit(overloaded{
    [](int x)                { std::cout << "int "    << x; },
    [](double x)             { std::cout << "double " << x; },
    [](const std::string& s) { std::cout << "string " << s; },
}, v);
```

The compiler errors if any alternative is unhandled — your switch is **exhaustive by construction**. Adding a new alternative to the variant and forgetting to handle it becomes a compile error rather than a runtime surprise.

### 3.4 Returning a value

`std::visit` forwards the result. The common return type must exist (or be explicitly specified in C++20 via `std::visit<R>`):

```cpp
double as_double = std::visit(overloaded{
    [](int x)                  { return double(x); },
    [](double x)               { return x; },
    [](const std::string& s)   { return std::stod(s); },
}, v);
```

### 3.5 Multi-variant visit

You can pass several variants; the callable is invoked with one argument per variant:

```cpp
std::variant<int, double> a = 1;
std::variant<int, double> b = 2.0;

std::visit([](auto x, auto y) {
    std::cout << x + y;          // works for any (int, int), (int, double), …
}, a, b);
```

## 4. Real-world examples

### 4.1 Heterogeneous configuration values

```cpp
using ConfigValue = std::variant<int, double, std::string, bool>;

std::unordered_map<std::string, ConfigValue> config = {
    {"max_connections", 100},
    {"timeout",         30.5},
    {"username",        std::string{"admin"}},
    {"debug",           true},
};

for (auto& [k, v] : config) {
    std::cout << k << ": ";
    std::visit([](auto&& x) { std::cout << x << '\n'; }, v);
}
```

### 4.2 Shapes without inheritance

```cpp
struct Circle    { double radius; };
struct Rectangle { double w, h;   };
struct Triangle  { double base, height; };

using Shape = std::variant<Circle, Rectangle, Triangle>;

double area(const Shape& s) {
    return std::visit(overloaded{
        [](const Circle& c)    { return 3.14159 * c.radius * c.radius; },
        [](const Rectangle& r) { return r.w * r.h; },
        [](const Triangle& t)  { return 0.5 * t.base * t.height; },
    }, s);
}
```

No virtual dispatch, no heap allocation, exhaustive at compile time, and the shape types stay simple value types. This is the "sum type" pattern from functional languages — apt whenever the set of alternatives is closed and known up front.

### 4.3 Robot sensor messages

A single channel that can carry IMU, GPS, or LiDAR samples, dispatched in one place:

```cpp
struct Imu  { double ax, ay, az, gx, gy, gz; };
struct Gps  { double lat, lon, alt; };
struct Scan { std::vector<float> ranges; };

using SensorMsg = std::variant<Imu, Gps, Scan>;

void handle(const SensorMsg& m) {
    std::visit(overloaded{
        [](const Imu& i)  { ekf.predict(i); },
        [](const Gps& g)  { ekf.correct_position(g); },
        [](const Scan& s) { mapping.integrate(s); },
    }, m);
}
```

Adding a fourth sensor (`Barometer`) to the variant produces compile errors at every `visit` call that hasn't been updated — exactly the safety property you want when the message set changes.

## 5. `variant`, `optional`, `expected` — picking the right one

| Type | Holds | Use when |
|---|---|---|
| `std::optional<T>` | `T` or nothing | A value may be absent and absence carries no extra information. Cache lookups, `find_*`. |
| `std::variant<A, B, …>` | One of N concrete types | The set of alternatives is closed and known at compile time. Messages, shapes, parser tokens. |
| `std::expected<T, E>` (C++23) | `T` or `E` | Operation can succeed or fail, and failure carries a typed reason. Parsing, IO, validation. |
| `std::any` | Anything | Last resort — type-erased single value when alternatives aren't known. Heavier and visit-by-type-id only. |

A few decision rules:

**Lookup where "not found" is normal → `std::optional<T>`**

```cpp
std::optional<User> find_user(UserId id) const;
```

Absence is a valid outcome, not an error worth explaining. Use `std::optional<std::reference_wrapper<const User>>` to avoid the copy.

**Operation can fail with a reason → `std::expected<T, E>`**

```cpp
enum class ParseErr { Empty, InvalidNumber, OutOfRange };
std::expected<int, ParseErr> parse_port(std::string_view s);
```

Callers want to recover or report differently per error kind.

**A fixed set of alternative payloads → `std::variant`**

```cpp
using Token = std::variant<Number, Identifier, Operator, EndOfFile>;
```

Every alternative is "normal" — none is an error.

**Avoid `try`-style out-parameters** (`bool try_parse(string_view, int&)`). Use `optional` or `expected` instead — they're explicit at the call site and compose.

**Bridging the three:**

```cpp
std::expected<User, Err> require_user(UserId id) {
    if (auto u = find_user(id)) return *u;       // optional → expected
    return std::unexpected(Err::NotFound);
}
```

## 6. Edge cases and pitfalls

- **`valueless_by_exception()`** — if an assignment throws partway through, the variant can end up in a special "no alternative" state. `index() == variant_npos`, and `visit` throws `std::bad_variant_access`. Rare in practice (only when the new alternative's move/copy throws), but it's the answer to "why does `visit` have a throwing path even though I always set a value." Use types with `noexcept` move to avoid this entirely.
- **Reference and `void` alternatives are not allowed.** Use `std::reference_wrapper<T>` if you need a reference alternative.
- **Default construction picks the first alternative.** If that's not what you want, list a `std::monostate` first to make the variant default-constructible without committing to a real type.
- **`std::get<T>` is O(N) at compile time, O(1) at runtime.** `std::visit` generates a jump table indexed by `index()` — same cost as a `switch`.
- **Don't store `std::variant<Base*, Derived*>`.** That's not what variant is for; use a single base pointer with virtual dispatch, or a variant of value types.

[code](../src/std_variant.cpp), [code](../src/std_visit.cpp)
