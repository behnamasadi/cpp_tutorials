// Tiny demos mirroring docs/const_constexpr_mutable.md.
// Each function shows ONE concept with the shortest possible snippet.

#include <iostream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// ---------------------------------------------------------------------------
// const variables: west-const vs east-const (same meaning)
// ---------------------------------------------------------------------------
void const_variables() {
  const int west = 1;   // west-const
  int const east = 2;   // east-const
  std::cout << "west=" << west << " east=" << east << '\n';
}

// ---------------------------------------------------------------------------
// const parameter: function cannot modify the argument
// ---------------------------------------------------------------------------
void take_by_const_ref(const std::string& s) {
  // s = "no";  // would not compile
  std::cout << "got: " << s << '\n';
}

void const_parameter() { take_by_const_ref("hello"); }

// ---------------------------------------------------------------------------
// const return: caller cannot modify the returned value
// ---------------------------------------------------------------------------
const int give_back() { return 7; }

void const_return() { std::cout << "returned: " << give_back() << '\n'; }

// ---------------------------------------------------------------------------
// const member functions: const methods cannot change *this
// ---------------------------------------------------------------------------
struct Foo {
  int i;
  int non_const_func() { return i; }       // not const
  int const_func() const { return i; }     // const: promises no mutation
};

void const_methods() {
  const Foo cf{10};
  Foo nf{20};
  std::cout << "const obj calls const: " << cf.const_func() << '\n';
  std::cout << "non-const obj calls both: " << nf.const_func() << ", "
            << nf.non_const_func() << '\n';
}

// ---------------------------------------------------------------------------
// const/non-const getter overloads (de-duplicated with const_cast)
// ---------------------------------------------------------------------------
struct Bar {
  int v = 99;
};

class Holder {
  Bar m_bar;

 public:
  // non-const getter forwards to const one via const_cast
  Bar& get() {
    return const_cast<Bar&>(const_cast<const Holder*>(this)->get());
  }
  const Bar& get() const { return m_bar; }
};

void const_getter_overload() {
  Holder h;
  const Holder ch;
  std::cout << "non-const get: " << h.get().v
            << ", const get: " << ch.get().v << '\n';
}

// ---------------------------------------------------------------------------
// const iterator
// ---------------------------------------------------------------------------
void const_iterator() {
  std::vector<int> vec{3, 4, 8};
  std::cout << "cbegin->cend:";
  for (std::vector<int>::const_iterator it = vec.cbegin(); it != vec.cend();
       ++it) {
    std::cout << ' ' << *it;
  }
  std::cout << '\n';
}

// ---------------------------------------------------------------------------
// const pointers: pointer-to-const vs const-pointer
// ---------------------------------------------------------------------------
void const_pointers() {
  int a = 1, b = 2;

  const int* p1 = &a;  // data is fixed: *p1 cannot change, p1 can repoint
  p1 = &b;             // ok
  // *p1 = 5;          // error

  int* const p2 = &a;  // pointer is fixed: p2 cannot repoint, *p2 can change
  *p2 = 5;             // ok
  // p2 = &b;          // error

  std::cout << "*p1=" << *p1 << " *p2=" << *p2 << '\n';
}

// ---------------------------------------------------------------------------
// const_cast: cast const to non-const, and non-const to const (std::as_const)
// ---------------------------------------------------------------------------
void only_takes_mutable(char* s) { std::cout << "mutable buf: " << s << '\n'; }

class Overloaded {
 public:
  void f() { std::cout << "non-const f\n"; }
  void f() const { std::cout << "const f\n"; }
};

void const_cast_demo() {
  std::string msg = "hi";
  only_takes_mutable(const_cast<char*>(msg.c_str()));  // remove const

  Overloaded o;
  o.f();                  // calls non-const
  std::as_const(o).f();   // adds const -> calls const overload
}

// ---------------------------------------------------------------------------
// mutable: modify a member even from a const method
// ---------------------------------------------------------------------------
class Student {
  std::string name;
  mutable int debug_counter;  // can change in const methods

 public:
  Student(std::string n) : name(std::move(n)), debug_counter(0) {}
  std::string get_name() const {
    ++debug_counter;  // ok because mutable
    return name;
  }
  int hits() const { return debug_counter; }
};

void mutable_member() {
  const Student s("jumbo");
  s.get_name();
  s.get_name();
  std::cout << "name=" << s.get_name() << " hits=" << s.hits() << '\n';
}

