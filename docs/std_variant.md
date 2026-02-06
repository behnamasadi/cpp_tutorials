### Explanation of `std::variant`

`std::variant` is a type-safe union available in C++17 and later. It allows you to store a value that could be one of several different types, but only one type at a time. Unlike traditional unions in C++, `std::variant` provides type safety by ensuring that only one type is active at any given time and by providing ways to query and retrieve the currently active type.

### Simple Example of `std::variant`

Let's say you want to store either an `int` or a `std::string` in a single variable. Using `std::variant`, you can define such a variable and safely use it.

Here’s a simple example:

```cpp
#include <iostream>
#include <variant>
#include <string>

int main() {
    // Define a variant that can hold either an int or a std::string
    std::variant<int, std::string> myVariant;

    // Assign an int to the variant
    myVariant = 42;
    
    // Check the type and print the value
    if (std::holds_alternative<int>(myVariant)) {
        std::cout << "Integer: " << std::get<int>(myVariant) << std::endl;
    }

    // Assign a string to the variant
    myVariant = std::string("Hello, Variant!");

    // Check the type and print the value
    if (std::holds_alternative<std::string>(myVariant)) {
        std::cout << "String: " << std::get<std::string>(myVariant) << std::endl;
    }

    return 0;
}
```

### Explanation of the Example

1. **Declaration**:
   ```cpp
   std::variant<int, std::string> myVariant;
   ```
   This declares a variable `myVariant` that can hold either an `int` or a `std::string`.

2. **Assigning an `int`**:
   ```cpp
   myVariant = 42;
   ```
   Here, the variant is assigned an integer value `42`.

3. **Checking the type and getting the value**:
   ```cpp
   if (std::holds_alternative<int>(myVariant)) {
       std::cout << "Integer: " << std::get<int>(myVariant) << std::endl;
   }
   ```
   This checks if the variant currently holds an `int` using `std::holds_alternative<int>`. If true, it retrieves the value using `std::get<int>`.

4. **Assigning a `std::string`**:
   ```cpp
   myVariant = std::string("Hello, Variant!");
   ```
   The variant is now assigned a string value.

5. **Checking the type and getting the value (string)**:
   ```cpp
   if (std::holds_alternative<std::string>(myVariant)) {
       std::cout << "String: " << std::get<std::string>(myVariant) << std::endl;
   }
   ```
   Similarly, this checks if the variant holds a `std::string` and retrieves the value.


### Real-World Example 1: Handling Different Types of Configuration Values

Consider a configuration system where values can be of different types, such as integers, floating-point numbers, strings, or boolean values. Using `std::variant` allows you to store these different types in a single container.

```cpp
#include <iostream>
#include <variant>
#include <string>
#include <unordered_map>

using ConfigValue = std::variant<int, double, std::string, bool>;

void printConfigValue(const ConfigValue& value) {
    std::visit([](auto&& arg) { std::cout << arg << std::endl; }, value);
}

int main() {
    std::unordered_map<std::string, ConfigValue> config;

    config["max_connections"] = 100;
    config["timeout"] = 30.5;
    config["username"] = std::string("admin");
    config["debug_mode"] = true;

    for (const auto& [key, value] : config) {
        std::cout << key << ": ";
        printConfigValue(value);
    }

    return 0;
}
```

### Explanation

- **Using `std::variant` for Configuration Values**:
  - The `ConfigValue` type is defined as a `std::variant` that can hold an `int`, `double`, `std::string`, or `bool`.
  - The configuration values are stored in a `std::unordered_map` where each key maps to a `ConfigValue`.

- **Printing Configuration Values**:
  - The `printConfigValue` function uses `std::visit` to handle each possible type in the variant and print its value.

### Real-World Example 2: Representing Different Types of Shapes

In a graphics or CAD application, you might have different types of shapes (e.g., circles, rectangles, triangles) but want to store them in a single container for processing.

```cpp
#include <iostream>
#include <variant>
#include <vector>

struct Circle {
    double radius;
};

struct Rectangle {
    double width, height;
};

struct Triangle {
    double base, height;
};

using Shape = std::variant<Circle, Rectangle, Triangle>;

void printShape(const Shape& shape) {
    std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, Circle>) {
            std::cout << "Circle with radius: " << arg.radius << std::endl;
        } else if constexpr (std::is_same_v<T, Rectangle>) {
            std::cout << "Rectangle with width: " << arg.width << " and height: " << arg.height << std::endl;
        } else if constexpr (std::is_same_v<T, Triangle>) {
            std::cout << "Triangle with base: " << arg.base << " and height: " << arg.height << std::endl;
        }
    }, shape);
}

int main() {
    std::vector<Shape> shapes = {
        Circle{5.0},
        Rectangle{3.0, 4.0},
        Triangle{6.0, 2.5}
    };

    for (const auto& shape : shapes) {
        printShape(shape);
    }

    return 0;
}
```

### Explanation

- **Using `std::variant` for Shapes**:
  - The `Shape` type is defined as a `std::variant` that can hold a `Circle`, `Rectangle`, or `Triangle`.

- **Printing Shape Information**:
  - The `printShape` function uses `std::visit` with a lambda to check the active type in the variant and print the corresponding shape information.

### Benefits of `std::variant`

