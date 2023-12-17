#include <algorithm>
#include <cmath>
#include <exception>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct Foo {
  std::string id;
  Foo(std::string id) : id(id) {

    std::cout << id << " constructor" << std::endl;
  }
  ~Foo() { std::cout << id << " destructor" << std::endl; }
};

Foo globalFoo("global object");
static Foo staticFoo("static object");

void runBeforeTermination() {
  std::cout << "cleaning up temp dir before termination..." << std::endl;
}

int abortExample() {
  Foo fooObject("abortExample");
  std::shared_ptr<Foo> fooObject_ptr(new Foo("abortExample_ptr"));
  atexit(runBeforeTermination);
  abort();
}

void myTerminate() {
  std::cout << "Custom terminate handler\n";
  abort(); // ensure program termination
}

int main() {
  std::set_terminate(myTerminate); // Set custom terminate handler

  throw std::runtime_error(
      "Unhandled exception"); // This will trigger terminate
}