void mutable_lambda() {
  int x = 0;
  auto inc = [=]() mutable {
    ++x;  // modifies the lambda's own copy
    std::cout << "lambda x=" << x << '\n';
  };
  inc();
  inc();
  std::cout << "outer x still=" << x << '\n';
}

// ---------------------------------------------------------------------------
// constexpr: compile-time evaluable function/variable
// ---------------------------------------------------------------------------
constexpr int multiply_by_10(int x) { return 10 * x; }
constexpr long fib(int n) { return n <= 1 ? n : fib(n - 1) + fib(n - 2); }

void constexpr_demo() {
  constexpr int v = multiply_by_10(5);   // computed at compile time
  static_assert(v == 50, "compile-time check");

  constexpr long f10 = fib(10);
  int arr[fib(5)] = {};                  // array size needs constant expr
  std::cout << "multiply_by_10(5)=" << v << " fib(10)=" << f10
            << " sizeof(arr)/sizeof(int)=" << sizeof(arr) / sizeof(int)
            << '\n';
}

// ---------------------------------------------------------------------------
// constexpr vs const: const may be runtime-initialized; constexpr cannot
// ---------------------------------------------------------------------------
int runtime_value() { return 42; }  // not constexpr

void constexpr_vs_const() {
  const int a = runtime_value();    // const, but initialized at runtime
  constexpr int b = multiply_by_10(2);  // must be compile-time
  std::cout << "const(runtime)=" << a << " constexpr=" << b << '\n';
}

// ---------------------------------------------------------------------------
// C++14/17/20 constexpr relaxation: loops + local mutation allowed
// ---------------------------------------------------------------------------
constexpr int sum_to(int n) {       // illegal in C++11, fine since C++14
  int total = 0;
  for (int i = 1; i <= n; ++i) total += i;
  return total;
}

void constexpr_cxx14() {
  constexpr int s = sum_to(10);
  static_assert(s == 55);
  std::cout << "sum_to(10)=" << s << '\n';
}

// ---------------------------------------------------------------------------
// C++20 consteval: MUST run at compile time
// ---------------------------------------------------------------------------
consteval int square(int x) { return x * x; }

void consteval_demo() {
  constexpr int s = square(5);   // ok: 5 is a constant expression
  // int n = 5; square(n);       // error: n is not constant
  std::cout << "square(5)=" << s << '\n';
}

// ---------------------------------------------------------------------------
// C++20 constinit: compile-time init, but still mutable later
// ---------------------------------------------------------------------------
constinit int g_counter = 42;  // initialized at compile time

void constinit_demo() {
  ++g_counter;  // still mutable at runtime
  std::cout << "g_counter=" << g_counter << '\n';
}

// ---------------------------------------------------------------------------
// std::is_const trait
// ---------------------------------------------------------------------------
void is_const_check() {
  std::cout << std::boolalpha;
  std::cout << "is_const<const int>            = "
            << std::is_const_v<const int> << '\n';
  std::cout << "is_const<const int&>           = "
            << std::is_const_v<const int&> << '\n';  // false: reference
  std::cout << "is_const<remove_ref<const int&>> = "
            << std::is_const_v<std::remove_reference_t<const int&>> << '\n';
  std::cout << "is_const<const int*>           = "
            << std::is_const_v<const int*> << '\n';  // false: pointer mutable
  std::cout << "is_const<int* const>           = "
            << std::is_const_v<int* const> << '\n';  // true
}

// ---------------------------------------------------------------------------
int main() {
  auto header = [](const char* h) { std::cout << "\n== " << h << " ==\n"; };

  header("const variables");          const_variables();
  header("const parameter");          const_parameter();
  header("const return");             const_return();
  header("const methods");            const_methods();
  header("const/non-const getter");   const_getter_overload();
  header("const iterator");           const_iterator();
  header("const pointers");           const_pointers();
  header("const_cast");               const_cast_demo();
  header("mutable member");           mutable_member();
  header("mutable lambda");           mutable_lambda();
  header("constexpr");                constexpr_demo();
  header("constexpr vs const");       constexpr_vs_const();
  header("constexpr C++14 (loops)");  constexpr_cxx14();
  header("consteval (C++20)");        consteval_demo();
  header("constinit (C++20)");        constinit_demo();
  header("std::is_const");            is_const_check();
  return 0;
}
