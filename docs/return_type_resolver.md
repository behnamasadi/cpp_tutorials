# Return Type Resolvers

In C++, a return type resolver is typically used in the context of template metaprogramming and function overloading, where the return type of a function is determined based on the types of its arguments. This is particularly useful in scenarios where the return type cannot be easily determined at the time of writing the function, or where the function needs to be flexible enough to handle different types of input.

Here are some key points about return type resolvers in C++:

1. **Template Metaprogramming**: Return type resolvers often rely on template metaprogramming. Templates in C++ allow functions to operate with generic types. When a function template is instantiated with specific types, the return type resolver can determine the appropriate return type based on these types.

2. **decltype**: C++11 introduced `decltype`, an operator that yields the type of an expression. This can be used in return type resolvers to deduce the return type based on the types of the function's arguments. For instance, in a function that adds two values, `decltype` can be used to determine the return type based on the types of the two operands.

3. **Auto and Trailing Return Types**: C++11 also introduced the `auto` specifier for functions, which can be used in conjunction with trailing return types. This allows the return type to be specified after the parameter list, using `->`. In combination with `decltype`, this provides a powerful way to define functions with return types that depend on their argument types.

4. **std::enable_if**: This template utility in the C++ Standard Library is used for enabling or disabling function overloads based on certain conditions at compile time. This can be combined with return type resolvers to create functions that only exist if certain type conditions are met.

5. **SFINAE (Substitution Failure Is Not An Error)**: This is a principle in C++ template metaprogramming where the failure to instantiate a template does not immediately result in a compilation error. Instead, the compiler simply discards that instantiation as a candidate in overload resolution. This principle is often used in conjunction with return type resolvers to create different function overloads for different types.

6. **Type Traits**: The C++ Standard Library provides type traits in the header `<type_traits>` which can be used to make compile-time decisions about types. These are often used in conjunction with return type resolvers to tailor a function's behavior based on type characteristics.

A simple example of a return type resolver might look something like this:

```cpp
template <typename T, typename U>
auto add(T t, U u) -> decltype(t + u) {
    return t + u;
}
```

In this example, the return type of `add` is resolved based on the types of `t` and `u`. If `t` and `u` are, say, an `int` and a `float`, the return type would be `float`, as that's the result of adding an `int` and a `float` together.

`std::enable_if` is a template metaprogramming utility in C++ that can be used to enable or disable a function template based on a compile-time condition. It is a part of the C++ Standard Library's `<type_traits>` header and is commonly used in the context of SFINAE (Substitution Failure Is Not An Error).

The basic idea is that `std::enable_if` can conditionally enable a function template when a particular trait or condition is true. If the condition is false, the template is not considered a valid candidate for specialization, which effectively removes it from the set of overloads the compiler considers during compilation.

Here's a basic example to illustrate how `std::enable_if` can be used:

### Example: Using `std::enable_if` for Function Overloading

Suppose you want to create a function template `print` that behaves differently based on whether the input type is an integer or a floating-point number.

1. **Print for Integer Types**

   ```cpp
   template <typename T>
   typename std::enable_if<std::is_integral<T>::value, void>::type 
   print(T value) {
       std::cout << "Integer: " << value << std::endl;
   }
   ```

   In this function, `std::enable_if` is used with `std::is_integral<T>::value` as the condition. This means the function will only be enabled if `T` is an integral type (e.g., `int`, `long`).

2. **Print for Floating-Point Types**

   ```cpp
   template <typename T>
   typename std::enable_if<std::is_floating_point<T>::value, void>::type 
   print(T value) {
       std::cout << "Floating point: " << value << std::endl;
   }
   ```

   Here, the function is enabled only if `T` is a floating-point type (e.g., `float`, `double`).

### How It Works

- When you call `print` with an integer, the compiler selects the first version because `std::is_integral` will be `true` for integral types. The second version is effectively removed from consideration because `std::is_floating_point` will be `false`.
  
- Conversely, when you call `print` with a floating-point number, the second version is selected, and the first one is ignored.

### Example Usage

```cpp
print(42);    // Calls the integer version
print(3.14);  // Calls the floating-point version
```

### Modern C++ Variants

In modern C++ (C++14 and later), you can use `std::enable_if_t` for a more concise syntax. This is a type alias template that simplifies the syntax by removing the need to write `typename` and `::type`. The above functions can be rewritten as:

```cpp
template <typename T>
std::enable_if_t<std::is_integral<T>::value, void> 
print(T value) {
    // ... implementation ...
}

template <typename T>
std::enable_if_t<std::is_floating_point<T>::value, void> 
print(T value) {
    // ... implementation ...
}
```

### Conclusion

`std::enable_if` is a powerful tool for creating conditionally enabled function templates. It allows for more precise control over template instantiation and is a key technique in advanced C++ template metaprogramming.

[code](../src/return_type_resolver.cpp)
