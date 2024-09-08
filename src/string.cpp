/*
 * StringCpp.cpp
 *
 *  Created on: Jan 4, 2012
 *      Author: behnam
 */
#include <algorithm>
#include <cctype>
#include <iostream>
#include <iterator>
#include <locale.h>
#include <sstream>
#include <string.h> //c style api
#include <string>   //c++ style api
#include <string_view>
#include <vector>

#if defined WIN32
//#include <stringapiset.h>
#include <commctrl.h>
#include <ole2.h>
#include <strsafe.h>
#include <uxtheme.h>
#include <windows.h>
#include <windowsx.h>
#endif

void stringDataTypes() {
  const char *name0 = "behnam";
  char name1[6] = {'b', 'e', 'h', 'n', 'a', 'm'};

  // this will print lots of strange character until it hit the null termination
  // character 0
  std::cout << "name1: " << name1 << std::endl;

  char name2[7] = {'b', 'e', 'h', 'n',
                   'a', 'm', '\0'}; // or = { 'b','e', 'h', 'n', 'a','m', 0 };
  std::cout << "name2: " << name2 << std::endl;

  /*
      =================================== Character/ String literals
     ===================================
  */

  auto c0 = 'A';   // char
  auto c1 = u8'A'; // char
  auto c2 = L'A';  // wchar_t
  auto c3 = u'A';  // char16_t
  auto c4 = U'A';  // char32_t

  // Multicharacter literals
  auto m0 = 'abcd'; // int, value 0x61626364

  // String literals
  auto s0 = "hello";   // const char*
  auto s1 = u8"hello"; // const char*, encoded as UTF-8
  auto s2 = L"hello";  // const wchar_t*
  auto s3 = u"hello";  // const char16_t*, encoded as UTF-16
  auto s4 = U"hello";  // const char32_t*, encoded as UTF-32

  const char *multiline = R"(line1
        line2
        line3)";

  (void)c0;

  std::cout << "sizeof(char): " << sizeof(char) << std::endl;
  std::cout << "sizeof(wchar_t): " << sizeof(wchar_t) << std::endl;
  std::cout << "sizeof(char16_t): " << sizeof(char16_t) << std::endl;
  std::cout << "sizeof(char32_t): " << sizeof(char32_t) << std::endl;
  std::cout << "sizeof(std::string): " << sizeof(std::string) << std::endl;
  std::cout << "sizeof(std::wstring): " << sizeof(std::wstring) << std::endl;
  std::cout << "sizeof(std::u32string): " << sizeof(std::u32string)
            << std::endl;

  using namespace std::string_literals;
  // s is function which turn char array string into c++ string
  std::string name5 = "Behnam"s + "Asadi"s;
}

void spitingByDelimiter() {
  std::vector<std::string> spilitedString;
  std::string s = "scott>=tiger>=mushroom";
  std::string delimiter = ">=";

  size_t pos = 0;
  std::string token;
  while ((pos = s.find(delimiter)) != std::string::npos) {
    token = s.substr(0, pos);
    // std::cout << token << std::endl;
    spilitedString.push_back(token);
    s.erase(0, pos + delimiter.length());
  }

  for (std::size_t i = 0; i < spilitedString.size(); i++)
    std::cout << spilitedString[i] << std::endl;
}

// string to number and number to string
void stringNumberConversion() {
  std::string strNumber = std::to_string(10.3);
  float f = std::atof(strNumber.c_str());  // 10.3
  int i = std::stoi(strNumber.c_str());    // 10
  double d = std::stod(strNumber.c_str()); // 10.3
}

