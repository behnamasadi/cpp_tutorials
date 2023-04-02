#include <algorithm>
#include <filesystem>
#include <iostream>
#include <optional>
#include <regex>

std::optional<std::string>
findFileWithRegexPattern(const std::string &searchPath,
                         const std::regex &regex) {
  const std::filesystem::directory_iterator end;
  try {
    for (std::filesystem::directory_iterator iter{searchPath}; iter != end;
         iter++) {
      const std::string file = iter->path().filename().string();
      if (std::filesystem::is_regular_file(*iter)) {
        // regex can be verified at https://regex101.com/
        if (std::regex_match(file, regex)) {
          std::cout << "hash exist: " << file.c_str();
          return (iter->path().string());
        }
      }
    }
  } catch (std::exception &) {
  }
  return std::nullopt;
}

int main(int argc, char **argv) {}