// Meaning of =default, =delete, and =0 in C++.
//
// =default : ask the compiler to generate the special member function.
// =delete  : forbid the function; calling it is a compile error.
// =0       : on a virtual function, declare it pure virtual (abstract).

#include <iostream>
#include <type_traits>

// ---------- =default ----------
// Foo has a user-defined constructor, so the compiler would NOT generate a
// default constructor. We explicitly default it so Foo() still works.
struct Foo {
  int i;
  Foo(int a, int b) : i(a), j(b) {}
  Foo() = default;          // explicitly defaulted -> Foo stays trivial-ish
private:
  int j;
};

// Replacing `Foo() = default;` with `Foo() {}` would make it non-trivial.
struct FooEmptyBody {
  int i;
  FooEmptyBody(int a, int b) : i(a), j(b) {}
  FooEmptyBody() {}         // user-provided empty body -> non-trivial
private:
  int j;
};

// ---------- =delete ----------
// Noncopyable: copy operations are forbidden.
struct Noncopyable {
  Noncopyable() = default;
  Noncopyable(const Noncopyable&) = delete;
  Noncopyable& operator=(const Noncopyable&) = delete;
};

// Movable-only: copies deleted, moves defaulted.
struct MoveOnly {
  MoveOnly() = default;
  MoveOnly(const MoveOnly&) = delete;
  MoveOnly& operator=(const MoveOnly&) = delete;
  MoveOnly(MoveOnly&&) = default;
  MoveOnly& operator=(MoveOnly&&) = default;
  ~MoveOnly() = default;
};

// ---------- =0 (pure virtual) ----------
struct Shape {
  virtual double area() const = 0;   // pure virtual -> Shape is abstract
  virtual ~Shape() = default;
};

struct Square : Shape {
  double side;
  Square(double s) : side(s) {}
  double area() const override { return side * side; }
};

// ---------- Implicit deletion examples (from the doc) ----------
struct A {
  int x;
  A(int x = 1) : x(x) {}             // user-defined default constructor
};

struct B : A { };                    // B() implicitly defined, calls A::A()

struct C { A a; };                   // C() implicitly defined, calls A::A()

struct E : A {
  E(int y) : A(y) {}
  E() = default;                     // explicitly defaulted, calls A::A()
};

int main() {
  std::cout << "--- =default makes Foo trivially-constructible ---\n";
  Foo f;
  std::cout << "Foo() compiles, Foo.i = " << f.i << "\n";
  std::cout << "is_trivially_default_constructible<Foo>      = "
            << std::is_trivially_default_constructible<Foo>::value << "\n";
  std::cout << "is_trivially_default_constructible<FooEmpty> = "
            << std::is_trivially_default_constructible<FooEmptyBody>::value
            << "  (empty body {} is NOT trivial)\n";

  std::cout << "\n--- =delete forbids copies ---\n";
  Noncopyable n1;
  // Noncopyable n2 = n1;   // would not compile: copy ctor deleted
  // n1 = n1;               // would not compile: copy assign deleted
  (void)n1;
  std::cout << "Noncopyable built; copy/assign would be a compile error\n";

  std::cout << "\n--- move-only via =delete + =default ---\n";
  MoveOnly m1;
  MoveOnly m2 = std::move(m1);       // move ok
  // MoveOnly m3 = m2;               // would not compile: copy deleted
  (void)m2;
  std::cout << "MoveOnly moved; copy would be a compile error\n";

  std::cout << "\n--- =0 pure virtual makes a class abstract ---\n";
  // Shape s;                        // would not compile: abstract type
  Square sq(3.0);
  Shape& sref = sq;
  std::cout << "Square(3).area() via Shape& = " << sref.area() << "\n";

  std::cout << "\n--- implicit default ctor follows from members/bases ---\n";
  A a; B b; C c; E e;
  std::cout << "A.x=" << a.x << "  B.x=" << b.x
            << "  C.a.x=" << c.a.x << "  E.x=" << e.x << "\n";
}