void cStyleString() {
  /*
  A string is a sequence of chars, strings are typically represented as char
  arrays that have a null terminator.
  '\0' character (which has ASCII code 0).
  Since C-style strings are arrays, you can use the [] operator to change
  individual characters in the string:
  */
  //////////////////////////// c style string definition
  ///////////////////////////////

  char name0[10] = {
      'b', 'e', 'h', 'n',
      'a', 'm', '\0'}; // stack variable, compiler output: char name0[10] =
                       // {'b', 'e', 'h', 'n', 'a', 'm', '\0', '\0', '\0',
                       // '\0'};
  char name1[] = {
      'b', 'e', 'h', 'n',
      'a', 'm', '\0'}; // stack variable, compiler output: char name1[7] = {'b',
                       // 'e', 'h', 'n', 'a', 'm', '\0'};
  char name2[] =
      "behnam"; // stack variable, compiler output: char name2[7] = "behnam";
  const char *name3 = "behnam"; // heap variable

  char name4[6] = {'b', 'e', 'h', 'n', 'a', 'm'};
  // this will print lots of strange character until it hit the null termination
  // character 0
  std::cout << "name4: " << name4 << std::endl;

  //////////////////////////// c style string elements access and modifications
  ///////////////////////////////

  char string1[] = "string";
  string1[1] = 'p';
  std::cout << string1 << std::endl;

  // but the following code is wrong
  //	string1 = "rope";

  char const *p2 = "Mary";
  // not allowed.
  //*p2[0] = 'C';
  p2 = "Margarethe"; // allowed

  char *const p1 = "John";
  p1[0] = 'C'; // allowed, but will cause segmentation fault
  // not allowed
  // p1 = "Margarethe";

  //////////////////////////// c style string copy ////////////////////////////
  char *cstr;
  std::string str("Please split this phrase into tokens");

  cstr = new char[str.size() + 1];
  strcpy(cstr, str.c_str());
  delete[] cstr;

  char string2[] = "Copy this!";
  char string2Dest[50];
  strcpy(string2Dest, string2);

  //////////////////////////// c style string split based on delimiter
  ///////////////////////////////
  /*
  Splits str[] according to given delimiters and returns next token. It needs to
  be called in a loop to get all tokens. It returns NULL when there are no more
  tokens. char * strtok(char str[], const char *delims);
  */
  char my_str[] = "Geeks-for-Geeks";
  // Returns first token
  char *token = strtok(my_str, "-");

  // Keep printing tokens while one of the  delimiters present in str[].
  while (token != NULL) {
    printf("%s\n", token);
    token = strtok(NULL, "-");
  }

  const char *sentence = "- This, a sample string.";
  const char *delimiter = " ,.-";
  char *p;
  p = strtok(strdup(sentence), delimiter);
  while (p != NULL) {
    std::cout << p << std::endl;
    p = strtok(NULL, " ");
  }
}

static uint32_t s_AllocCount = 0;

void *operator new(size_t size) {
  s_AllocCount++;
  std::cout << "Allocating: " << size << " bytes" << std::endl;
  return malloc(size);
}

void printName(const std::string &name) { std::cout << name << std::endl; }

void printName(const std::string_view &name) { std::cout << name << std::endl; }

void string_viewExample() {
  /*
       Dynamic memory allocation and deallocation (malloc, free and new, delete)
     are very slow operations when compared to automatic memory allocation and
     deallocation. In other words, the heap is much slower than the stack. If a
     static or non-large array has compile-time constant length, instead of a
     vector object, string, ... use a C-language array, std::array, or an array.
       If the array is large, such advantages are diminished and it becomes more
     important to avoid using too much stack space.

       To track memory allocation we overload new operator:
       void* operator new (size_t size)
   */

  // This will allocate memory
  std::string name = "Behnam Asadi";
  printName(name);
  std::cout << s_AllocCount << " allocated" << std::endl;

  // This one as well
  // printName("Behnam Asadi");

  std::string firstName = name.substr(0, 6);
  std::cout << s_AllocCount << " allocated" << std::endl;
  std::string lastName = name.substr(7, 11);
  std::cout << s_AllocCount << " allocated" << std::endl;

  printName(name.substr(0, 6));
  std::cout << s_AllocCount << " allocated" << std::endl;

  /*
  string_view const char * to an exsiting string (and size of the string)  owned
  by someone else.
  */
  std::size_t sizeOfString, startingPoint;
  sizeOfString = 6; // Behnam
  startingPoint = 7;

  std::string_view firstNameView(name.c_str(), sizeOfString);
  std::cout << s_AllocCount << " allocated" << std::endl;

  printName(firstNameView);
  std::cout << s_AllocCount << " allocated" << std::endl;

  sizeOfString = 5; // Asadi
  std::string_view lastNameView(name.c_str() + startingPoint, sizeOfString);
  std::cout << s_AllocCount << " allocated" << std::endl;

  std::cout << s_AllocCount << " allocated" << std::endl;
  printName(lastNameView);
  std::cout << s_AllocCount << " allocated" << std::endl;

  /*
  if instead of:
      std::string name = "Behnam Asadi";
  we use:
      char * name = "Behnam Asadi";
  we will have 0 allocation

  */
}

