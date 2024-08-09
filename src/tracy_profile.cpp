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

https://davespace.xyz/blog/building-tracy-profiler-on-linux
git clone https://github.com/capstone-engine/capstone.git

https://github.com/wolfpld/tracy/issues/484


https://www.youtube.com/watch?v=W9U5y5jjQDM

cmake -S . -DCMAKE_INSTALL_PREFIX=~/usr -B build
cmake --build build/
cmake --install build/
*/
