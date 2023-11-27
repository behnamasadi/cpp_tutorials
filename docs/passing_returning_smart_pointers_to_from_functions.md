- [Passing smart pointers to functions](#passing-smart-pointers-to-functions)
  * [unique_pointer](#unique-pointer)
    + [Good Practices](#good-practices)
      - [1. Passing by Reference](#1-passing-by-reference)
      - [2. Moving Ownership](#2-moving-ownership)
    + [Bad Practices](#bad-practices)
      - [1. Passing by Value Without `std::move`](#1-passing-by-value-without--std--move-)
      - [2. Returning a Unique Pointer to a Local Object](#2-returning-a-unique-pointer-to-a-local-object)
    + [Corrected Example for Returning a `std::unique_ptr`](#corrected-example-for-returning-a--std--unique-ptr-)
      - [3. Using Raw Pointers Alongside](#3-using-raw-pointers-alongside)
  * [shared_pointer](#shared-pointer)
    + [Good Practices](#good-practices-1)
      - [1. Passing by Value](#1-passing-by-value)
      - [2. Returning a `std::shared_ptr`](#2-returning-a--std--shared-ptr-)
    + [Bad Practices](#bad-practices-1)
      - [1. Passing by Reference to Modify the `std::shared_ptr`](#1-passing-by-reference-to-modify-the--std--shared-ptr-)
      - [2. Returning a `std::shared_ptr` to a Local Object](#2-returning-a--std--shared-ptr--to-a-local-object)
      - [3. Creating `std::shared_ptr` from Raw Pointers Arbitrarily](#3-creating--std--shared-ptr--from-raw-pointers-arbitrarily)
  * [weak_ptr](#weak-ptr)
    + [Good Practices](#good-practices-2)
      - [1. Passing `std::weak_ptr` by Value](#1-passing--std--weak-ptr--by-value)
      - [2. Returning `std::weak_ptr`](#2-returning--std--weak-ptr-)
    + [Bad Practices](#bad-practices-2)
      - [1. Using `std::weak_ptr` Without Checking](#1-using--std--weak-ptr--without-checking)
      - [2. Storing `std::weak_ptr` as a Permanent Reference](#2-storing--std--weak-ptr--as-a-permanent-reference)
    + [Summary](#summary)


# Passing smart pointers to functions
Read the full [here](passing_returning_smart_pointers_to_from_functions.md)




According to the [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#r30-take-smart-pointers-as-parameters-only-to-explicitly-express-lifetime-semantics) a function should take a smart pointer as parameter only if it examines/manipulates the smart pointer itself, i.e:
counting the references of a std::shared_ptr, increasing them by making a copy, change of ownership (moving data from a std::unique_ptr to another one).


## unique_pointer
Let's explore some good and bad practices with examples.

```cpp
struct S {
  int m_id;
  S(int id) {
    m_id = id;
    std::cout << "ctor " << m_id << std::endl;
  }

  ~S() { std::cout << "dtor " << m_id << std::endl; }

  S(S &&rhs) { std::cout << "move ctor " << m_id << std::endl; }

  S &operator=(S &&rhs) {
    std::cout << "move assignment operator " << m_id << std::endl;
    return *this;
  }
};
```

### Good Practices

#### 1. Passing by Reference
It's often advisable to pass unique pointers by reference if the function is not going to take ownership of the pointer. This avoids unnecessary transfers of ownership.

```cpp
#include <memory>

void useResource(std::unique_ptr<S>& ptr) {
    // Use the resource without taking ownership
    if (ptr) {
        // Do something with *ptr
    }
}

int main() {
    std::unique_ptr<S> myPtr = std::make_unique<S>(10);
    useResource(myPtr);
    // myPtr is still valid here
}
```

#### 2. Moving Ownership
When the function needs to take ownership of the unique pointer, use `std::move` to transfer ownership explicitly.

```cpp
#include <memory>

void takeOwnership(std::unique_ptr<S> ptr) {
    // Now owns the resource
}

int main() {
    std::unique_ptr<S> myPtr = std::make_unique<S>(10);
    takeOwnership(std::move(myPtr));
    // myPtr is now empty
}
```

### Bad Practices

#### 1. Passing by Value Without `std::move`
Passing a unique pointer by value without using `std::move` results in a compilation error, as unique pointers cannot be copied.

```cpp
#include <memory>

void process(std::unique_ptr<S> ptr) {
    // ...
}

int main() {
    std::unique_ptr<int> myPtr = std::make_unique<int>(10);
    process(myPtr); // Compilation error: unique_ptr cannot be copied
}
```

#### 2. Returning a Unique Pointer to a Local Object
Returning a unique pointer to a locally created object is a bad practice as it can lead to undefined behavior.

```cpp
#include <memory>

std::unique_ptr<int> createResource() {
    int local = 10;
    return std::make_unique<int>(local);
    // Returning a pointer to a local variable is dangerous
}

int main() {
    auto ptr = createResource();
    // Undefined behavior
}
```

The problem in this example is not a correct usage. The proper way to return a `std::unique_ptr` is to ensure that the object it manages is dynamically allocated (usually on the heap).

### Corrected Example for Returning a `std::unique_ptr`

```cpp
#include <memory>

std::unique_ptr<int> createResource() {
    // Dynamically allocate the resource
    std::unique_ptr<S> ptr = std::make_unique<S>(10);
    return ptr; // Safely return the unique_ptr
}

int main() {
    // ptr now owns the resource created in createResource()
    std::unique_ptr<S> ptr = createResource();
    // Use ptr as needed
    // Memory is automatically managed
}
```

In this corrected example, `createResource` function dynamically allocates an `S` and wraps it in a `std::unique_ptr`. This unique pointer is then returned. Because of the move semantics in C++, the ownership of the heap-allocated `S` is transferred safely from `createResource` to `ptr` in `main`. This avoids any issues with memory leaks or dangling pointers, ensuring proper resource management.


[How does return know to call std::move or return a copy?](scope_and_life_cycle_of_objects_returned_by_functions.md)

#### 3. Using Raw Pointers Alongside
Mixing raw pointers with unique pointers can lead to confusion about ownership and potentially cause memory leaks or double deletions.

```cpp
#include <memory>

void useRawPointer(int* ptr) {
    // Risk of incorrect memory management
}

int main() {
    std::unique_ptr<int> myPtr = std::make_unique<int>(10);
    useRawPointer(myPtr.get()); // Now both raw and unique pointers refer to the same memory
    // Risk of double deletion or memory leak
}
```

In summary, when working with `std::unique_ptr` in C++, it's essential to handle ownership correctly. Use references when ownership should not be transferred, use `std::move` to transfer ownership explicitly, and avoid practices that can lead to undefined behavior or memory management issues.






## shared_pointer

### Good Practices

#### 1. Passing by Value
For `std::shared_ptr`, passing by value is usually fine as it expresses the intent to share ownership of the resource. Each time a `shared_ptr` is copied, the reference count is incremented.

```cpp
#include <memory>

void processShared(std::shared_ptr<S> s_ptr) {
  // Do something with ptr
  // Reference count is maintained automatically
  std::cout << "Reference count: " << s_ptr.use_count() << std::endl;
}

int main() {

   std::cout << "Reference count: " << s_ptr.use_count() << std::endl;
   processShared(s_ptr);
   std::cout << "Reference count: " << s_ptr.use_count() << std::endl;
    // sharedPtr remains valid here
}
```

#### 2. Returning a `std::shared_ptr`
Returning a `std::shared_ptr` from a function is a way to share ownership of a resource. It's a clear and safe way to transfer shared ownership.

```cpp
#include <memory>

std::shared_ptr<S> createSharedResource() {
  std::shared_ptr<S> s_ptr = std::make_shared<S>(100);
  std::cout << "Reference count: " << s_ptr.use_count() << std::endl;
  return s_ptr;
}

int main() {
  auto sharedPtr = createSharedResource();
  std::cout << "Reference count: " << sharedPtr.use_count() << std::endl;
    // sharedPtr now shares ownership of the resource
}
```


#### 3. std::shared_ptr copied or moved ?


when returning a `std::shared_ptr` from a function, whether the pointer is copied or moved depends on a few factors, primarily related to the function's return type and the context in which the return statement is used. Here's how it generally works:

1. **Copy Semantics**: If a `std::shared_ptr` is returned from a function and the return statement uses an lvalue (an object that has a name or is identifiable), the copy constructor of `std::shared_ptr` is invoked. This increases the reference count of the managed object.

   ```cpp
   std::shared_ptr<MyObject> createObject() {
       std::shared_ptr<MyObject> ptr = std::make_shared<MyObject>();
       return ptr; // Copy constructor is called
   }
   ```

2. **Move Semantics**: If the return statement uses an rvalue (temporary objects or objects that are about to be destroyed), the move constructor of `std::shared_ptr` is invoked. This transfers ownership without increasing the reference count.

   ```cpp
   std::shared_ptr<MyObject> createObject() {
       return std::make_shared<MyObject>(); // Move constructor is called
   }
   ```

3. **Return Value Optimization (RVO)**: Modern C++ compilers often apply an optimization called Return Value Optimization or RVO. RVO allows the compiler to construct the return value directly in the memory location where the function's return value would be stored, thereby eliminating the need for a copy or move. This optimization is particularly common when returning a temporary `std::shared_ptr` object.

4. **Named Return Value Optimization (NRVO)**: Similar to RVO, NRVO applies when returning a named local object. The compiler may optimize away the copy or move operation, constructing the object directly in the location it will be used.

5. **Explicit Move**: You can also explicitly use `std::move` to indicate that you want to use move semantics. This is useful when you have an lvalue that you want to treat as an rvalue.

   ```cpp
   std::shared_ptr<MyObject> createObject() {
       std::shared_ptr<MyObject> ptr = std::make_shared<MyObject>();
       return std::move(ptr); // Explicit move
   }
   ```

In summary, whether a `std::shared_ptr` is copied or moved when returned from a function depends on whether the returned object is an lvalue or rvalue, and compiler optimizations like RVO and NRVO can also play a significant role in how the return is handled.





### Bad Practices

#### 1. Passing by Reference to Modify the `std::shared_ptr`
Passing a `std::shared_ptr` by reference to modify the actual `std::shared_ptr` object is generally not recommended. It can lead to unclear ownership semantics in the calling code.

```cpp
#include <memory>

void riskyFunction(std::shared_ptr<S>& ptr) {
    ptr = std::make_shared<S>(20); // Modifies the original shared_ptr
}

int main() {
    auto sharedPtr = std::make_shared<S>(10);
    riskyFunction(sharedPtr);
    // sharedPtr has been modified; might be unexpected in the calling context
}
```

#### 2. Returning a `std::shared_ptr` to a Local Object
Creating a `shared_ptr` for a local (stack-allocated) object can lead to undefined behavior since the object goes out of scope and gets destroyed, but the `shared_ptr` might still exist.

```cpp
#include <memory>

std::shared_ptr<S> badIdea() {
    S local (10);
    return std::shared_ptr<S>(&local); // Very bad: points to a local variable
}

int main() {
    auto sharedPtr = badIdea();
    // Undefined behavior
}
```

The issue with returning a `std::shared_ptr` to a local object lies in the fact that stack-allocated objects are automatically destroyed when they go out of scope. This leads to undefined behavior if a `std::shared_ptr` attempts to manage such an object. The correct way to return a `std::shared_ptr` is to ensure it manages a dynamically allocated object. Here's the corrected example:


```cpp
#include <memory>

std::shared_ptr<S> createSharedResource() {
    // Correctly create a shared_ptr managing a dynamically allocated object
    std::shared_ptr<S> ptr = std::make_shared<S>(10);
    return ptr; // Safely return the shared_ptr
}

int main() {
    auto sharedPtr = createSharedResource();
    // sharedPtr now shares ownership of the dynamically allocated int
}
```

In this corrected version, `createSharedResource` function creates a `std::shared_ptr` that manages a heap-allocated `int`. This shared pointer is then returned from the function. Since `std::shared_ptr` uses reference counting, it ensures that the dynamically allocated `int` is properly deleted when the last `shared_ptr` managing it is destroyed or reset.

This approach is safe and follows the best practices for memory management in modern C++, ensuring that there are no dangling pointers or memory leaks. It takes advantage of `std::shared_ptr`'s automatic memory management capabilities, making the code more robust and easier to maintain.


#### 3. Creating `std::shared_ptr` from Raw Pointers Arbitrarily
Creating multiple `shared_ptr`s from the same raw pointer can lead to double-free errors, as each `shared_ptr` will try to delete the resource independently.

```cpp
#include <memory>

int main() {
    int* rawPtr = new S(10);
    std::shared_ptr<S> sharedPtr1(rawPtr);
    std::shared_ptr<S> sharedPtr2(rawPtr); // Dangerous: Two shared_ptrs managing the same raw pointer
    // Double-free error when sharedPtr1 and sharedPtr2 go out of scope
}
```

In summary, when working with `std::shared_ptr`, it's important to use value semantics to share ownership, avoid modifying the `shared_ptr` object directly through references, and be careful not to create multiple `shared_ptr`s from the same raw pointer. Proper understanding and handling of shared pointers are crucial for effective resource management and avoiding undefined behavior in C++ applications.


## weak_ptr

`std::weak_ptr` in C++ is used to hold a non-owning ("weak") reference to an object that is managed by a `std::shared_ptr`. Let's explore good and bad practices for sending or returning `std::weak_ptr` to functions with examples.

### Good Practices

#### 1. Passing `std::weak_ptr` by Value
Passing a `std::weak_ptr` by value is usually fine because it does not affect the reference count of the object managed by the corresponding `std::shared_ptr`.

```cpp
#include <iostream>
#include <memory>

void processWeakPtr(std::weak_ptr<S> weakPtr) {
    auto sharedPtr = weakPtr.lock(); // Convert to shared_ptr before using
    if (sharedPtr) {
        std::cout << "Shared resource value: " << *sharedPtr.id << std::endl;
    } else {
        std::cout << "Resource is no longer available." << std::endl;
    }
}

int main() {
    auto sharedPtr = std::make_shared<S>(10);
    std::weak_ptr<S> weakPtr = sharedPtr;
    processWeakPtr(weakPtr);
    // sharedPtr remains valid here
}
```

#### 2. Returning `std::weak_ptr`
Returning a `std::weak_ptr` from a function can be useful to provide temporary access to an object without affecting its lifetime.

```cpp
#include <memory>

std::weak_ptr<S> getWeakResource(const std::shared_ptr<S>& sharedPtr) {
    return std::weak_ptr<S>(sharedPtr);
}

int main() {
    auto sharedPtr = std::make_shared<S>(20);
    auto weakPtr = getWeakResource(sharedPtr);
    // Use weakPtr as needed
}
```

### Bad Practices

#### 1. Using `std::weak_ptr` Without Checking
Using `std::weak_ptr` directly without converting it to a `std::shared_ptr` and checking its validity can lead to undefined behavior.

```cpp
#include <memory>

void unsafeFunction(std::weak_ptr<S> weakPtr) {
    // Incorrect: Directly dereferencing weakPtr without locking
    if (weakPtr.expired()) {
        // Handle expired weak pointer
    } else {
        S value = *(weakPtr.lock()); // Unsafe if the resource was just released
    }
}

int main() {
    // ...
}
```

#### 2. Storing `std::weak_ptr` as a Permanent Reference
Using `std::weak_ptr` as a permanent reference to an object can be misleading, as the object might be destroyed while the `weak_ptr` still exists.

```cpp
#include <memory>

class Example {
    std::weak_ptr<S> weakRef;

public:
    void setWeakRef(const std::weak_ptr<S>& ref) {
        weakRef = ref; // Risky: The object weakRef points to might get destroyed
    }

    void useWeakRef() {
        // The object might no longer exist here
    }
};

int main() {
    // ...
}
```

### Summary

In summary, when working with `std::weak_ptr`, it's important to:

- Convert it to a `std::shared_ptr` using `lock()` and check its validity before using it.
- Understand that it's a non-owning reference and should not be used for long-term references where the lifetime of the object is critical.
- Use it when you need to observe an object but don't want to extend its lifetime. 

`std::weak_ptr` is particularly useful in avoiding circular references that can lead to memory leaks. However, its correct use requires careful consideration of the object's lifecycle to ensure safe and predictable code behavior.






