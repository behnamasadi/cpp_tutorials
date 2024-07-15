#include <cmath>
#include <exception>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>

int sub(int a, int b) { return a - b; }

void inc(int a, int &b, const int &c) {
  std::cout << "In function: " << a << ' ' << b << ' ' << c << '\n';

  a++;
  b++;
  return;
}

int main() {
  // double number = 3.1914;

  // std::cout << "default std::cout.precision(): " << std::cout.precision()
  //           << "\n";

  // std::cout << number << "\n";
  // std::cout << "std::setprecision(1): " << std::setprecision(1) << number
  //           << "\n";

  // std::cout << "std::fixed <<std::setprecision(7): " << std::fixed
  //           << std::setprecision(7) << number << "\n";

  // int n = 10;

  // std::cout << "Using pow : " << pow(10, n) << std::endl;
  // std::cout << "std::fixed <<" << std::fixed << pow(10, n) << std::endl;

  // std::cout << std::invoke(sub, 2, 5) << std::endl;
  // std::cout << std::invoke(sub, 5, 2) << std::endl;

  // auto subFunc1 = std::bind(sub, std::placeholders::_1,
  // std::placeholders::_2); auto subFunc2 = std::bind(sub,
  // std::placeholders::_2, std::placeholders::_1);

  // std::cout << subFunc1(2, 5) << std::endl;
  // std::cout << subFunc2(2, 5) << std::endl;

  // auto f = std::bind(foo, std::ref(x));
  int a, b, c;

  a = 1;
  b = 10;
  c = 100;
  std::cout << "before bind: " << a << ' ' << b << ' ' << c << '\n';
  auto f = std::bind(inc, a, std::ref(b), std::cref(c));
  a = 5;
  b = 50;
  c = 500;
  std::cout << "before call: " << a << ' ' << b << ' ' << c << '\n';
  f();
  std::cout << "after call: " << a << ' ' << b << ' ' << c << '\n';
}