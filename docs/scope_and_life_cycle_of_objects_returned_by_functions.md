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

