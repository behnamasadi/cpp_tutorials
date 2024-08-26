# std::any
`std::any` is a feature introduced in C++17. It provides a type-safe container for single values of any type.

## Explanation
- `std::any` can store an instance of any type that satisfies `CopyConstructible`.
- It's a better, type-safe alternative to `void*` for storing values of unknown type at compile time.
- `std::any_cast` is a function template in C++ that is used to retrieve the value stored in an `std::any` object. It serves two primary purposes: to safely extract a value of a specific type from an `std::any` object and to check the type of the stored value at runtime.
- If the cast to the original type fails (i.e., if you try to cast it to a different type), an exception of type `std::bad_any_cast` is thrown.

## Example

```cpp
#include <iostream>
#include <any>
#include <string>

int main() {
    std::any a = 1; // Storing an int
    std::cout << std::any_cast<int>(a) << '\n'; // Casting back to int

    a = 3.14; // Now storing a double
    std::cout << std::any_cast<double>(a) << '\n'; // Casting back to double

    a = std::string("Hello, std::any!");
    std::cout << std::any_cast<std::string>(a) << '\n'; // Casting back to std::string

    // Attempting to cast to an incorrect type will throw std::bad_any_cast
    try {
        std::cout << std::any_cast<int>(a) << '\n';
    } catch (const std::bad_any_cast& e) {
        std::cout << e.what() << '\n';
    }
}
```

In this example:
- `std::any a` is first assigned an integer, then a double, and finally a `std::string`.
- For each assignment, it's cast back to its original type using `std::any_cast`.
- The last block demonstrates the exception handling in case of a wrong cast. When we try to cast the `std::string` to `int`, `std::bad_any_cast` is thrown.

## std::any with and without satisfying the CopyConstructible condition

To illustrate a non-`CopyConstructible` type, we can create a class that explicitly deletes its copy constructor. This class can't be used with `std::any` directly, as it violates the `CopyConstructible` requirement.

```cpp
#include <iostream>
#include <any>
#include <string>

class NonCopyable {
public:
    NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete; // deleting the copy constructor
    NonCopyable& operator=(const NonCopyable&) = delete;
};

int main() {
    NonCopyable nonCopyableObj;

    // This line will cause a compilation error because NonCopyable is not CopyConstructible
    // std::any a = nonCopyableObj;
    
    // Uncommenting the above line will lead to a compilation error
    // Instead, you can store a pointer to the object
    std::any a = &nonCopyableObj; // Storing a pointer is fine

    // Retrieving the pointer back
    auto storedObj = std::any_cast<NonCopyable*>(a);
    // Use storedObj as a pointer to NonCopyable
}
```

In the second example, attempting to store an instance of `NonCopyable` in `std::any` will result in a compilation error. However, storing a pointer to a `NonCopyable` object works because pointers are `CopyConstructible`.


## How to find out if a class is CopyConstructible 

### 1. Check the Source Code
A class is `CopyConstructible` if it has a public copy constructor. Look for something like:
```cpp
ClassName(const ClassName&);
```
If this constructor is public and not deleted, the class is `CopyConstructible`.

### 2. Use Type Traits
C++ provides type traits in the `<type_traits>` header that can be used to check certain properties at compile-time. You can use `std::is_copy_constructible` to check if a type is `CopyConstructible`. For example:
```cpp
#include <type_traits>

if constexpr (std::is_copy_constructible<ThirdPartyClass>::value) {
    // The class is CopyConstructible
} else {
    // The class is not CopyConstructible
}
```
This method is useful when you want to write code that depends on whether a type is `CopyConstructible` or not.

### 3. Attempt Compilation
As a quick-and-dirty method, you can write a small piece of code that attempts to copy an instance of the class. If the code fails to compile, it's likely that the class is not `CopyConstructible`. However, this method is less precise and should be used cautiously.

## std::any vs templates

In C++, both `std::any` and templates serve different purposes and have different use cases. Understanding the differences and when to use each is important for effective C++ programming.

### **1. `std::any`:**

- **Type Erasure:** `std::any` is a type-erased container that can hold an instance of any type. This means that the type of the object stored in `std::any` is not known at compile time, only at runtime. It can store any type, but type information is erased, and you need to know the type to retrieve the stored object.

- **Runtime Polymorphism:** Since `std::any` works with any type, it provides a form of runtime polymorphism. You can store different types in the same container or pass around different types using `std::any`.

- **Performance:** Using `std::any` incurs runtime overhead. This includes type checks, storage management, and potential dynamic memory allocations. This makes it less efficient compared to templates, which are resolved at compile time.

- **Usage Example:**

    ```cpp
    #include <any>
    #include <iostream>

    int main() {
        std::any value = 10;  // Storing an int
        value = std::string("Hello");  // Storing a string
        
        try {
            std::cout << std::any_cast<std::string>(value) << std::endl;
        } catch (const std::bad_any_cast& e) {
            std::cout << "Bad cast: " << e.what() << std::endl;
        }
        
        return 0;
    }
    ```

- **Use Case:** `std::any` is useful when you need to store or pass around objects of different types without knowing the type in advance or when working with APIs that require handling various types generically.

### **2. Templates:**

- **Compile-Time Polymorphism:** Templates provide compile-time polymorphism, meaning the code is generated at compile time based on the types provided. This allows for type-safe and efficient code generation.

- **Type Safety:** Templates are type-safe, as the types are known at compile time. This allows the compiler to catch type errors before the program runs.

- **Performance:** Templates are generally more performant than `std::any` because the compiler generates specialized code for each type, eliminating the need for runtime type checks and dynamic allocations.

- **Usage Example:**

    ```cpp
    #include <iostream>

    template<typename T>
    void print(T value) {
        std::cout << value << std::endl;
    }

    int main() {
        print(10);          // Prints an integer
        print("Hello");     // Prints a string literal
        print(3.14);        // Prints a double

        return 0;
    }
    ```

- **Use Case:** Templates are ideal when you want to write generic and efficient code that works with different types but where the types are known at compile time.

### **Comparison Summary:**

| Feature         | `std::any`                          | Templates                        |
|-----------------|-------------------------------------|----------------------------------|
| Type Handling   | Runtime type erasure                | Compile-time type resolution     |
| Type Safety     | Type safety only at runtime         | Type safety at compile-time      |
| Performance     | Slower, with potential overhead     | Generally faster, optimized code |
| Flexibility     | Can hold any type, very flexible    | Generic, but types must be known at compile time |
| Usage Scenario  | Heterogeneous type containers, APIs | Generic programming, containers, algorithms |

### **When to Use Which:**
- **Use `std::any`** when you need to store or pass around objects of unknown types at runtime and are willing to accept the associated performance costs.
  
- **Use templates** when you need to write type-safe, efficient, and generic code where types are known at compile time.

Choosing between `std::any` and templates depends on whether you need runtime flexibility (`std::any`) or compile-time efficiency and type safety (templates).
