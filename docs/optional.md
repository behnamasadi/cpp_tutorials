# std::optional
`std::optional`, introduced in C++17, is a template class that provides a way to represent 'optional' values. An `std::optional` object can either contain a value or not. This is particularly useful in situations where a value might be missing or not applicable.

Here's a breakdown of how `std::optional` is used, with various examples:

### Basic Usage

1. **Initialization and Assignment**
   ```cpp
   #include <optional>
   
   std::optional<int> opt; // An empty optional
   opt = 5; // Assigning a value
   ```

2. **Accessing the Value**
   ```cpp
   if (opt) { // Check if there's a value
       std::cout << *opt << std::endl; // Access the value
   }
   ```

3. **Using `value()` and `value_or()`**
   ```cpp
   std::cout << opt.value() << std::endl; // Throws std::bad_optional_access if empty
   std::cout << opt.value_or(0) << std::endl; // Returns 0 if opt is empty
   ```

### Advanced Usage

1. **With Custom Types**
   ```cpp
   struct MyStruct {
       int x;
   };

   std::optional<MyStruct> optStruct;
   optStruct.emplace(MyStruct{10}); // Constructing in-place
   ```

2. **Returning `std::optional` from Functions**
   ```cpp
   std::optional<int> findValue(bool condition) {
       if (condition)
           return 42; // Found value
       return {}; // Return empty optional
   }
   ```

3. **Chaining Optionals**
   ```cpp
   std::optional<int> opt1 = 10;
   std::optional<int> opt2 = opt1.value_or(0) + 5; // Chaining with addition
   ```

4. **Using with STL Containers**
   ```cpp
   std::vector<std::optional<int>> vec = {1, std::nullopt, 3};
   for (const auto& val : vec) {
       if (val) {
           std::cout << *val << " ";
       }
   }
   ```

5. **Resetting an Optional**
   ```cpp
   opt.reset(); // Removes the contained value, if any
   ```

### Practical Scenarios

- **Error Handling**: Use `std::optional` to represent the result of a function that might fail to produce a value, instead of using sentinel values or exceptions.
- **Lazy Initialization**: Delay the construction of an object until it's actually needed.
- **APIs with Optional Parameters**: Functions can return `std::optional` to indicate optional parameters or results.

`std::optional` is a powerful feature that makes your code more expressive and safe by explicitly handling the presence or absence of values.

[code](../src/optional.cpp)

