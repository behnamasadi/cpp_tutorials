# SFINAE
SFINAE, which stands for "Substitution Failure Is Not An Error," is a concept in C++ template programming. It's a rule that applies during the template instantiation process. The idea is that if a substitution of template parameters into a template results in an invalid code, this is not in itself an error. Instead, the invalid template is simply discarded from the set of potential templates to use.

This concept is particularly useful for creating template specializations that are only valid for certain types or conditions, enabling more flexible and powerful template designs.

### Basic Example of SFINAE

Here's a simple example to illustrate SFINAE:

```cpp
#include <iostream>
#include <type_traits>

// Base template
template <typename T>
typename std::enable_if<std::is_integral<T>::value, bool>::type isIntegral(T) {
    return true;
}

// Template specialization for non-integral types
template <typename T>
typename std::enable_if<!std::is_integral<T>::value, bool>::type isIntegral(T) {
    return false;
}

int main() {
    std::cout << std::boolalpha;
    std::cout << "isIntegral(10): " << isIntegral(10) << std::endl;   // Outputs: true
    std::cout << "isIntegral(3.14): " << isIntegral(3.14) << std::endl; // Outputs: false

    return 0;
}
```

In this example, there are two versions of the `isIntegral` function template. The first version is enabled (via `std::enable_if`) only for integral types, while the second version is enabled for non-integral types. When you call `isIntegral` with an integer, the first version is instantiated. If you call it with a non-integer, the second version is instantiated.

### Explanation

- `std::enable_if`: This is a standard library utility that provides a member typedef `type` if the given boolean constant is true. If the boolean is false, there's no member typedef. This utility is commonly used for controlling template instantiation.

- `std::is_integral`: This is a type trait that checks if a type is an integral type (like int, char, etc.).

### How SFINAE Comes Into Play

In the example, if `T` is an integral type, the `std::enable_if<std::is_integral<T>::value, bool>::type` becomes `bool`, so the first `isIntegral` function template is valid. If `T` is not an integral type, this substitution fails, but due to SFINAE, this failure is not an error; instead, the first `isIntegral` template is simply discarded, and the compiler looks for other templates (in this case, the second `isIntegral`).

SFINAE allows for powerful metaprogramming techniques in C++, enabling the creation of templates that can adapt to different types and conditions at compile time, leading to more efficient and tailored code.
