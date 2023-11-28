### Template Specialization

In C++, template specialization allows you to define a specific implementation of a template for a particular data type or a set of data types.

#### Primary Template
First, you define a general template, known as the primary template, that works with generic types. For example:

```cpp
template <typename T>
void process(T data) {
    // generic processing for any type
}
```

#### Specialization
Then, you can define specializations for specific types. This is like saying, "Use this specific implementation when the template is instantiated with this particular type."

```cpp
template <>
void process<int>(int data) {
    // specialized processing for int
}

template <>
void process<std::string>(std::string data) {
    // specialized processing for std::string
}
```

When you call `process` with an `int` or a `std::string`, C++ uses the specialized versions. For other types, it falls back to the primary template.

### Tag Dispatch

Tag dispatch is a technique used to select different function overloads based on the type traits of the parameters at compile time, without incurring runtime overhead.

#### Tags
You define empty structures (tags) that represent different categories or traits.

```cpp
struct tag1 {};  // Represents one category
struct tag2 {};  // Represents another category
```

#### Functions with Tags
You then write functions that take these tags as parameters, along with other arguments. The choice of which function to call is based on these tag parameters.

```cpp
template <typename T>
void function(T data, tag1) {
    // Implementation for category represented by tag1
}

template <typename T>
void function(T data, tag2) {
    // Implementation for category represented by tag2
}
```

#### Dispatching
Finally, you use traits or other compile-time checks to choose the right tag, and thereby the right function overload, at the call site.

```cpp
template <typename T>
void process(T data) {
    if constexpr (std::is_integral<T>::value) {
        function(data, tag1());
    } else {
        function(data, tag2());
    }
}

process(10);  // Calls version with tag1
process("hello");  // Calls version with tag2
```

In this example, `process` uses `constexpr if` (a C++17 feature) to choose the tag based on whether `T` is an integral type.


[code](../src/template_specialization_tag_dispatch.cpp)
