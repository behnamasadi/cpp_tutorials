// Initialization forms in C++ — one demo per section in docs/initialization.md.
// Each demo prints a labeled header. Gotchas are shown via comments or
// alternative forms.

#include <initializer_list>
#include <iostream>
#include <map>
#include <string>
#include <vector>

// ------------------------------------------------------------
// 1) Constructor member initializer list
// ------------------------------------------------------------
struct Foo {
  const int   id;      // const member: initializer list is the ONLY way
  std::string name;    // non-trivial: avoid default-construct-then-assign

  Foo(int i, std::string n) : id(i), name(std::move(n)) {}
};

// Declaration-order trap: members are initialized in declaration order,
// NOT in the order written in the initializer list.
struct OrderTrap {
  int  capacity;       // declared first  -> initialized first
  int *data;           // declared second -> initialized second

  // Written as if capacity comes first; that's also the declaration order,
  // so this is fine. Swapping the declarations would make `new int[capacity]`
  // read an uninitialized capacity. (See -Wreorder-ctor.)
  OrderTrap(int n) : capacity(n), data(new int[n]) {}
  ~OrderTrap() { delete[] data; }
};

// ------------------------------------------------------------
// 2) Aggregate initialization
// ------------------------------------------------------------
struct S {
  int i;
};

struct Aggregate2 {
  int    i;
  double d;
};

// ------------------------------------------------------------
// 3) Copy initialization
// ------------------------------------------------------------
struct Copyable {
  int v;
  Copyable(int x) : v(x) {}
  Copyable(const Copyable &o) : v(o.v) {
    std::cout << "  (copy ctor called)\n";
  }
};

// ------------------------------------------------------------
// 4) Direct initialization
// ------------------------------------------------------------
struct Direct {
  int x;
  explicit Direct(int a) : x(a) {}
};

// ------------------------------------------------------------
// 5) Default initialization
// ------------------------------------------------------------
struct DefaultDemo {
  int built_in;        // default-initialized => indeterminate
};

// ------------------------------------------------------------
// 6) Value initialization
// ------------------------------------------------------------
struct ValueDemo {
  int x;
  ValueDemo() : x(123) {}   // called by value-init: ValueDemo{}
};

// ------------------------------------------------------------
// 7) Zero initialization
// ------------------------------------------------------------
static int g_zero;          // static storage duration => zero-initialized

// ------------------------------------------------------------
// 8) Uniform / list initialization
// ------------------------------------------------------------
struct TwoFields {
  int    a;
  double b;
  TwoFields(int x, double y) : a(x), b(y) {}
};

// ------------------------------------------------------------
// 9) In-class member initializers
// ------------------------------------------------------------
struct InClassInit {
  int    x = 10;
  double y{3.14};
};

// ------------------------------------------------------------
// 10) std::initializer_list constructor (NOT aggregate init)
// ------------------------------------------------------------
class IntArray {
  std::vector<int> data;
public:
  IntArray(std::initializer_list<int> list) : data(list) {}
  void print() const {
    std::cout << "  IntArray: ";
    for (int v : data) std::cout << v << ' ';
    std::cout << "\n";
  }
};

// -------------------------- demos ----------------------------

void demo_1_member_initializer_list() {
  std::cout << "\n1) Member initializer list\n";
  Foo f(7, "alice");
  std::cout << "  id=" << f.id << " name=" << f.name << "\n";

  OrderTrap ot(4);
  std::cout << "  OrderTrap.capacity=" << ot.capacity << "\n";
}

void demo_2_aggregate_initialization() {
  std::cout << "\n2) Aggregate initialization\n";
  S s{10};                      // canonical example from the doc
  S s_copy_list = {20};         // copy-list form, still aggregate init
  Aggregate2 a{1, 2.5};
  std::cout << "  s.i=" << s.i << "\n";
  std::cout << "  s_copy_list.i=" << s_copy_list.i << "\n";
  std::cout << "  a.i=" << a.i << " a.d=" << a.d << "\n";

  // Narrowing is rejected by brace init:
  //   S bad{3.14}; // error: narrowing conversion from double to int
}

void demo_3_copy_initialization() {
  std::cout << "\n3) Copy initialization\n";
  int a = 5;
  int b = a;
  std::cout << "  a=" << a << " b=" << b << "\n";

  Copyable c(1);                // direct-init for comparison
  Copyable d = c;               // copy initialization -> copy ctor
  std::cout << "  d.v=" << d.v << "\n";
}

void demo_4_direct_initialization() {
  std::cout << "\n4) Direct initialization\n";
  Direct d(42);
  int    x(5);
  std::cout << "  d.x=" << d.x << " x=" << x << "\n";

  // Most vexing parse gotcha:
  //   Direct dd();   // declares a FUNCTION named dd returning Direct,
  //                  // it does NOT default-construct a Direct.
  // Use Direct dd{}; or Direct dd; instead.
}

void demo_5_default_initialization() {
  std::cout << "\n5) Default initialization\n";
  int x;                        // indeterminate -- do not read
  DefaultDemo obj;              // obj.built_in indeterminate -- do not read
  (void)x; (void)obj;
  std::cout << "  built-in default-init leaves indeterminate values; not printed\n";
}

void demo_6_value_initialization() {
  std::cout << "\n6) Value initialization\n";
  int a{};                      // zero
  int b = int();                // zero
  ValueDemo v{};                // calls default ctor -> x=123
  std::cout << "  a=" << a << " b=" << b << " v.x=" << v.x << "\n";
}

void demo_7_zero_initialization() {
  std::cout << "\n7) Zero initialization\n";
  static int local_static;      // zero-initialized too
  std::cout << "  g_zero=" << g_zero << " local_static=" << local_static << "\n";
}

void demo_8_uniform_list_initialization() {
  std::cout << "\n8) Uniform / list initialization\n";
  int                         a{42};
  std::vector<int>            v{1, 2, 3};
  std::map<std::string, int>  m{{"a", 1}, {"b", 2}};
  TwoFields                   t{7, 2.5};

  std::cout << "  a=" << a << " v.size=" << v.size()
            << " m.size=" << m.size() << "\n";
  std::cout << "  t.a=" << t.a << " t.b=" << t.b << "\n";

  // Narrowing protection:
  //   int bad{3.14}; // error: narrowing conversion
}

void demo_9_in_class_member_initializers() {
  std::cout << "\n9) In-class member initializers\n";
  InClassInit a;
  std::cout << "  a.x=" << a.x << " a.y=" << a.y << "\n";
}

void demo_10_initializer_list_constructor() {
  std::cout << "\n10) std::initializer_list constructor\n";
  IntArray arr{1, 2, 3, 4};
  arr.print();
}

int main() {
  demo_1_member_initializer_list();
  demo_2_aggregate_initialization();
  demo_3_copy_initialization();
  demo_4_direct_initialization();
  demo_5_default_initialization();
  demo_6_value_initialization();
  demo_7_zero_initialization();
  demo_8_uniform_list_initialization();
  demo_9_in_class_member_initializers();
  demo_10_initializer_list_constructor();
  return 0;
}
