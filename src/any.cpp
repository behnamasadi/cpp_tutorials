#include <algorithm>
#include <any>
#include <iostream>
#include <type_traits>

int main() {
  std::any a;

  a = 1;

  std::cout << std::any_cast<int>(a) << std::endl;

  // std::is_integral_v<int>;
}