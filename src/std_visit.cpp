#include <iostream>
#include <string>
#include <variant>

int main() {
  std::variant<int, float, std::string> myVariant = 10;

  auto visitor = [](auto &&arg) {
    using T = std::decay_t<decltype(arg)>; // Get the type of the argument
    if constexpr (std::is_same_v<T, int>)
      std::cout << "It's an int: " << arg << std::endl;
    else if constexpr (std::is_same_v<T, float>)
      std::cout << "It's a float: " << arg << std::endl;
    else if constexpr (std::is_same_v<T, std::string>)
      std::cout << "It's a string: " << arg << std::endl;
  };

  std::visit(visitor, myVariant); // Output: It's an int: 10

  myVariant = "Hello";
  std::visit(visitor, myVariant); // Output: It's a string: Hello

  myVariant = 3.14f;
  std::visit(visitor, myVariant); // Output: It's a float: 3.14

  return 0;
}
