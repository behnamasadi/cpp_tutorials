# regex
The `<regex>` header in C++ provides several interesting features and components that are particularly useful for handling regular expressions. Here are some of the key elements:

1. **`std::regex` Class**: This is the main class for storing and using regular expressions. It supports various regex syntaxes like ECMAScript (default), basic, extended, awk, grep, and egrep.

2. **`std::smatch` and `std::cmatch`**: These are specializations of `std::match_results` for strings (`std::smatch`) and C-strings (`std::cmatch`). They hold the results of regex operations like matches and sub-matches.

3. **`std::regex_iterator` and `std::regex_token_iterator`**: These iterators are used for iterating through all matches or specific submatches within a target sequence. They make it easy to apply a regex to a sequence and process each match.

4. **`std::regex_replace`**: This function is used for regex-based string replacement. It replaces all matches of a regex within a target string with a replacement string, and it can also use matched subexpressions in the replacement string.

5. **Regex Syntax Options**: The `<regex>` header provides various flags to modify the behavior of regex operations, like `std::regex_constants::icase` for case-insensitive matching, and `std::regex_constants::nosubs` for no subexpression matching.

6. **Error Handling with `std::regex_error`**: This exception is thrown when a regex operation encounters an error, like a syntax error in the regular expression.

Here's an example demonstrating some of these features, particularly `std::regex_iterator` and `std::regex_replace`:

```cpp
#include <iostream>
#include <regex>
#include <string>

int main() {
    std::string text = "The year 2023, the month 11 and the day 28";
    std::regex pattern("\\b\\d{4}\\b");

    // Using regex_iterator to find all year-like numbers
    std::sregex_iterator begin(text.begin(), text.end(), pattern);
    std::sregex_iterator end;
    std::cout << "Found year-like numbers:" << std::endl;
    for (std::sregex_iterator i = begin; i != end; ++i) {
        std::smatch match = *i;
        std::cout << match.str() << std::endl;
    }

    // Replacing all year-like numbers with "YEAR"
    std::string replaced = std::regex_replace(text, pattern, "YEAR");
    std::cout << "After replacement: " << replaced << std::endl;

    return 0;
}
```

In this example, `std::regex_iterator` is used to iterate over all occurrences of a four-digit number in the text, and `std::regex_replace` is used to replace these occurrences with the word "YEAR". The `<regex>` header in C++ is quite powerful and offers robust functionality for working with regular expressions in a variety of contexts.



## std::regex_search and std::regex_match


`std::regex_search` and `std::regex_match` are functions provided by the `<regex>` header in the Standard Library, which are used to perform regular expression operations. Let's break down each of them with examples:

### 1. `std::regex_search`

- **Purpose**: Used to search for a regular expression pattern within a string. It checks if there is any substring in the target string that matches the regular expression pattern.
- **Function Signature**: `bool regex_search(const std::string& s, std::smatch& match, const std::regex& e);`
- **Example**:

    ```cpp
    #include <iostream>
    #include <regex>
    #include <string>

    int main() {
        std::string text = "The quick brown fox jumps over the lazy dog";
        std::regex pattern("quick.*fox");

        std::smatch matches;
        if (std::regex_search(text, matches, pattern)) {
            std::cout << "Found match: " << matches.str(0) << std::endl;
        } else {
            std::cout << "No match found" << std::endl;
        }

        return 0;
    }
    ```

    In this example, `regex_search` looks for any substring that matches the pattern "quick.*fox" in the given text. It would find "quick brown fox" as the matching substring.

### 2. `std::regex_match`

- **Purpose**: Used to check if the entire target string matches a regular expression pattern.
- **Function Signature**: `bool regex_match(const std::string& s, std::smatch& match, const std::regex& e);`
- **Example**:

    ```cpp
    #include <iostream>
    #include <regex>
    #include <string>

    int main() {
        std::string text = "2023-11-28";
        std::regex pattern("\\d{4}-\\d{2}-\\d{2}");

        std::smatch matches;
        if (std::regex_match(text, matches, pattern)) {
            std::cout << "The string is a valid date format" << std::endl;
        } else {
            std::cout << "The string is NOT a valid date format" << std::endl;
        }

        return 0;
    }
    ```

    In this example, `regex_match` checks if the entire string "2023-11-28" matches the date format pattern `\\d{4}-\\d{2}-\\d{2}`. Since it does, the program prints out that the string is a valid date format.

In summary, use `std::regex_search` when you want to find a part of the string that matches the pattern and `std::regex_match` when you need to check if the entire string conforms to a pattern.` and `std::regex_match` are functions provided by the `<regex>` header in the Standard Library, which are used to perform regular expression operations. Let's break down each of them with examples:

### 1. `std::regex_search`

- **Purpose**: Used to search for a regular expression pattern within a string. It checks if there is any substring in the target string that matches the regular expression pattern.
- **Function Signature**: `bool regex_search(const std::string& s, std::smatch& match, const std::regex& e);`
- **Example**:

    ```cpp
    #include <iostream>
    #include <regex>
    #include <string>

    int main() {
        std::string text = "The quick brown fox jumps over the lazy dog";
        std::regex pattern("quick.*fox");

        std::smatch matches;
        if (std::regex_search(text, matches, pattern)) {
            std::cout << "Found match: " << matches.str(0) << std::endl;
        } else {
            std::cout << "No match found" << std::endl;
        }

        return 0;
    }
    ```

    In this example, `regex_search` looks for any substring that matches the pattern "quick.*fox" in the given text. It would find "quick brown fox" as the matching substring.

### 2. `std::regex_match`

- **Purpose**: Used to check if the entire target string matches a regular expression pattern.
- **Function Signature**: `bool regex_match(const std::string& s, std::smatch& match, const std::regex& e);`
- **Example**:

    ```cpp
    #include <iostream>
    #include <regex>
    #include <string>

    int main() {
        std::string text = "2023-11-28";
        std::regex pattern("\\d{4}-\\d{2}-\\d{2}");

        std::smatch matches;
        if (std::regex_match(text, matches, pattern)) {
            std::cout << "The string is a valid date format" << std::endl;
        } else {
            std::cout << "The string is NOT a valid date format" << std::endl;
        }

        return 0;
    }
    ```

In this example, `regex_match` checks if the entire string "2023-11-28" matches the date format pattern `\\d{4}-\\d{2}-\\d{2}`. Since it does, the program prints out that the string is a valid date format.

In summary, use `std::regex_search` when you want to find a part of the string that matches the pattern and `std::regex_match` when you need to check if the entire string conforms to a pattern.





[code](../src/regex_mathch_search.cpp)
