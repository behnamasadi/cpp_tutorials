# Variadic Templates
Variadic templates in C++ are a powerful feature introduced in C++11, allowing functions and classes to accept an arbitrary number of arguments of varying types. Here's a brief guide on how to use them, along with some examples and precautions.

### Understanding Variadic Templates

Variadic templates use a special syntax with an ellipsis (`...`) to denote that they can accept a variable number of arguments. There are two main components to understand:

1. **Function Templates:** Accept any number of arguments.
2. **Class Templates:** Define classes that can work with any number of types.

### Basic Example of Variadic Function Template

```cpp
#include <iostream>

template <typename... Args>
void Print(Args... args) {
    (std::cout << ... << args) << '\n';
}

int main() {
    Print(1, 2, 3, "Hello", 'A');
    return 0;
}
```

This example defines a `Print` function that can take any number of arguments and print them to the console.

### Recursive Variadic Template

Sometimes, unpacking arguments requires a recursive approach:

```cpp
#include <iostream>

template <typename T>
void Print(T arg) {
    std::cout << arg << '\n'; // base case
}

template <typename T, typename... Args>
void Print(T first, Args... args) {
    std::cout << first << ' ';
    Print(args...); // recursive call
}

int main() {
    Print("Hello", 42, 3.14, 'A');
    return 0;
}
```

This `Print` function prints each argument on a new line, demonstrating recursive unpacking.

### Class Template Example

```cpp
template <typename... Args>
class Tuple {};

Tuple<int, double, std::string> myTuple;
```

This is a simple example of a variadic class template. `Tuple` can hold any number of types.

### Things to Be Careful About

1. **Infinite Recursion:** Always have a base case in recursive variadic templates to prevent infinite recursion.
2. **Compilation Time:** Extensive use of variadic templates can increase compilation time.
3. **Debugging Difficulty:** Code using variadic templates can be harder to debug due to complex template instantiations.
4. **Understanding Errors:** Compiler errors can be more complex and harder to decipher.
5. **Parameter Pack Expansion:** Ensure correct expansion of parameter packs, especially in nested template contexts.

### Advanced Usage

You can also use variadic templates for more advanced scenarios like forwarding arguments (`std::forward`), implementing custom tuple-like structures, or writing type-safe variadic functions.


[code](../src/variadic_templates.cpp)
