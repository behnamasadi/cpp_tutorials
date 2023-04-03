#include <algorithm>
#include <filesystem>
#include <iostream>
#include <map>
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
void mapFindingUsingRegex(std::regex regex, std::map<T, T> map) {
  // https://stackoverflow.com/questions/17253690/finding-in-a-std-map-using-regex
  return;
}

template <typename T>
std::vector<T>::iterator vectorFindingRegex(std::regex regex,
                                            std::vector<T> vec) {
  return std::find_if(vec.begin(), vec.end(), [regex](T item) {
    std::cout << regex_search(item, regex);
    return regex_search(item, regex);
  });
}

int main(int argc, char **argv) {

  /*
  regex_search will successfully match any subsequence of the given sequence,
  whereas std::regex_match will only return true if the regular expression
  matches the entire sequence.
  */

  std::regex filename_regex("[0-5]+([a-z][A-Z])*");
  std::vector<std::string> file_names = {"eybI7", "3bghIU", "kINgtd",
                                         "9wpmdcI"};

  /*

    for (auto const &file_name : file_names) {
      if (std::regex_search(file_name, filename_regex)) {
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
  */
  // std::cout << vectorFindingRegex(filename_regex, file_names) -
  //                  file_names.begin();

  // std::vector<std::string>::iterator find_it = std::find_if(
  //     file_names.begin(), file_names.end(), [filename_regex](std::string
  //     item) {
  //       std::cout << std::regex_search(item, filename_regex);
  //       return std::regex_search(item, filename_regex);
  //     });

  if (auto it = std::find_if(file_names.begin(), file_names.end(),
                             [filename_regex](std::string item) {
                               return std::regex_search(item, filename_regex);
                             });
      it != file_names.end()) {
    std::cout << "The first valid regex is " << *it
              << " and the index is: " << it - file_names.begin() << std::endl;
  } else {
    std::cout << "No matching regex found" << std::endl;
  }

  const std::string s = "/home/toto/FILE_mysymbol_EVENT.DAT";
  std::regex rgx(".*FILE_(\\w+)_EVENT\\.DAT.*");
  std::smatch match;

  if (std::regex_search(s.begin(), s.end(), match, rgx)) {
    std::cout << "match: " << match[1] << '\n';
    std::cout << "match: " << match.suffix().str() << '\n';
    std::cout << "match: " << match.prefix().str() << '\n';
  }
}