# C++20 Concepts

## What problem do concepts solve?

Before C++20, generic code looked like this:

```cpp
template <typename T>
T add(T a, T b) { return a + b; }

add(1, 2);                   // OK
add(std::string{}, 5);       // ❌ error — but where?
```

When you misuse a template, the compiler dives into the *body* of the function and prints page after page of cryptic errors about substitution deep inside the STL. There's no way to say *up front* "this template only works for types that support `+`."

The pre-C++20 workaround was [SFINAE](SFINAE.md) — using `std::enable_if`, `std::void_t`, and detection idioms to disable overloads that don't fit. It works, but it's verbose, hard to read, and the error messages are still terrible.

**Concepts** are the language-level fix. A concept is a *named, reusable predicate on types* that you can attach to a template parameter. The compiler checks the predicate **before** instantiating the template body, so errors point at the call site with a message like *"the constraint `Addable<std::string>` was not satisfied"*.

For background on the underlying mechanics, see [templates.md](templates.md) and [SFINAE.md](SFINAE.md).

## Defining a concept

The `concept` keyword introduces a compile-time boolean predicate. Inside, a `requires` expression lists what must be valid for the type.

```cpp
#include <concepts>

template <typename T>
concept Addable = requires(T a, T b) {
  a + b;            // simple-requirement: this expression must compile
};
```

That's the whole concept. `Addable<int>` is `true`, `Addable<std::string>` is `true`, `Addable<std::vector<int>>` is `false`.

### The four kinds of `requires` clauses

A `requires` expression can contain four kinds of requirement:

```cpp
#include <concepts>
#include <iostream>

template <typename T>
concept Container = requires(T c) {
  // 1. simple-requirement: the expression must be valid
  c.size();

  // 2. type-requirement: the type must exist
  typename T::value_type;

  // 3. compound-requirement: expression valid AND its result satisfies a concept
  { c.size() } -> std::convertible_to<std::size_t>;
  { c.begin() } -> std::same_as<typename T::iterator>;

  // 4. nested-requirement: an arbitrary boolean predicate
  requires std::default_initializable<T>;
};
```

What each one does:

| Form | Checks |
|------|--------|
| `expr;` | `expr` compiles (the result is discarded) |
| `typename T::X;` | `T::X` names a type |
| `{ expr } -> Concept;` | `expr` compiles **and** `decltype(expr)` satisfies `Concept` |
| `requires bool-expr;` | the boolean expression is `true` |

## Using a concept

Once defined, a concept can be used in four interchangeable ways. All four say "T must satisfy `Addable`":

```cpp
// 1. Replace `typename` with the concept name
template <Addable T>
T sum(T a, T b) { return a + b; }

// 2. `requires` clause after the template header
template <typename T>
  requires Addable<T>
T sum(T a, T b) { return a + b; }

// 3. Trailing `requires` clause
template <typename T>
T sum(T a, T b) requires Addable<T> { return a + b; }

// 4. Abbreviated function template (no template<> header at all)
auto sum(Addable auto a, Addable auto b) { return a + b; }
```

Form (4) is the shortest and the most common in modern code. Note that each `Addable auto` parameter introduces an *independent* template parameter — `sum(1, 2.0)` has two different `T`s.

You can also test a concept directly with `static_assert`:

```cpp
static_assert(Addable<int>);
static_assert(!Addable<void*>);
```

## Standard library concepts

The `<concepts>` header ships with a catalog of ready-to-use concepts. You almost never need to define `Addable` yourself.

```cpp
#include <concepts>
#include <ranges>
#include <iostream>

template <std::integral T>
T factorial(T n) {
  T r = 1;
  for (T i = 2; i <= n; ++i) r *= i;
  return r;
}

template <std::floating_point T>
T to_radians(T degrees) {
  return degrees * T(3.14159265358979) / T(180);
}

void print_all(std::ranges::range auto&& r) {
  for (auto&& x : r) std::cout << x << ' ';
  std::cout << '\n';
}

int main() {
  std::cout << factorial(5) << '\n';      // 120
  // factorial(5.0);                       // ❌ 5.0 is not std::integral
  std::cout << to_radians(180.0) << '\n'; // 3.14159

  std::vector v{1, 2, 3};
  print_all(v);                           // works on any range
}
```

