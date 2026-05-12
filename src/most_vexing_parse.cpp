// Most Vexing Parse
//
// If a piece of syntax can be parsed as a declaration, C++ parses it as a
// declaration. That rule turns what looks like an object definition into a
// function declaration.

#include <iostream>

class MyClass {
public:
  MyClass() { std::cout << "MyClass constructed\n"; }
};

// --- The trap -------------------------------------------------------------
// This looks like "default-construct an object 'a' of type MyClass",
// but it actually declares a function 'a' taking no arguments and
// returning MyClass. No constructor runs.
MyClass a();

// --- Fix 1: uniform (brace) initialization --------------------------------
void fix_brace_init() {
  std::cout << "[brace init] ";
  MyClass b{}; // unambiguous: object, not a function
}

// --- Fix 2: drop the parentheses entirely ---------------------------------
void fix_no_parens() {
  std::cout << "[no parens] ";
  MyClass c; // default-initialized object
}

int main() {
  std::cout << "--- the trap: MyClass a(); is a function declaration ---\n";
  // Calling 'a' would require a definition; we just note that no object
  // was created at file scope.
  std::cout << "no constructor ran for 'a'\n";

  std::cout << "--- fix 1: brace init ---\n";
  fix_brace_init();

  std::cout << "--- fix 2: no parens ---\n";
  fix_no_parens();
}