- **Type Safety**: Ensures that only one type is active and that the operations are valid for that type.
- **Simplified Code**: Eliminates the need for manual type management and type checks.
- **Extensibility**: Easily add more types to the variant without changing the existing logic significantly.
- **Ease of Use**: Functions like `std::visit` provide a clean way to handle different types without repetitive code.

In summary, `std::variant` is a powerful tool for managing variables that can hold multiple types, ensuring type safety and making the code easier to maintain and extend.



`std::variant` is particularly useful in scenarios where a variable or a function needs to work with multiple types of data, but only one type at a time. It ensures type safety and eliminates the need for manual type management that comes with traditional unions or `void*` pointers. Here are some real-world examples where `std::variant` proves beneficial:




## Real-world API design examples

### 1. Lookup where “not found” is normal → `std::optional<T>`

**Example: map/cache lookup**

```cpp
std::optional<User> find_user(UserId id) const;
```

Why: absence is a *valid outcome*, not necessarily an error. You usually don’t need a reason beyond “missing”.

Variant: avoid copies

```cpp
std::optional<std::reference_wrapper<const User>>
find_user_ref(UserId id) const;
```

### 2. Parsing where failure reason matters → `std::expected<T, E>`

**Example: parsing config**

```cpp
enum class ParseErr { Empty, InvalidNumber, OutOfRange };

std::expected<int, ParseErr> parse_port(std::string_view s);
```

Why: the caller often wants to report *why* it failed (error message, user feedback, logs), or recover differently.

### 3. File/network/database operations → `std::expected<T, E>` (or exceptions in exception-friendly APIs)

**Example: file read**

```cpp
struct IoErr {
  std::error_code ec;
  std::string path;
};

std::expected<std::string, IoErr> read_text_file(std::string path);
```

Why: failures are common and actionable (permissions, missing file, short read).

### 4. Operations where failure is exceptional/unrecoverable in your domain → exceptions (sometimes)

If your library’s style is exception-based and failures are truly exceptional:

```cpp
std::string read_text_file_or_throw(const std::string& path); // throws
```

But for systems/robotics/infra-style code, `expected` is often preferred because you can propagate errors cheaply and explicitly.

### 5. “Try” APIs with out parameters → replace with `optional` or `expected`

❌

```cpp
bool try_parse(std::string_view s, int& out);
```

✅ optional (only success/fail)

```cpp
std::optional<int> parse(std::string_view s);
```

✅ expected (success/fail + reason)

```cpp
std::expected<int, ParseErr> parse(std::string_view s);
```

---

## `std::optional<T>` vs `std::expected<T, E>` (decision rules)

### Use `optional` when:

* “No value” is a **normal** and **fully sufficient** outcome
* There’s no meaningful error to report
* Callers don’t need branching on error type
* You want the simplest API surface

Common examples:

* `find_*`
* `maybe_get_*`
* best-effort queries (“give me cached value if any”)

### Use `expected` when:

* Failure is common and you want **structured error info**
* You need to differentiate errors to decide recovery
* You want to propagate errors without exceptions
* You want call sites to be explicit without ad-hoc logging

Common examples:

* parsing/validation with user feedback
* IO, network, database
* decoding/serialization
* resource creation (device init, driver open)

---

## “Absence vs error” clarity (the key design point)

If “missing” can happen for multiple reasons and the reason matters:

* `optional` collapses everything into “no”
* `expected` keeps meaning

Example: config read

**Bad with optional**

```cpp
std::optional<std::string> read_setting(std::string_view key);
```

Is it missing? permission denied? corrupted file? parse failed? caller can’t know.

**Good with expected**

```cpp
enum class SettingsErr { MissingKey, IoFailed, ParseFailed };

std::expected<std::string, SettingsErr>
read_setting(std::string_view key);
```

---

## Composability patterns (production-friendly)

### Pattern A: Parse → validate → build (with `expected`)

```cpp
enum class Err { Parse, Invalid };

std::expected<int, Err> parse_int(std::string_view);
std::expected<int, Err> validate_positive(int v);

std::expected<int, Err> parse_positive_int(std::string_view s) {
  auto v = parse_int(s);
  if (!v) return std::unexpected(Err::Parse);
  return validate_positive(*v);
}
```

### Pattern B: optional-to-expected bridge (common in APIs)

If you have an optional lookup but want structured errors at a higher layer:

```cpp
enum class Err { NotFound };

std::expected<User, Err> require_user(UserId id) {
  if (auto u = find_user(id)) return *u;
  return std::unexpected(Err::NotFound);
}
```

---

## Choosing error type `E` in `expected<T, E>`

Good options:

* `std::error_code` (interoperable with system errors)
* small `enum class` (fast, stable, easy switching)
* a struct holding `enum + context` (path, id, message, underlying code)

Example:

```cpp
enum class Errc { NotFound, Permission, Corrupt };

struct Err {
  Errc code;
  std::string context;      // optional extra info
  std::error_code sys = {}; // if relevant
};
```

---

## Practical guideline for public library APIs

* Use `optional` for “missing is normal” queries.
* Use `expected` for operations that can fail in meaningful ways.
* Avoid mixing exceptions + expected in the same layer unless you have a clear boundary:

  * low-level: `expected`
  * high-level convenience wrappers: `throw` by converting `expected` to exception

---

If you tell me one concrete API you’re designing (e.g., “parse YAML config”, “load frame from video”, “get camera intrinsic from DB”), I’ll sketch 2–3 clean signatures and show how callers would use them.

