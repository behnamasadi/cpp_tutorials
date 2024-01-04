# type_traits
`type_traits` in C++ is a part of the Standard Template Library (STL) that provides a set of templates to inspect, transform, and query the properties of types at compile time. These tools are extremely useful for writing generic, type-safe code. Here are some key aspects of `type_traits`:

1. **Type Categories**: These traits allow you to determine whether a type is a particular category of type, such as `std::is_integral`, `std::is_floating_point`, or `std::is_array`.

2. **Type Properties**: These traits check for specific properties of types, like `std::is_const`, `std::is_volatile`, or `std::is_abstract`.

3. **Type Modifiers**: They modify types to produce new types, for example, `std::remove_const`, `std::add_pointer`, or `std::make_signed`.

4. **Type Relationships**: These traits compare two types to check for relationships, like `std::is_same`, `std::is_base_of`, or `std::is_convertible`.

Here are some examples to illustrate these concepts:

1. **Type Category Example**:
   ```cpp
   #include <type_traits>
   #include <iostream>

   int main() {
       std::cout << std::boolalpha;
       std::cout << "Is int an integral type? " << std::is_integral<int>::value << std::endl;
       std::cout << "Is float an integral type? " << std::is_integral<float>::value << std::endl;
       return 0;
   }
   ```

   This example will output `true` for `int` and `false` for `float`, showing how `std::is_integral` can be used to check if a type is an integral type.

2. **Type Property Example**:
   ```cpp
   #include <type_traits>
   #include <iostream>

   int main() {
       std::cout << std::boolalpha;
       std::cout << "Is const int a const type? " << std::is_const<const int>::value << std::endl;
       std::cout << "Is int a const type? " << std::is_const<int>::value << std::endl;
       return 0;
   }
   ```

   This will output `true` for `const int` and `false` for `int`, demonstrating the use of `std::is_const`.

3. **Type Modifier Example**:
   ```cpp
   #include <type_traits>
   #include <iostream>

   int main() {
       std::cout << std::boolalpha;
       std::cout << "Is remove_const<const int> the same as int? " << std::is_same<std::remove_const<const int>::type, int>::value << std::endl;
       return 0;
   }
   ```

   This code checks if removing `const` from `const int` results in `int`, which it does.

4. **Type Relationship Example**:
   ```cpp
   #include <type_traits>
   #include <iostream>

   class Base {};
   class Derived : public Base {};

   int main() {
       std::cout << std::boolalpha;
       std::cout << "Is Derived a subclass of Base? " << std::is_base_of<Base, Derived>::value << std::endl;
       return 0;
   }
   ```

   This will output `true`, indicating that `Derived` is indeed a subclass of `Base`.

These examples demonstrate how `type_traits` can be used to make compile-time decisions based on type information, which is a powerful technique for template metaprogramming in C++.

## A real-world scenario
A real-world scenario where type traits can be extremely useful is in the development of a generic container class. Let's consider a scenario where we're implementing a custom container class, and we want it to behave differently based on whether it's storing a pointer type or a non-pointer type. 

For instance, we might want our container to automatically delete objects it owns when it's storing pointers (to manage memory), but do nothing of the sort for non-pointer types. Here's how you might implement this using `type_traits`:

1. **Generic Container Class**: Define a template class `MyContainer` that can store elements of any type.

2. **Specialization for Pointer Types**: Use `std::is_pointer` from `type_traits` to detect if the stored type is a pointer. If it is, implement a destructor that deletes the pointers to prevent memory leaks.

3. **Fallback for Non-Pointer Types**: For non-pointer types, the destructor does nothing special.

Here's how the code might look:

```cpp
#include <type_traits>
#include <iostream>

template <typename T>
class MyContainer {
public:
    T* data;
    size_t size;

    MyContainer(size_t size) : size(size) {
        data = new T[size];
    }

    ~MyContainer() {
        clear();
    }

    void clear() {
        if constexpr (std::is_pointer<T>::value) {
            for (size_t i = 0; i < size; ++i) {
                delete data[i];
            }
        }
        delete[] data;
    }
};

class ExampleClass {};

int main() {
    // Using MyContainer with a non-pointer type
    MyContainer<int> intContainer(10);

    // Using MyContainer with a pointer type
    MyContainer<ExampleClass*> classContainer(5);
    for (int i = 0; i < 5; ++i) {
        classContainer.data[i] = new ExampleClass();
    }

    // Automatic cleanup for pointers happens here
    return 0;
}
```

In this example:

- `MyContainer` can be used to store any type.
- The destructor and `clear` method check if `T` is a pointer using `std::is_pointer`.
- If `T` is a pointer, `clear` method deletes each object and then the array itself.
- If `T` is not a pointer, `clear` only deletes the array.

This approach ensures that the container is memory safe without imposing unnecessary overhead on non-pointer types. It's a common pattern in generic programming where the behavior needs to be adjusted based on type properties. Type traits make this kind of template metaprogramming possible and efficient in C++.



## std::is_copy_constructible` and `std::is_copy_constructible_v

`std::is_copy_constructible` and `std::is_copy_constructible_v` are related but have slightly different usages in the context of C++ type traits. Here's an explanation of each and how they are used:

### `std::is_copy_constructible`
- `std::is_copy_constructible` is a type trait that is a part of the C++ Standard Library's `<type_traits>` header.
- It is a template that takes a type as its template parameter and evaluates to a `std::integral_constant` (essentially a compile-time constant).
- The `value` member of this `integral_constant` will be `true` if the type is copy constructible, and `false` otherwise.

Example usage:
```cpp
if (std::is_copy_constructible<MyClass>::value) {
    // MyClass is copy constructible
} else {
    // MyClass is not copy constructible
}
```

### `std::is_copy_constructible_v`
- `std::is_copy_constructible_v` is a shorthand (introduced in C++17) for `std::is_copy_constructible<T>::value`.
- It's a template variable, not a type, and directly provides a `bool` value.
- It simplifies the syntax when you just need the boolean result and don't need the full `integral_constant` type.

Example usage:
```cpp
if (std::is_copy_constructible_v<MyClass>) {
    // MyClass is copy constructible
} else {
    // MyClass is not copy constructible
}
```

### Passing an Object Instance
It's important to note that both `std::is_copy_constructible` and `std::is_copy_constructible_v` take a type as a template argument, not an object instance. So, you would pass the type of the object, not the object itself. For example, if you have an object `myObject` of type `MyClass`, you would check `std::is_copy_constructible_v<MyClass>`.

Incorrect usage:
```cpp
// This is incorrect and will not compile
if (std::is_copy_constructible_v<myObject>) {
    // ...
}
```

Correct usage:
```cpp
// This is the correct way to use it
if (std::is_copy_constructible_v<MyClass>) {
    // ...
}
```


[code](../src/type_traits.cpp)
