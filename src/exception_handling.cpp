// Demos that mirror docs/exception_handling.md.
// Each demo throws inside its own try/catch so main() stays clean.

#include <bitset>
#include <cmath>
#include <exception>
#include <iostream>
#include <new>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <vector>

// --- basic try / catch / catch(...) ----------------------------------------
void basicTryCatch() {
  try {
    throw std::runtime_error("something went wrong");
  } catch (const std::runtime_error& e) {
    std::cout << "caught runtime_error: " << e.what() << '\n';
  } catch (...) {
    std::cout << "caught unknown exception\n";
  }
}

// --- std::bad_alloc --------------------------------------------------------
void badAllocDemo() {
  try {
    // Ask for an absurd amount until allocation fails.
    while (true) {
      new int[100000000ul];
    }
  } catch (const std::bad_alloc& e) {
    std::cout << "bad_alloc: " << e.what() << '\n';
  }
}

// --- nothrow new -----------------------------------------------------------
void nothrowDemo() {
  // Non-throwing new returns nullptr on failure instead of throwing.
  int* p = new (std::nothrow) int[100000000ul];
  if (p == nullptr) {
    std::cout << "nothrow new returned nullptr\n";
  } else {
    std::cout << "nothrow new succeeded\n";
    delete[] p;
  }
}

// --- std::bad_cast ---------------------------------------------------------
struct A { virtual ~A() {} };
struct B { virtual ~B() {} };

void badCastDemo() {
  B b;
  try {
    A& a = dynamic_cast<A&>(b);
    (void)a;
  } catch (const std::bad_cast& e) {
    std::cout << "bad_cast: " << e.what() << '\n';
  }
}

// --- std::bad_typeid -------------------------------------------------------
struct Poly { virtual void f() {} };

void badTypeidDemo() {
  Poly* p = nullptr;
  try {
    std::cout << typeid(*p).name() << '\n';
  } catch (const std::bad_typeid& e) {
    std::cout << "bad_typeid: " << e.what() << '\n';
  }
}

// --- std::logic_error ------------------------------------------------------
void logicErrorDemo() {
  try {
    int amount = 10, available = 9;
    if (amount > available)
      throw std::logic_error("amount exceeds available");
  } catch (std::exception& e) {
    std::cout << "logic_error: " << e.what()
              << " (type: " << typeid(e).name() << ")\n";
  }
}

// --- std::domain_error -----------------------------------------------------
void domainErrorDemo() {
  try {
    // acos is only defined on [-1, 1]; 2.0 is outside the domain.
    const double x = std::acos(2.0);
    std::cout << x << '\n';
  } catch (const std::domain_error& e) {
    std::cout << "domain_error: " << e.what() << '\n';
  }
}

// --- std::invalid_argument -------------------------------------------------
void invalidArgumentDemo() {
  try {
    // 'X' is not a valid binary digit.
    std::bitset<32> bits(std::string("0101001X01010110000"));
  } catch (const std::exception& e) {
    std::cout << "invalid_argument: " << e.what()
              << " (type: " << typeid(e).name() << ")\n";
  }
}

// --- std::length_error -----------------------------------------------------
void lengthErrorDemo() {
  try {
    std::vector<int> v;
    v.resize(v.max_size() + 1);
  } catch (const std::length_error& e) {
    std::cout << "length_error: " << e.what() << '\n';
  }
}

// --- std::out_of_range -----------------------------------------------------
void outOfRangeDemo() {
  std::vector<int> v(10);
  try {
    v.at(20) = 100;
  } catch (const std::out_of_range& e) {
    std::cout << "out_of_range: " << e.what() << '\n';
  }
}

// --- std::overflow_error ---------------------------------------------------
void overflowErrorDemo() {
  try {
    std::bitset<100> bits;
    bits[99] = 1;
    bits[0] = 1;
    // Result does not fit in unsigned long.
    unsigned long n = bits.to_ulong();
    (void)n;
  } catch (const std::exception& e) {
    std::cout << "overflow_error: " << e.what()
              << " (type: " << typeid(e).name() << ")\n";
  }
}

// --- std::range_error ------------------------------------------------------
void rangeErrorDemo() {
  try {
    throw std::range_error("The range is in error!");
  } catch (const std::range_error& e) {
    std::cout << "range_error: " << e.what() << '\n';
  }
}

// --- catch(...) ------------------------------------------------------------
void catchAllDemo() {
  try {
    throw 42; // not derived from std::exception
  } catch (const std::exception& e) {
    std::cout << "std::exception: " << e.what() << '\n';
  } catch (...) {
    std::cout << "catch(...) caught a non-std exception\n";
  }
}

// --- user-defined exception ------------------------------------------------
struct CustomException : public std::exception {
  const char* what() const noexcept override {
    return "CustomException happened";
  }
};

void customExceptionDemo() {
  try {
    throw CustomException();
  } catch (const std::exception& e) {
    std::cout << "custom: " << e.what() << '\n';
  }
}

// --- noexcept and stack unwinding ------------------------------------------
struct Traced {
  std::string name;
  Traced(std::string n) : name(std::move(n)) {
    std::cout << "  ctor " << name << '\n';
  }
  ~Traced() noexcept {
    std::cout << "  dtor " << name << " (unwinding)\n";
  }
};

void mayThrow() {
  Traced a("a");
  Traced b("b");
  throw std::runtime_error("boom");
}

void stackUnwindingDemo() {
  try {
    mayThrow();
  } catch (const std::exception& e) {
    std::cout << "caught after unwinding: " << e.what() << '\n';
  }
}

// --- noexcept function -----------------------------------------------------
void neverThrows() noexcept {
  std::cout << "neverThrows() ran\n";
}

void noexceptDemo() {
  neverThrows();
}

// --- main ------------------------------------------------------------------
int main() {
  std::cout << "== basic try/catch ==\n";
  basicTryCatch();

  std::cout << "\n== bad_alloc ==\n";
  badAllocDemo();

  std::cout << "\n== nothrow new ==\n";
  nothrowDemo();

  std::cout << "\n== bad_cast ==\n";
  badCastDemo();

  std::cout << "\n== bad_typeid ==\n";
  badTypeidDemo();

  std::cout << "\n== logic_error ==\n";
  logicErrorDemo();

  std::cout << "\n== domain_error ==\n";
  domainErrorDemo();

  std::cout << "\n== invalid_argument ==\n";
  invalidArgumentDemo();

  std::cout << "\n== length_error ==\n";
  lengthErrorDemo();

  std::cout << "\n== out_of_range ==\n";
  outOfRangeDemo();

  std::cout << "\n== overflow_error ==\n";
  overflowErrorDemo();

  std::cout << "\n== range_error ==\n";
  rangeErrorDemo();

  std::cout << "\n== catch(...) ==\n";
  catchAllDemo();

  std::cout << "\n== custom exception ==\n";
  customExceptionDemo();

  std::cout << "\n== stack unwinding ==\n";
  stackUnwindingDemo();

  std::cout << "\n== noexcept ==\n";
  noexceptDemo();
}
