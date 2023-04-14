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

  using namespace std::regex_constants;

  std::cmatch m;

  // std::regex_match("subject", m, std::regex("(sub)(.*)"));

  std::regex_match(
      "00000-2023-02-23_16-13-23.ulg_"
      "06531353AC6AFC26C7633316B317B58A680E5D59039EE71E6D09B0A950A5F087.hash",
      m, std::regex("00000-2023-02-23_16-13-23.ulg_[A-F0-9]*\\.hash"));

  std::cout << "--------------"
            << "\n";
  std::string output = "matches:\n";
  for (std::cmatch::iterator it = m.begin(); it != m.end(); ++it) {
    std::cout << it->str() << "\n";
    std::cout << "************"
              << "\n";
    output += it->str() + "\n";
  }
  std::cout << "......................"
            << "\n";
  std::cout << output << std::endl;

  std::regex rx("00000-2023-02-23_16-13-23.ulg_[A-F0-9]*\\.hash");
  std::cmatch mr;

  std::regex_search(
      "00000-2023-02-23_16-13-23.ulg_"
      "06531353AC6AFC26C7633316B317B58A680E5D59039EE71E6D09B0A950A5F087.hash",
      mr, rx);

  std::csub_match sub = mr[1];
  std::cout << "matched == " << std::boolalpha << sub.matched << std::endl;
  std::cout << "length == " << sub.length() << std::endl;

  std::csub_match::difference_type dif = std::distance(sub.first, sub.second);
  std::cout << "difference == " << dif << std::endl;

  std::csub_match::iterator first = sub.first;
  std::csub_match::iterator last = sub.second;
  std::cout << "range == " << std::string(first, last) << std::endl;
  std::cout << "string == " << sub << std::endl;

  std::csub_match::value_type const *ptr = "aab";
  std::cout << "compare(\"aab\") == " << sub.compare(ptr) << std::endl;
  std::cout << "compare(string) == " << sub.compare(std::string("AAA"))
            << std::endl;
  std::cout << "compare(sub) == " << sub.compare(sub) << std::endl;

  std::string mainStr =
      "00000-2023-02-23_16-13-23.ulg_"
      "06531353AC6AFC26C7633316B317B58A680E5D59039EE71E6D09B0A950A5F087.hash";
  std::string toErase = "00000-2023-02-23_16-13-23.ulg_";
  size_t pos = mainStr.find(toErase);
  if (pos != std::string::npos) {
    // If found then erase it from string
    mainStr.erase(pos, toErase.length());
  }

  toErase = ".hash";
  pos = mainStr.find(toErase);

  if (pos != std::string::npos) {
    // If found then erase it from string
    mainStr.erase(pos, toErase.length());
  }

  std::cout << mainStr;
}

// https://www.regular-expressions.info/dot.html
// https://www3.ntu.edu.sg/home/ehchua/programming/howto/Regexe.html#:~:text=The%20%5C%20is%20known%20as%20the,have%20special%20meaning%20in%20regex.

/*
To match a string that contains only those characters (or an empty string), try
"^[a-zA-Z0-9_]*$"

^ : start of string
[ : beginning of character group
a-z : any lowercase letter
A-Z : any uppercase letter
0-9 : any digit
_ : underscore
] : end of character group
* : zero or more of the given characters
$ : end of string
*/

/*
https://en.cppreference.com/w/cpp/regex
https://en.cppreference.com/w/cpp/regex/sub_match
https://cplusplus.com/reference/regex/sub_match/str/
https://stackoverflow.com/questions/39946618/c-regex-how-to-use-sub-matches
https://learn.microsoft.com/en-us/cpp/standard-library/sub-match-class?view=msvc-170
*/