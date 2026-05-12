// Lambda expressions in C++.
//
// Syntax:
//   [ captures ] ( params ) { body }
//   [ captures ] ( params ) -> ret { body }
//
// Each demo below mirrors one section of docs/lambda.md.

#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>

// ---------- inline lambda ----------
void demoInline() {
  std::cout << "--- inline lambda ---\n";
  std::vector<int> v = {1, 8, 5, 6, 3, 4, 0, 9, 7, 2};
  std::sort(v.begin(), v.end(), [](int x, int y) { return x < y; });
  for (int n : v)
    std::cout << n << ' ';
  std::cout << '\n';
}

// ---------- lambda stored in a variable, used outside the call ----------
void demoOutside() {
  std::cout << "--- lambda stored in a variable ---\n";
  auto cmp = [](int x, int y) { return x < y; };
  std::vector<int> v = {1, 8, 5, 6, 3, 4, 0, 9, 7, 2};
  std::sort(v.begin(), v.end(), cmp);
  for (int n : v)
    std::cout << n << ' ';
  std::cout << '\n';
}

// ---------- lambda defined at file scope, used as a function ----------
auto increaser = [](int &i) { i = i + 1; };

void demoOutsideAsFunction() {
  std::cout << "--- lambda at file scope used like a function ---\n";
  std::vector<int> v = {1, 2, 3};
  std::for_each(v.begin(), v.end(), increaser);
  for (int n : v)
    std::cout << n << ' ';
  std::cout << '\n';
}

// ---------- capture by value with mutable ----------
void demoCaptureByValue() {
  std::cout << "--- capture by value + mutable ---\n";
  int a = 10;
  auto lambda = [=]() mutable {
    ++a;
    std::cout << "inside lambda: a = " << a << '\n';
  };
  lambda();
  std::cout << "outside lambda: a = " << a << '\n';
}

// ---------- capture by reference ----------
void demoCaptureByReference() {
  std::cout << "--- capture by reference ---\n";
  int a = 10;
  auto lambda = [&]() {
    ++a;
    std::cout << "inside lambda: a = " << a << '\n';
  };
  lambda();
  std::cout << "outside lambda: a = " << a << '\n';
}

// ---------- mixed captures ----------
void demoMixedCapture() {
  std::cout << "--- mixed capture [=, &y] ---\n";
  int x = 1, y = 1;
  auto lambda = [=, &y]() mutable {
    x = 100; // modifies the copy
    y = 100; // modifies the original
  };
  lambda();
  std::cout << "x = " << x << " (unchanged), y = " << y << " (changed)\n";
}

// ---------- accept a lambda via a function template ----------
template <typename Func> void executeTemplate(Func f) { f(); }

void demoAcceptViaTemplate() {
  std::cout << "--- function template accepting a lambda ---\n";
  auto hello = []() { std::cout << "Hello from lambda!\n"; };
  executeTemplate(hello);
  executeTemplate([]() { std::cout << "Direct lambda call!\n"; });
}

// ---------- accept a lambda via std::function ----------
void executeStdFunction(const std::function<void()> &f) { f(); }

void demoAcceptViaStdFunction() {
  std::cout << "--- std::function accepting a lambda ---\n";
  auto hello = []() { std::cout << "Hello from lambda!\n"; };
  executeStdFunction(hello);
  executeStdFunction([]() { std::cout << "Direct lambda call!\n"; });
}

int main() {
  demoInline();
  demoOutside();
  demoOutsideAsFunction();
  demoCaptureByValue();
  demoCaptureByReference();
  demoMixedCapture();
  demoAcceptViaTemplate();
  demoAcceptViaStdFunction();
  return 0;
}
