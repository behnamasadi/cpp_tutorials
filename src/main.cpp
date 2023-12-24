#include <cmath>
#include <exception>
#include <iomanip>
#include <iostream>
#include <memory>

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
void abortExample() {
  Foo fooObject("abortExample");
  std::shared_ptr<Foo> fooObject_ptr(new Foo("abortExample_ptr"));
  atexit(runBeforeTermination);
  abort();
}

int main() {
  int array[5] = {0, 1, 2, 3, 4};

  std::cout << "Original array values:\n";
  for (int i = 0; i < 5; i++) {
    std::cout << "array[" << i << "] = " << array[i] << "\n";
  }

  // Overflowing the array
  // Writing beyond the bounds of the array
  std::cout << "\nWriting beyond the bounds of the array...\n";
  for (int i = 0; i < 10;
       i++) { // This loop intentionally goes well beyond the array's bounds
    array[i] = i;
  }

  // std::cout << "\nReading values after overflow:\n";
  // // Reading values after overflow
  // for (int i = 0; i < int(100);
  //      i++) { // Again, intentionally reading beyond the array's bounds
  //   std::cout << "array[" << i << "] = " << array[i] << "\n";
  //   std::cout << "i = " << i << "\n";
  // }
}