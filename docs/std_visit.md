## std::visit

`std::visit` is a powerful utility in C++ that is primarily used to work with `std::variant`, a type-safe union introduced in C++17. A `std::variant` can hold a value from a set of specified types, but only one at a time. The challenge with `std::variant` is accessing the value it holds safely, without resorting to unsafe type casts or multiple `std::get` calls. This is where `std::visit` comes into play.

### Why Do We Need `std::visit`?

`std::variant` encapsulates multiple types, and to handle the value it holds, we need a way to determine its current type and execute the corresponding logic. Using `std::visit`, we can apply a visitor (a callable object like a lambda, function, or functor) to the currently held value in the `std::variant`. This provides a clean, type-safe way to access the variant's value without manually checking the active type.

Without `std::visit`, we'd have to manually inspect the `std::variant`'s active type using `std::holds_alternative<T>()` or by trying multiple `std::get<T>()` calls, which can lead to code that's difficult to maintain and prone to errors.

### What Does `std::visit` Do?

`std::visit` applies a callable (often referred to as a visitor) to the value stored in a `std::variant`. The key benefit is that the visitor is automatically invoked with the correct type, eliminating the need for explicit type checking and handling.

**Key points about `std::visit`:**
- It ensures that the visitor is called with the correct type from the `std::variant`.
- The visitor can be a lambda function, a regular function, or a functor.
- If the `std::variant` can hold multiple types, `std::visit` allows you to handle each type case in a unified, readable manner.
- It can be used with multiple `std::variant` arguments simultaneously, where the visitor will handle combinations of the types.

### Syntax of `std::visit`

The basic syntax is as follows:

```cpp
#include <variant>
#include <iostream>

std::variant<int, float, std::string> myVariant = 42;

auto visitor = [](auto&& arg) {
    std::cout << arg << std::endl;
};

std::visit(visitor, myVariant);
```

Here's a breakdown of the syntax:
- `std::visit(visitor, variant)` is how you call `std::visit`.
- `visitor` is the callable object that you want to apply to the variant's currently held value.
- `variant` is the `std::variant` you want to inspect.

### Example: Using `std::visit`

Let's look at a more detailed example:

```cpp
#include <iostream>
#include <variant>
#include <string>

int main() {
    std::variant<int, float, std::string> myVariant = 10;

    auto visitor = [](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;  // Get the type of the argument
        if constexpr (std::is_same_v<T, int>)
            std::cout << "It's an int: " << arg << std::endl;
        else if constexpr (std::is_same_v<T, float>)
            std::cout << "It's a float: " << arg << std::endl;
        else if constexpr (std::is_same_v<T, std::string>)
            std::cout << "It's a string: " << arg << std::endl;
    };

    std::visit(visitor, myVariant);  // Output: It's an int: 10

    myVariant = "Hello";
    std::visit(visitor, myVariant);  // Output: It's a string: Hello

    myVariant = 3.14f;
    std::visit(visitor, myVariant);  // Output: It's a float: 3.14

    return 0;
}
```

### Explanation:
- **Variant Initialization**: `myVariant` is a `std::variant` that can hold an `int`, `float`, or `std::string`.
- **Visitor**: The visitor is a lambda function that checks the type of the argument using `if constexpr` and `std::is_same_v`, and prints out a message based on the type.
- **`std::visit` Call**: `std::visit` applies the visitor to the current value of `myVariant`. Depending on what type is stored in `myVariant`, the corresponding branch of the lambda is executed.

### What `std::visit` Does That Can't Be Done Easily Without It

Without `std::visit`, you would have to manually manage type handling in `std::variant`, which can lead to verbose and error-prone code. For example:

```cpp
#include <variant>
#include <iostream>
#include <string>

int main() {
    std::variant<int, float, std::string> myVariant = 42;

    if (std::holds_alternative<int>(myVariant)) {
        std::cout << "It's an int: " << std::get<int>(myVariant) << std::endl;
    } else if (std::holds_alternative<float>(myVariant)) {
        std::cout << "It's a float: " << std::get<float>(myVariant) << std::endl;
    } else if (std::holds_alternative<std::string>(myVariant)) {
        std::cout << "It's a string: " << std::get<std::string>(myVariant) << std::endl;
    }

    return 0;
}
```

This approach requires separate type checks (`std::holds_alternative<T>`) and then fetching the value (`std::get<T>`), making the code less elegant and harder to extend or maintain.

[code](../src/std_visit.cpp)

