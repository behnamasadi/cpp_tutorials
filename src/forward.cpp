#include <iostream>
#include <utility>

/*
std::forward casts a templated function parameter back to the value category
(lvalue or rvalue) the caller used. It is the tool for writing a wrapper that
"perfectly forwards" its argument to another function.

Refs:
  https://stackoverflow.com/questions/3582001/what-are-the-main-purposes-of-using-stdforward-and-which-problems-it-solves
  https://www.youtube.com/watch?v=0xcCNnWEMgs
  http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2002/n1385.htm
*/

// A tiny class that announces which constructor fires.
struct Resource {
  Resource() { std::cout << "  Resource()" << std::endl; }
  Resource(const Resource &) { std::cout << "  Resource(const Resource&)  [copy]" << std::endl; }
  Resource(Resource &&) { std::cout << "  Resource(Resource&&)       [move]" << std::endl; }
};

// Two overloads so the reader can label which value category was picked.
void f(Resource &) { std::cout << "  f(Resource&)   picked lvalue overload" << std::endl; }
void f(Resource &&) { std::cout << "  f(Resource&&)  picked rvalue overload" << std::endl; }

// Wrapper that just passes the parameter along. Inside the function `t` has a
// name, so it is an lvalue, even if the caller handed us a temporary.
template <typename T> void wrap(T t) { f(t); }

// Same wrapper, but std::forward restores the original value category.
template <typename T> void wrap_forward(T t) { f(std::forward<T>(t)); }

int main() {
  std::cout << "--- direct call with lvalue ---" << std::endl;
  Resource s;
  f(s); // picks f(Resource&)

  std::cout << "\n--- direct call with rvalue ---" << std::endl;
  f(Resource()); // picks f(Resource&&)

  std::cout << "\n--- wrap(rvalue) without forward ---" << std::endl;
  // Wrong: inside wrap, t is a named lvalue, so f(Resource&) is chosen.
  wrap(Resource());

  std::cout << "\n--- wrap_forward(rvalue) with std::forward ---" << std::endl;
  // Correct: std::forward<T> casts t back to an rvalue, so f(Resource&&) wins.
  wrap_forward(Resource());
}
