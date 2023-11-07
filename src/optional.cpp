#include <iostream>
#include <optional>
#include <string>

std::optional<std::string> found(bool found = false) {
  if (found)
    return "something found!";
  else
    return std::nullopt;
}

int main(int argc, char **argv) {
  std::cout << found(false).value_or("empty") << '\n';
  if (found(true).has_value()) {
    std::cout << found(true).value() << '\n';
  }
}