#include <functional>
#include <iostream>

void foo(int x) { std::cout << "foo(" << x << ")" << std::endl; }

struct bar {
  void operator()(int x) const { std::cout << "bar(" << x << ")" << std::endl; }
};

int main() {
  int x = 42;

  // Invoke a function pointer.
  std::invoke(foo, x);

  // Invoke a member function.
  bar b;
  std::invoke(&bar::operator(), b, x);

  // Invoke a function object.
  std::invoke(std::function<void(int)>([](int x) {
                std::cout << "lambda(" << x << ")" << std::endl;
              }),
              x);

  return 0;
}