#include <cstdlib> // for  exit()
#include <iostream>
#include <memory>

struct Foo {
  std::string id;
  Foo(std::string id) {
    this->id = id;
    std::cout << id << " constructor" << std::endl;
  }
  ~Foo() { std::cout << id << " destructor" << std::endl; }
};

Foo globalFoo("globalFoo");

static Foo staticFoo("staticFoo");

void runBeforeTermination() {
  std::cout << "cleaning up temp dir before termination..." << std::endl;
}

int retunExample() {
  //  run the program and then   echo $?
  Foo fooObject("retunExample");

  return 5;
}

int exitExample() {
  Foo fooObject("exitExample");
  std::shared_ptr<Foo> fooObject_ptr(new Foo("exitExample_ptr"));
  //    atexit (runBeforeTermination);

  exit(7);
}

void abortExample() {
  Foo fooObject("abortExample");
  std::shared_ptr<Foo> fooObject_ptr(new Foo("abortExample_ptr"));
  atexit(runBeforeTermination);
  abort();
}

int main(int argc, char *argv[]) {
  atexit(runBeforeTermination);

  // return exitExample();
  // abortExample();
}
