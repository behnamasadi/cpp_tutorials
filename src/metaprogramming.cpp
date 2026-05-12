// Companion to docs/metaprogramming.md
//
// Compile-time programming with templates. Each demo is tiny and prints a
// label. Compile-time results use static_assert; runtime-visible values use
// std::cout.

#include <iostream>
#include <type_traits>

// --- 1. Template metaprogramming: recursive Fibonacci ---------------------
// The compiler unrolls Fibonacci<N> into a chain of specializations.
template <int N> struct Fibonacci {
  static constexpr int value = Fibonacci<N - 1>::value + Fibonacci<N - 2>::value;
};
template <> struct Fibonacci<0> { static constexpr int value = 0; };
template <> struct Fibonacci<1> { static constexpr int value = 1; };

// --- 2. constexpr function: same idea, modern syntax ----------------------
// Useful for sizing a lookup table of motor commands at compile time.
constexpr int factorial(int n) { return (n <= 1) ? 1 : n * factorial(n - 1); }

// --- 3. Type traits: inspect types at compile time ------------------------
template <typename T> void describe(const char *name) {
  std::cout << name << ": integral=" << std::is_integral<T>::value
            << " floating=" << std::is_floating_point<T>::value << '\n';
}

// --- 4. SFINAE: enable a template only for some types ---------------------
// Only integral types match this overload; others are silently ignored.
template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type twice(T x) {
  return x + x;
}

// --- 5. Variadic templates + fold expression (C++17) ----------------------
// Sum any number of arguments. The fold "(args + ...)" expands at compile time.
template <typename... Args> auto sum(Args... args) { return (args + ...); }

// --- 6. constexpr if (C++17): pick a branch at compile time ---------------
template <typename T> void print_kind(T) {
  if constexpr (std::is_integral_v<T>)
    std::cout << "got an integer\n";
  else
    std::cout << "got something else\n";
}

int main() {
  std::cout << "--- 1. Template recursion (Fibonacci) ---\n";
  static_assert(Fibonacci<10>::value == 55, "fib(10) == 55");
  std::cout << "Fibonacci<10> = " << Fibonacci<10>::value << '\n';

  std::cout << "\n--- 2. constexpr function ---\n";
  static_assert(factorial(5) == 120, "5! == 120");
  constexpr int f5 = factorial(5);
  std::cout << "factorial(5) = " << f5 << '\n';

  std::cout << "\n--- 3. Type traits ---\n";
  describe<int>("int");
  describe<double>("double");

  std::cout << "\n--- 4. SFINAE (enable_if) ---\n";
  std::cout << "twice(7) = " << twice(7) << '\n';
  // twice(3.14) would not compile: no matching overload.

  std::cout << "\n--- 5. Variadic templates + fold ---\n";
  std::cout << "sum(1,2,3,4) = " << sum(1, 2, 3, 4) << '\n';

  std::cout << "\n--- 6. constexpr if ---\n";
  print_kind(42);
  print_kind(3.14);

  std::cout << "\n--- 7. static_assert ---\n";
  static_assert(sizeof(int) >= 2, "int must be at least 16 bits");
  std::cout << "static_assert checks passed at compile time\n";

  return 0;
}
