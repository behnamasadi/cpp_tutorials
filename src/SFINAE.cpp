#include <iostream>
#include <string>
#include <type_traits>
#include <utility>

// Trait: does T support expression: a / b ?
template <typename T, typename = void> struct has_division : std::false_type {};

template <typename T>
struct has_division<
    T, std::void_t<decltype(std::declval<T>() / std::declval<T>())>>
    : std::true_type {};

// Overload 1: enabled only when T supports division
template <typename T, std::enable_if_t<has_division<T>::value, int> = 0>
T divide(const T &a, const T &b) {
  return a / b;
}

// Overload 2: enabled only when T does NOT support division
template <typename T, std::enable_if_t<!has_division<T>::value, int> = 0>
void divide(const T &, const T &) {
  std::cout << "divide(): no operator/ for this type\n";
}

int main() {
  std::cout << divide(10.0, 2.0) << "\n"; // OK -> 5
  std::cout << divide(10, 3) << "\n";     // OK -> 3 (integer division)

  std::string s1 = "hi", s2 = "there";
  divide(s1, s2); // picks fallback overload
}
