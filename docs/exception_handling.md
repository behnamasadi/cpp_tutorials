# C++ Standard Exceptions

1) std::bad_alloc  
2) std::bad_cast  
3) std::bad_exception  
4) std::bad_typeid  
5) std::logic_error  
    1) std::domain_error: exception thrown when a mathematically invalid domain is used.  
    2) std::invalid_argument  
    3) std::length_error  
    4) std::out_of_range  
6) std::runtime_error: An exception that theoretically cannot be detected by reading the code.  
    1) std::overflow_error: The only standard library components that throw std::overflow_error are std::bitset::to_ulong and std::bitset::to_ullong.  
    2) std::underflow_error  
    3) std::range_error  
    

## std::bad_alloc

```cpp

  try
  {
      while (true)
      {
          new int[100000000ul];
      }
  } catch (const std::bad_alloc& e)
  {
      std::cout << "Allocation failed: " << e.what() << '\n';
  }
```

### std::nothrow

This constant value is used as an argument for operator new and operator new[] to indicate that these functions shall not throw an exception on failure,
but return a null pointer instead.

```cpp
    try {
        while (true) {
            new int[100000000ul];   // throwing overload
        }
    } catch (const std::bad_alloc& e) {
        std::cout << e.what() << '\n';
    }
 
    while (true) {
        int* p = new(std::nothrow) int[100000000ul]; // non-throwing overload
        if (p == nullptr) {
            std::cout << "Allocation returned nullptr\n";
            break;
        }
    }
```    
Refs: [1](https://www.cplusplus.com/reference/new/nothrow/), [2](https://en.cppreference.com/w/cpp/memory/new/nothrow)


[code](../src/exception_handling.cpp)
