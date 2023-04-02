#include <algorithm>
#include <filesystem>
#include <iostream>
#include <optional>
#include <regex>

// regex can be verified at https://regex101.com/

std::optional<std::string>
findFileWithRegexPattern(const std::string &searchPath,
                         const std::regex &regex) {
  try {
    for (auto const &iter : std::filesystem::directory_iterator{searchPath}) {
      const std::string file = iter.path().filename().string();
      if (std::filesystem::is_regular_file(iter)) {

        if (std::regex_match(file, regex)) {
          std::cout << "file exist: " << file.c_str();
          return (iter.path().string());
        }
      }
    }
  } catch (std::exception &) {
  }
  return std::nullopt;
}

template <typename T>
void searchingMapUsingRegex(std::regex regx, std::map<T, T>)
// https://stackoverflow.com/questions/17253690/finding-in-a-std-map-using-regex
}

int main(int argc, char **argv) {

  std::regex filename_regex("[0-5]?([a-z][A-Z])*");
  std::string file_names[] = {"kINgtd", "9wpmdcI", "3bghIU"};

  for (auto const &file_name : file_names) {
    if (std::regex_match(file_name, filename_regex)) {
      std::cout << file_name << std::endl;
    }
  }

  std::smatch file_name_match;

  for (auto const &file_name : file_names) {

    if (std::regex_search(file_name, file_name_match, filename_regex)) {
      if (std::regex_match(file_name, filename_regex)) {
        std::cout << file_name_match.prefix() << std::endl;
      }
    }
  }
}