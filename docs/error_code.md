## std::error_code
`std::error_code` in C++ is a part of the `<system_error>` header and represents an error condition in a program, typically used to report and handle error situations in a more flexible and extendable way compared to traditional methods like using integer error codes or exceptions.
- It combines error codes with an error category, making it possible to understand and manage errors across different domains and libraries.
- It is particularly useful in performance-critical code or when working in environments where exceptions are not suitable.

### Key Concepts:

1. **Error Code and Error Category**:
   - `std::error_code` consists of two parts: an error value (an integer) and an error category (`std::error_category`).
   - The error value represents the specific error.
   - The error category represents the domain or context in which the error value is meaningful, such as system errors, application-specific errors, etc.

2. **Error Handling**:
   - `std::error_code` provides a way to handle errors without throwing exceptions. This is particularly useful in contexts where exceptions are either not allowed or not preferred, such as in real-time systems or performance-critical code.

3. **Comparison**:
   - You can compare `std::error_code` objects directly to check if two error codes represent the same error.

4. **Conversion to `std::error_condition`**:
   - `std::error_code` can be converted to a more generic `std::error_condition`, which represents a portable error condition, independent of the platform-specific error codes.

### Example:

Let's walk through an example to understand how `std::error_code` can be used in practice.

#### Example 1: Basic Usage

```cpp
#include <iostream>
#include <system_error>
#include <fstream>

void open_file(const std::string& filename, std::error_code& ec) {
    std::ifstream file(filename);
    if (!file) {
        ec = std::make_error_code(std::errc::no_such_file_or_directory);
        return;
    }
    // Process file...
    ec.clear();  // No error
}

int main() {
    std::error_code ec;
    open_file("non_existent_file.txt", ec);
    
    if (ec) {
        std::cout << "Error opening file: " << ec.message() << " (Error code: " << ec.value() << ")\n";
    } else {
        std::cout << "File opened successfully.\n";
    }

    return 0;
}
```

#### Explanation:

1. **`std::errc::no_such_file_or_directory`**:
   - This is an enumerator from the `std::errc` enumeration, which contains platform-independent error codes.

2. **`std::make_error_code`**:
   - This function converts the `std::errc` value to a `std::error_code`.

3. **Error Message**:
   - `ec.message()` returns a human-readable error message corresponding to the error code.
   - `ec.value()` returns the actual error value.

In this example, if the file `"non_existent_file.txt"` does not exist, the function `open_file` sets the `std::error_code` to indicate that the file was not found, and the error is reported in `main`.

#### Example 2: Using `std::system_category`

`std::system_category()` represents the operating system's error codes, allowing you to work directly with native error codes.

```cpp
#include <iostream>
#include <system_error>
#include <cerrno>
#include <cstdio>

void delete_file(const std::string& filename, std::error_code& ec) {
    if (std::remove(filename.c_str()) != 0) {
        ec = std::error_code(errno, std::system_category());
    } else {
        ec.clear();  // No error
    }
}

int main() {
    std::error_code ec;
    delete_file("non_existent_file.txt", ec);
    
    if (ec) {
        std::cout << "Error deleting file: " << ec.message() << " (Error code: " << ec.value() << ")\n";
    } else {
        std::cout << "File deleted successfully.\n";
    }

    return 0;
}
```

#### Explanation:

1. **`errno`**:
   - This global variable is set by certain system calls and library functions in the C standard library when an error occurs.

2. **`std::system_category()`**:
   - It is used here to associate the error code with the system's error category.

In this example, if the file `"non_existent_file.txt"` does not exist, `std::remove` sets `errno` to `ENOENT`, and we set the `std::error_code` accordingly. This code is then used to report the error.


[code](../src/error_code.cpp)
