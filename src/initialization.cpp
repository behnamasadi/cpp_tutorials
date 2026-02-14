#include <initializer_list>
#include <iostream>
#include <map>
#include <string>
#include <vector>

// ------------------------------------------------------------
// 1) Constructor Member Initializer List
// ------------------------------------------------------------
struct WithMemberInitList {
  const int id;     // must be initialized in initializer list
  std::string name; // more efficient to initialize than default+assign

  WithMemberInitList(int i, std::string n) : id(i), name(std::move(n)) {}
};

// ------------------------------------------------------------
// 2) Aggregate Initialization (Brace Initialization for aggregates)
// ------------------------------------------------------------
struct AggregateS {
  int i;
  double d;
};

// ------------------------------------------------------------
// 3) Copy Initialization
// ------------------------------------------------------------
struct Copyable {
  int v{};
  Copyable(int x) : v(x) {}

  // show when copy ctor is actually used
  Copyable(const Copyable &other) : v(other.v) {
    std::cout << "Copyable: copy ctor\n";
  }
};

// ------------------------------------------------------------
// 4) Direct Initialization
// ------------------------------------------------------------
struct DirectInit {
  int x;
  explicit DirectInit(int a) : x(a) {}
};

// ------------------------------------------------------------
// 5) Default Initialization
// ------------------------------------------------------------
struct DefaultInitDemo {
  int built_in; // if default-initialized, this is indeterminate
  // no user-defined ctor => aggregate-like rules for members, but still default
  // init here
};

// ------------------------------------------------------------
// 6) Value Initialization
// ------------------------------------------------------------
struct ValueInitClass {
  int x;
  ValueInitClass() : x(123) {} // value-initialization calls this default ctor
};

// ------------------------------------------------------------
// 7) Zero Initialization (static / thread_local)
// ------------------------------------------------------------
static int g_static_zero; // zero-initialized (static storage duration)

// ------------------------------------------------------------
// 8) Uniform / List Initialization (general brace syntax)
// ------------------------------------------------------------
struct ListInitCtor {
  int a;
  double b;
  ListInitCtor(int x, double y) : a(x), b(y) {}
};

// ------------------------------------------------------------
// 9) In-class Member Initializers
// ------------------------------------------------------------
struct InClassInit {
  int x = 10;     // default member initializer
  double y{3.14}; // default member initializer
};

// ------------------------------------------------------------
// 10) std::initializer_list constructor (custom brace handling)
//     This is NOT aggregate initialization; it's list-init calling a ctor.
// ------------------------------------------------------------
class IntArray {
  std::vector<int> data;

public:
  IntArray(std::initializer_list<int> init) : data(init) {}
  void print() const {
    std::cout << "IntArray: ";
    for (int v : data)
      std::cout << v << ' ';
    std::cout << "\n";
  }
};

// ------------------- helper functions ------------------------

void demo_1_member_initializer_list() {
  std::cout << "\n1) Member initializer list\n";
  WithMemberInitList a(7, "Alice");
  std::cout << "id=" << a.id << ", name=" << a.name << "\n";
}

void demo_2_aggregate_initialization() {
  std::cout << "\n2) Aggregate initialization\n";
  AggregateS s{10, 2.5};    // aggregate init
  AggregateS t = {11, 3.5}; // copy-list init, still aggregate init here
  std::cout << "s.i=" << s.i << ", s.d=" << s.d << "\n";
  std::cout << "t.i=" << t.i << ", t.d=" << t.d << "\n";

  // Your specific example:
  struct S {
    int i;
  };
  S u{10}; // aggregate initialization
  std::cout << "u.i=" << u.i << "\n";
}

void demo_3_copy_initialization() {
  std::cout << "\n3) Copy initialization\n";
  Copyable a = Copyable(5); // copy-init form; in practice may be elided
  Copyable b = a;           // copy initialization => copy ctor likely called
  std::cout << "a.v=" << a.v << ", b.v=" << b.v << "\n";
}

void demo_4_direct_initialization() {
  std::cout << "\n4) Direct initialization\n";
  DirectInit d(42); // direct-init: calls ctor
  std::cout << "d.x=" << d.x << "\n";
}

void demo_5_default_initialization() {
  std::cout << "\n5) Default initialization\n";

  int x; // default-initialized fundamental => indeterminate (do NOT print x)

  DefaultInitDemo obj; // built_in is indeterminate (do NOT print obj.built_in)

  std::cout << "Default initialization for fundamentals leaves indeterminate "
               "values.\n";
  std::cout << "So we do NOT print 'x' or 'obj.built_in'.\n";
}

void demo_6_value_initialization() {
  std::cout << "\n6) Value initialization\n";

  int a{};       // value-init => zero
  int b = int(); // value-init => zero
  std::cout << "a=" << a << ", b=" << b << "\n";

  ValueInitClass c{}; // calls default ctor, sets x=123
  std::cout << "c.x=" << c.x << "\n";
}

void demo_7_zero_initialization() {
  std::cout << "\n7) Zero initialization\n";
  std::cout << "g_static_zero=" << g_static_zero
            << " (static storage => zero-initialized)\n";

  static double local_static; // also zero-initialized
  std::cout << "local_static=" << local_static << "\n";
}

void demo_8_uniform_list_initialization() {
  std::cout << "\n8) Uniform/List initialization\n";

  int a{42}; // list-init
  std::vector<int> v{1, 2, 3};
  std::map<std::string, int> m{{"Scott", 1976}, {"Dijkstra", 1972}};

  ListInitCtor obj{7, 2.5}; // list-init calling ctor
  std::cout << "a=" << a << ", v.size()=" << v.size()
            << ", m.size()=" << m.size() << "\n";
  std::cout << "obj.a=" << obj.a << ", obj.b=" << obj.b << "\n";

  // Narrowing protection example (uncomment to see compile error):
  // int bad{3.14}; // error: narrowing
}

void demo_9_in_class_member_initializers() {
  std::cout << "\n9) In-class member initializers\n";
  InClassInit a; // uses defaults x=10, y=3.14
  std::cout << "a.x=" << a.x << ", a.y=" << a.y << "\n";

  InClassInit b;
  b.x = 99; // you can still change after construction
  std::cout << "b.x=" << b.x << ", b.y=" << b.y << "\n";
}

void demo_10_initializer_list_constructor() {
  std::cout << "\n10) std::initializer_list constructor\n";
  IntArray arr{1, 2, 3, 4}; // list-init calls IntArray(initializer_list)
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
