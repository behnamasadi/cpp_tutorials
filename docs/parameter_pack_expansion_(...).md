# Parameter pack

Parameter pack expansion in C++ is a feature used in template programming, particularly with variadic templates, where you can work with an arbitrary number of template parameters. It allows functions and classes to accept and process any number of template arguments, making your code more flexible and general.

Here's a basic rundown of how parameter pack expansion works:

1. **Variadic Templates**: Introduced in C++11, these templates allow you to write functions and classes that can take a variable number of template arguments. This is achieved using an ellipsis (`...`) syntax.

2. **Parameter Pack**: A template parameter pack is a set of zero or more template parameters. Similarly, a function parameter pack is a set of zero or more function parameters.

3. **Expansion**: Parameter pack expansion occurs when you use the pack in a context where multiple elements are expected, with the ellipsis (`...`) indicating where and how the expansion should take place.

For example, consider a simple function template that prints all its arguments:

```cpp
template<typename... Args>
void print(Args... args) {
    (std::cout << ... << args) << '\n';
}
```

In this function, `Args` is a template parameter pack representing a variable number of arguments, and `args` is a function parameter pack. The line `(std::cout << ... << args)` is where the expansion occurs. It expands to a series of insertions into `std::cout` for each element in the `args` pack.

If you call `print(1, "string", 3.14)`, the expansion results in something equivalent to `std::cout << 1 << "string" << 3.14`.


## Examples
Let's explore some practical, real-world scenarios where parameter pack expansion in C++ variadic templates can be highly useful:

### 1. **Custom Logger**

Imagine you're building a software application and need a flexible logger that can accept various types of data:

```cpp
template<typename... Args>
void logMessage(Args... args) {
    (std::cout << ... << args) << std::endl;
}

// Usage
logMessage("User ", userId, " logged in at ", timestamp);
```

This function can take any number of arguments of different types and log them to the console.

### 2. **Summing Different Numbers**

A function that can sum an arbitrary number of numbers, regardless of their type (int, double, etc.), can be quite useful:

```cpp
template<typename... Args>
auto sum(Args... args) -> decltype((args + ...)) {
    return (args + ...);
}

// Usage
auto total = sum(1, 2, 3, 4.5);  // Can mix integers and floating-point numbers
```

### 3. **Generic Tuple Printer**

A function to print the contents of a tuple, regardless of the number and types of its elements:

```cpp
#include <tuple>

template<typename Tuple, std::size_t... Is>
void printTupleImpl(const Tuple &t, std::index_sequence<Is...>) {
    ((std::cout << (Is == 0 ? "" : ", ") << std::get<Is>(t)), ...);
}

template<typename... Args>
void printTuple(const std::tuple<Args...>& t) {
    printTupleImpl(t, std::index_sequence_for<Args...>{});
    std::cout << std::endl;
}

// Usage
auto t = std::make_tuple(10, "Hello", 3.14);
printTuple(t);
```

### 4. **Type-Safe printf-like Function**

Creating a type-safe version of `printf` that accepts any number of arguments of various types:

```cpp
template<typename... Args>
void printFormatted(const std::string& format, Args... args) {
    // Implementation would involve parsing the format string and matching
    // placeholders to the types of the arguments in 'args...'
    // For simplicity, let's assume it just prints the arguments.
    (std::cout << ... << args) << std::endl;
}

// Usage
printFormatted("Name: %s, Age: %d", "Alice", 30);
```

### 5. **Applying a Function to Multiple Arguments**

A function that applies another function to multiple arguments:

```cpp
template<typename Func, typename... Args>
void applyToAll(Func f, Args... args) {
    (f(args), ...);
}

// Usage
applyToAll([](int x){ std::cout << x*x << " "; }, 1, 2, 3, 4);
```

### 6. **Constructing Objects from Multiple Arguments**

Building objects from a variable number of arguments, like in a factory pattern:

```cpp
template<typename T, typename... Args>
T* createObject(Args... args) {
    return new T(args...);
}

// Usage
class MyClass {
public:
    MyClass(int, std::string) {}
    // ...
};

auto obj = createObject<MyClass>(42, "Example");
```

In all these scenarios, variadic templates and parameter pack expansion provide a flexible, type-safe way to handle multiple arguments of varying types and numbers, which is a cornerstone of modern C++ template metaprogramming.


Refs: [1](https://en.cppreference.com/w/cpp/language/parameter_pack)
