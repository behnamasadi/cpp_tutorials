#include <iostream>
#include <memory>

#include <stdio.h>
#include <string>

void *operator new(size_t size) {
  std::cout << "Allocating " << size << " bytes" << std::endl;
  return malloc(size);
}

void operator delete(void *memory, size_t size) {
  std::cout << "Freeing " << size << " bytes" << std::endl;
  free(memory);
}

void operator delete(void *memory) {
  std::cout << "Freeing " << std::endl;
  free(memory);
}

struct S {
  double a;
  int b;
  float c;
};

int main() {

  {
    std::cout << "allocating and freeing memory for struct S" << std::endl;
    std::cout << "size of struct S is: " << sizeof(S) << " bytes" << std::endl;
    S *my_S = new S;
    delete my_S;
  }

  std::cout << "************************************************" << std::endl;

  {
    std::cout
        << "allocating and freeing memory for struct S with unique pointers: "
        << std::endl;
    std::unique_ptr<S> my_S = std::unique_ptr<S>(new S);
    std::unique_ptr<S> bject_ptr = std::make_unique<S>();
  }

  std::cout << "************************************************" << std::endl;

  {
    std::string str;
    std::cout << "allocating and freeing memory for strings" << std::endl;
    for (std::size_t i = 0; i < 25; i++) {
      str = str + std::to_string(i);
      std::cout << "size of string is: " << str.size()
                << " bytes and string is: " << str << std::endl;
    }
  }
  return 0;
}
