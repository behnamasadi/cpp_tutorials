# How does return know to call std::move or return a copy
In C++11 and later, the mechanism that allows for efficient return of objects, like `std::unique_ptr`, without explicit use of `std::move` is known as *Return Value Optimization (RVO)* and its special case, *Named Return Value Optimization (NRVO)*. Additionally, starting with C++17, the language standard mandates an even more powerful optimization known as *guaranteed copy elision*.

### Understanding RVO and NRVO

1. **Return Value Optimization (RVO)**: This optimization allows the compiler to construct the return value of a function directly in the location where it is used. This means that instead of creating a temporary object that is then copied or moved to the target location, the object is constructed directly in the target location, avoiding the need for a copy or move.

2. **Named Return Value Optimization (NRVO)**: This is a specific case of RVO where the returned object has a name (i.e., it is a named variable within the function). NRVO allows the compiler to eliminate the copy or move of the named object, directly constructing it in the place it will be used.

### C++17 and Guaranteed Copy Elision

C++17 introduces guaranteed copy elision which further optimizes return values. In C++17, when a function returns a prvalue (a temporary or an expression that initializes an object), the compiler is required to omit both the copy and the move operation, even if the copy/move constructor has side effects. This change makes returning objects like `std::unique_ptr` even more efficient.

### How It Works with `std::unique_ptr`

When returning a `std::unique_ptr`:

- **Before C++17**: RVO or NRVO may apply. The compiler can optimize away the move operation, but this is not guaranteed. If RVO or NRVO does not apply, the move constructor is called because `std::unique_ptr` cannot be copied.

- **C++17 and Later**: Guaranteed copy elision applies. The `std::unique_ptr` is constructed directly in the memory location where it's expected, without any need for a move operation. This is now guaranteed by the standard, making the code both efficient and predictable.

### Example

```cpp
std::unique_ptr<int> createResource() {
    return std::make_unique<int>(10);
    // Before C++17: RVO/NRVO may apply, or move constructor is called.
    // C++17 and later: Guaranteed copy elision, no move needed.
}
```

In this example, the `std::unique_ptr` is returned efficiently without the need for an explicit `std::move`, thanks to RVO, NRVO, and guaranteed copy elision in C++17 and later versions. This optimization is a key feature in modern C++ that enables both performance and ease of use.




## Scope of return values in C++

Refs: [1](https://stackoverflow.com/questions/275214/scope-and-return-values-in-c)

## Return smart pointers from functions
return smart pointers if the caller wants to manipulate the smart pointer itself, return raw pointers/references if the caller just needs a handle to 
the underlying object.
If you really need to return smart pointers from a function, always return by value.

```cpp
std::unique_ptr<Object> getUnique();
std::shared_ptr<Object> getShared();
std::weak_ptr<Object>   getWeak();
```

There are at least three good reasons for this:


1) Once again, smart pointers are powered by move semantics: the dynamically-allocated resource they hold is moved around, not wastefully copied.

2) modern compilers play the Return Value Optimization (RVO) trick. They are able to detect that you are returning an object by value, and they apply a sort of return shortcut to avoid useless copies. Starting from C++17, this is guaranteed by the standard. So even the smart pointer itself will be optimized out.

3) returning `std::shared_ptr` by reference doesn't properly increment the reference count, which opens up the risk of deleting something at the wrong time.

Thanks to point 2. you don't need move anything when returning a std::unique_ptr:
```cpp
std::unique_ptr<Object> getUnique()
{
    std::unique_ptr<Object> p = std::make_unique<Object>();
    return p; 
    // also return std::make_unique<Object>();
}
```

Refs: [1](https://stackoverflow.com/questions/4316727/returning-unique-ptr-from-functions)

## Named/ Return Value Optimization (RVO/ NRVO)


## Copy elision (Copy omission) 