The most useful ones to know:

| Concept | Means |
|---------|-------|
| `std::integral<T>` | `T` is an integer type (`int`, `long`, `char`, ...) |
| `std::floating_point<T>` | `T` is `float`, `double`, or `long double` |
| `std::same_as<T, U>` | `T` and `U` are the same type |
| `std::convertible_to<From, To>` | `From` is implicitly convertible to `To` |
| `std::derived_from<D, B>` | `D` is publicly derived from `B` |
| `std::invocable<F, Args...>` | `F` can be called with `Args...` |
| `std::ranges::range<R>` | `R` has `begin()` and `end()` |

## Overload resolution: most-constrained wins

When multiple constrained overloads match, the compiler picks the **more constrained** one. This is concepts' killer feature compared to SFINAE.

```cpp
#include <concepts>
#include <iostream>

template <typename T>
void describe(T) { std::cout << "anything\n"; }

template <std::integral T>
void describe(T) { std::cout << "integer\n"; }

template <std::same_as<int> T>
void describe(T) { std::cout << "exactly int\n"; }

int main() {
  describe(3.14);   // "anything"     — only the unconstrained one matches
  describe(42L);    // "integer"      — long is integral but not int
  describe(42);     // "exactly int"  — most constrained wins
}
```

The compiler arranges the concepts in a partial order: `same_as<int>` *subsumes* `integral` (because `int` implies integral but not the other way around), and both subsume the unconstrained template. Whichever overload sits highest in this lattice is chosen.

With SFINAE you'd have to manually exclude one overload from the other's domain (`enable_if<integral && !same_as<int>>`). With concepts the subsumption rules do it for you.

## Concepts vs SFINAE vs `if constexpr`

Three tools for writing code that adapts to its types — they solve different problems.

| Tool | Where it acts | When to reach for it |
|------|---------------|----------------------|
| **Concepts** | On the template signature | "Reject types that don't fit, with a clean error" |
| **SFINAE** | On the template signature (pre-C++20) | Same as concepts, but you're stuck on an older compiler |
| **`if constexpr`** | Inside the function body | "Same signature, different code path per type" |

A concrete contrast:

```cpp
// Concepts: two overloads, signature-level dispatch
template <std::integral T>     T process(T x) { return x * 2; }
template <std::floating_point T> T process(T x) { return x * 0.5; }

// if constexpr: one overload, body-level dispatch
template <typename T>
T process(T x) {
  if constexpr (std::integral<T>)            return x * 2;
  else if constexpr (std::floating_point<T>) return x * 0.5;
  else                                       static_assert(sizeof(T) == 0, "unsupported");
}
```

Both compile to identical code. Pick concepts when you want the cases visible in the API; pick `if constexpr` when the variation is an implementation detail the caller shouldn't see.

## Mental model

> A concept is a *named precondition on a type*, checked at the template boundary. Failing it produces "type doesn't meet `MyConcept`," not 200 lines of mangled STL output.

Concepts didn't add any new power to templates — everything they do was already possible with SFINAE. What they added was **readability**, **better error messages**, and **automatic overload ordering by specificity**. That's enough that, in C++20 and later, you should reach for concepts first and only drop to SFINAE when forced to.

## References

- [cppreference: Constraints and concepts](https://en.cppreference.com/w/cpp/language/constraints)
- [cppreference: `<concepts>` library](https://en.cppreference.com/w/cpp/concepts)
- [templates.md](templates.md) — template basics
- [SFINAE.md](SFINAE.md) — the pre-C++20 way of doing this
