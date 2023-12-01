# Structured binding declarations

Structured binding declarations, introduced in C++17, provide a convenient way to unpack elements from a tuple, pair, array, or a struct into separate variables. This feature enhances code readability and simplifies the process of working with compound data types.

Here's a basic overview of how structured binding declarations work:

### Syntax:

```cpp
auto [var1, var2, ..., varN] = expression;
```

- `var1, var2, ..., varN` are the variables you declare.
- `expression` is a tuple-like object (e.g., a `std::tuple`, `std::pair`, array, or a struct with public data members).

### Example Usage:

1. **With a Tuple:**
   ```cpp
   std::tuple<int, double, std::string> myTuple = {1, 2.3, "example"};
   auto [x, y, z] = myTuple;
   ```
   Here, `x`, `y`, and `z` are initialized with the corresponding elements of `myTuple`.

2. **With a Pair:**
   ```cpp
   std::pair<int, std::string> myPair = {1, "hello"};
   auto [a, b] = myPair;
   ```
   `a` and `b` are initialized with the elements of `myPair`.

3. **With an Array:**
   ```cpp
   int myArray[] = {1, 2, 3};
   auto [first, second, third] = myArray;
   ```
   `first`, `second`, and `third` correspond to the elements of `myArray`.

4. **With a Struct:**
   ```cpp
   struct MyStruct { int id; double value; };
   MyStruct s = {1, 3.14};
   auto [id, value] = s;
   ```
   `id` and `value` correspond to the members of `s`.

### Key Points:

- Structured bindings create local variables that are either copies of, references to, or proxies for the unpacked elements.
- They help to avoid the verbose syntax of accessing tuple or struct elements and make code more readable.
- Structured bindings work with any type that has a non-explicit `tuple_size` and `tuple_element` specialization, making them versatile.
- They can also be used with `const` and references, e.g., `const auto& [x, y] = myPair;`.

Structured bindings simplify the process of working with grouped data and are a significant enhancement in modern C++ for both readability and convenience.


[code](../src/structured_binding_declaration.cpp)
