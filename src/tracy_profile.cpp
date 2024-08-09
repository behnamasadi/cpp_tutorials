#include <chrono>
#include <iostream>
#include <memory>
#include <tracy/Tracy.hpp>
int main() {

  for (;;) {
    ZoneScoped;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "foo" << std::endl;
  }
}

/*
cmake -S . -DCMAKE_INSTALL_PREFIX=~/usr -B build
cmake --build build/
cmake --install build/
*/