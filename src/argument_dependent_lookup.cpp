// Argument Dependent Lookup (ADL), a.k.a. Koenig Lookup.
//
// When you make an unqualified function call, the compiler doesn't only look
// in the current scope and its enclosing scopes. It also looks in the
// namespaces associated with the argument types. That's ADL.
//
// Reference: https://en.cppreference.com/w/cpp/language/adl

#include <iostream>

// ---------------------------------------------------------------------------
// 1. The canonical example from the doc.
// ---------------------------------------------------------------------------
namespace my_namespace {
struct MyType {};

void func(MyType) {
  std::cout << "my_namespace::func(MyType) called via ADL\n";
}
}  // namespace my_namespace

// ---------------------------------------------------------------------------
// 2. ADL is what makes `std::cout << x` work without writing `std::operator<<`.
//    operator<< is found in namespace std because std::ostream lives there.
// ---------------------------------------------------------------------------
namespace demo_stream {
void show() {
  // No `using namespace std;` and no `std::operator<<` qualification.
  // ADL finds operator<<(std::ostream&, const char*) in namespace std
  // because std::cout's type is associated with namespace std.
  std::cout << "ADL finds operator<< in namespace std\n";
}
}  // namespace demo_stream

// ---------------------------------------------------------------------------
// 3. ADL applies to namespaces, not classes.
//    A free function inside a class is NOT found by ADL on objects of that
//    class. Only namespace-scope functions associated with the argument's
//    type are considered.
// ---------------------------------------------------------------------------
namespace adl_limit {
struct Widget {
  // A static function inside a class is not reached by ADL.
  static void poke(Widget) {
    std::cout << "Widget::poke (called with explicit qualification)\n";
  }
};

// This free function IS in the same namespace as Widget, so ADL finds it.
void nudge(Widget) { std::cout << "adl_limit::nudge found via ADL\n"; }
}  // namespace adl_limit

// ---------------------------------------------------------------------------
// 4. ADL only kicks in for unqualified calls. A qualified call disables it.
// ---------------------------------------------------------------------------
namespace qualified_demo {
struct Tag {};
void ping(Tag) { std::cout << "qualified_demo::ping via ADL\n"; }
}  // namespace qualified_demo

int main() {
  // --- (1) Basic ADL: no my_namespace:: prefix needed. -------------------
  my_namespace::MyType obj;
  func(obj);  // resolved via ADL to my_namespace::func

  // --- (2) The everyday case you forget is ADL. --------------------------
  demo_stream::show();

  // --- (3) ADL is namespace-scoped, not class-scoped. --------------------
  adl_limit::Widget w;
  nudge(w);                    // found by ADL
  adl_limit::Widget::poke(w);  // class members need explicit qualification

  // --- (4) Unqualified vs qualified call. --------------------------------
  qualified_demo::Tag t;
  ping(t);                 // unqualified -> ADL finds qualified_demo::ping
  qualified_demo::ping(t); // qualified call, no ADL needed

  return 0;
}