void shortStringOptimization() {

  std::size_t stringSize = sizeof(std::string);
  std::cout << stringSize << std::endl;

  for (std::size_t i = 0; i < stringSize; i++) {
    std::string s(i, 'X');
    std::cout << i << ":" << s << std::endl;
  }
}

/// Try to find in the word the sentence - case insensitive
bool findStringCaseInsensitive(const std::string &sentence,
                               const std::string &word) {
  auto it = std::search(sentence.begin(), sentence.end(), word.begin(),
                        word.end(), [](char ch1, char ch2) {
                          return std::toupper(ch1) == std::toupper(ch2);
                        });
  return (it != sentence.end());
}

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)

// this is warning free
std::wstring StringToWideString(std::string str) {
  if (str.empty()) {
    return std::wstring();
  }
  size_t len = str.length() + 1;
  std::wstring ret = std::wstring(len, 0);
#if defined WIN32
  size_t size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, &str[0],
                                    (int)str.size(), &ret[0], (int)len);
  ret.resize(size);
#else
  size_t size = 0;
  _locale_t lc = _create_locale(LC_ALL, "en_US.UTF-8");
  errno_t retval = _mbstowcs_s_l(&size, &ret[0], len, &str[0], _TRUNCATE, lc);
  _free_locale(lc);
  ret.resize(size - 1);
#endif
  return ret;
}

#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)

std::string WidestringToString(std::wstring wstr) {
  if (wstr.empty()) {
    return std::string();
  }
#if defined WIN32
  int size = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, &wstr[0],
                                 (int)wstr.size(), NULL, 0, NULL, NULL);
  std::string ret = std::string(size, 0);
  WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, &wstr[0], (int)wstr.size(),
                      &ret[0], size, NULL, NULL);
#else
  size_t size = 0;
  _locale_t lc = _create_locale(LC_ALL, "en_US.UTF-8");
  errno_t err = _wcstombs_s_l(&size, NULL, 0, &wstr[0], _TRUNCATE, lc);
  std::string ret = std::string(size, 0);
  err = _wcstombs_s_l(&size, &ret[0], size, &wstr[0], _TRUNCATE, lc);
  _free_locale(lc);
  ret.resize(size - 1);
#endif
  return ret;
}
#endif

#include <codecvt>
#include <locale>
#include <string>
// this will give you warning regarding depricated API
void WidestringStringConversion() {

  // std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  // std::string narrow = converter.to_bytes(wide_utf16_source_string);
  // std::wstring wide = converter.from_bytes(narrow_utf8_source_string);
}

int isalnumExample() {
  char c = 'A';
  if (std::isalnum(c)) {
    std::cout << c << " is alphanumeric." << std::endl;
  } else {
    std::cout << c << " is not alphanumeric." << std::endl;
  }
  return 0;
}
#include <stdio.h>

void strstrExample() {
  char haystack[] = "Hello, World!"; // Now this is modifiable
  const char *needle = "World";

  // Use strstr to find the first occurrence of the needle in the haystack
  char *result = strstr(haystack, needle);

  if (result) {
    printf("Found substring: %s\n", result);
  } else {
    printf("Substring not found.\n");
  }
}

#include <cstring> // for strdup and free

void strdupExample() {
  // Original string
  const char *original = "Hello, World!";

  // Duplicate the string using strdup
  char *duplicate = strdup(original);

  // Print both strings
  std::cout << "Original: " << original << std::endl;
  std::cout << "Duplicate: " << duplicate << std::endl;

  // Free the memory allocated by strdup
  free(duplicate);
}
int main() {

  // stringNumberConversion();
  // splitingByDelimiter();
}
