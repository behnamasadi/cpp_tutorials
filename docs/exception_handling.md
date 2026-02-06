# Exception Handling

Exception handling in C++ allows programs to react to runtime errors in a structured and safe way.

```cpp
try {
    // protected code
}
catch (const ExceptionType& e) {
    // handle specific exception
}
catch (...) {
    // handle any exception
}
```

---

# C++ Standard Exceptions

The standard library provides a hierarchy of exception types derived from `std::exception`:

* `std::bad_alloc`
* `std::bad_cast`
* `std::bad_exception`
* `std::bad_typeid`
* `std::logic_error`

  * `std::domain_error`
  * `std::invalid_argument`
  * `std::length_error`
  * `std::out_of_range`
* `std::runtime_error`

  * `std::overflow_error`
  * `std::underflow_error`
  * `std::range_error`
* `std::ios_base::failure`


---

## bad_alloc

This exception is thrown by the allocation functions to when it failures to allocate memory.

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

### nothrow

`nothrow` is an empty class type and used as an argument for operator `new` and operator `new[]` to indicate that these functions shall not throw an exception on failure, but return a null pointer instead.

```cpp
    try 
    {
        while (true) 
        {
            new int[100000000ul];   // throwing overload
        }
    } catch (const std::bad_alloc& e) 
    {
        std::cout << e.what() << '\n';
    }
 
    while (true) 
    {
        int* p = new(std::nothrow) int[100000000ul]; // non-throwing overload
        if (p == nullptr) 
        {
            std::cout << "Allocation returned nullptr\n";
            break;
        }
    }
```

Refs: [1](https://www.cplusplus.com/reference/new/nothrow/), [2](https://en.cppreference.com/w/cpp/memory/new/nothrow)


## bad_cast  

This exception is thrown when a `dynamic_cast` to a reference type fails, for instance when the types are not related by inheritance.


```cpp
struct Foo { virtual ~Foo() {} };
struct Bar { virtual ~Bar() {} };

Bar b;
try
{
	Foo& f = dynamic_cast<Foo&>(b);
} catch(const std::bad_cast& e)
{
	std::cout << e.what() << '\n';
}
```






## bad_typeid

This exception is thrown when a `typeid` operator is applied to a dereferenced null pointer value of a polymorphic type.

The type has to be polymorphic:
```cpp
struct Foo 
{ 
    virtual void bar();
}; 
```


```cpp
Foo* p = nullptr;
try
{
	std::cout << typeid(*p).name() << '\n';
} catch(const std::bad_typeid& e) {
	std::cout << e.what() << '\n';
}
```


## logic_error

This defines a type of object to be thrown as exception when there are a consequence of faulty logic in your program.

```cpp
int amount, available;
amount=10;
available=9;

try
{
  if(amount>available)
    throw std::logic_error( "logic error" );
}
catch ( std::exception &e )
{
  std::cerr << "Caught: " << e.what( ) << std::endl;
  std::cerr << "Type: " << typeid( e ).name( ) << std::endl;
};
```
## domain_error

This defines a type of object to be thrown as exception in situations where the inputs are outside of the domain on which an operation is defined.


```cpp
try
{
    const double x = std::acos(2.0);
    std::cout << x << '\n';
}
catch (std::domain_error& e)
{
    std::cout << e.what() << '\n';
}
```


## invalid_argument

This defines a type of object to be thrown as exception when an argument value has not been accepted.

binary wrongly represented by char `X`:
```cpp
try
{
    std::bitset<32> bitset(std::string("0101001X01010110000"));
}
catch (std::exception &err)
{
    std::cerr<<"Caught "<<err.what()<<std::endl;
    std::cerr<<"Type "<<typeid(err).name()<<std::endl;
}
```

## length_error

This defines a type of object to be thrown as exception as a result of attempts to exceed implementation defined length limits for some object. For instance vector throws a `length_error` if resized above `max_size`
```cpp
try
{
    std::vector<int> myvector;
    myvector.resize(myvector.max_size()+1);
}
catch (const std::length_error& le)
{
    std::cerr << "Length error: " << le.what() << '\n';
}
```

## out_of_range

This defines a type of object to be thrown as exception as consequence of attempt to access elements out of defined range.

```cpp
std::vector<int> myvector(10);
try
{
    myvector.at(20)=100;      // vector::at throws an out-of-range
}
catch (const std::out_of_range& oor)
{
    std::cerr << "Out of Range error: " << oor.what() << '\n';
}
```


## overflow_error
The only standard library components that throw `std::overflow_error` are `std::bitset::to_ulong` and `std::bitset::to_ullong`.  
```cpp
try
{
    std::bitset<100> bitset;
    bitset[99] = 1;
    bitset[0] = 1;
    // to_ulong(), converts a bitset object to the integer that would generate the sequence of bits
    unsigned long Test = bitset.to_ulong();
}
catch(std::exception &err)
{
    std::cerr<<"Caught "<<err.what()<<std::endl;
    std::cerr<<"Type "<<typeid(err).name()<<std::endl;
}
```


## range_error

This defines a type of object to be thrown as exception where a result of a computation cannot be represented by the destination type.
The only standard library components that throw this exception are `std::wstring_convert::from_bytes` and `std::wstring_convert::to_bytes`.
```cpp
try
{
   throw std::range_error( "The range is in error!" );
}
catch (std::range_error &e)
{
   std::cerr << "Caught: " << e.what( ) << std::endl;
   std::cerr << "Type: " << typeid( e ).name( ) << std::endl;
}
```

# 

```cpp
    std::ifstream f("doesn't exist");
   f.exceptions ( ifstream::badbit ); // No need to check failbit
    try
    {
        f.exceptions(f.failbit);
    }
    catch (const std::ios_base::failure& e)
    {
        std::cout << "Caught an ios_base::failure.\n"
                  << "Explanatory string: " << e.what() << '\n'
                  << "Error code: " << e.code() << '\n';
    }
```


Ref: [1](https://en.cppreference.com/w/cpp/io/ios_base/failure)

[code](../src/exception_handling.cpp)


# Catching All Exceptions With Parameter Pack Expansion ...
`...` is a parameter pack and refers to zero or more template parameters. The `...` will catch **all** exception.



```cpp
try
{
    some code
}
catch (Exception e)
{
    some code
}
catch (...)//... Parameter Pack Expansion, will catch any exception
{
    some code
}
```




---

# User-defined Exceptions

## Overriding `std::exception::what()`

`std::exception` exposes **one virtual function intended for overriding**:

```cpp
virtual const char* what() const noexcept;
```

Important points:

* We override **`what()`**, not `throw()`
* `throw()` is an old C++98 exception specification
* `noexcept` is the modern replacement
* The function must **never throw**

Correct modern definition:

```cpp
struct CustomException : public std::exception {
  const char* what() const noexcept override {
    return "CustomException happened";
  }
};
```

Why `what()` is `noexcept`:

* It is usually called while another exception is already being handled
* Throwing during exception handling is fatal
* The standard enforces safety via `noexcept`

Usage:

```cpp
try {
  throw CustomException();
} catch (const std::exception& e) {
  std::cout << e.what() << '
';
}
```

---

# noexcept

## What `noexcept` means

`noexcept` declares a **non-throwing contract**:

> This function guarantees it will not throw exceptions.

If the guarantee is violated, the program calls `std::terminate()` immediately.

### `noexcept` vs `throw()`

| Old syntax      | Modern C++ |
| --------------- | ---------- |
| `throw()`       | `noexcept` |
| `throw(T1, T2)` | ❌ removed |

---

## Stack Unwinding

When an exception is thrown:

1. Normal execution stops
2. The runtime walks up the call stack
3. Destructors of fully-constructed objects are invoked
4. A matching `catch` is searched

This cleanup process is called **stack unwinding**.

---

## Why Destructors MUST be `noexcept`

### Fatal case: throwing during unwinding

```cpp
struct Bad {
  ~Bad() {
    throw std::runtime_error("destructor failed");
  }
};

void f() {
  Bad b;
  throw std::runtime_error("original error");
}
```

Execution order:

* `original error` triggers stack unwinding
* `~Bad()` is invoked
* `~Bad()` throws another exception
* Two active exceptions now exist

### Result

```text
terminate called after throwing an instance of 'std::runtime_error'
  what():  original error
Aborted (core dumped)

```

The C++ standard mandates this behavior: **throwing while another exception is active is fatal**.

---

## Language Rule

Since C++11, destructors are implicitly:

```cpp
~T() noexcept(true);
```

Violating this guarantee leads to immediate program termination.

---

## Correct RAII Design

### ❌ Incorrect

```cpp
struct File {
  FILE* f;
  ~File() {
    if (std::fclose(f) != 0)
      throw std::runtime_error("close failed");
  }
};
```

### ✅ Correct

```cpp
struct File {
  FILE* f;

  ~File() noexcept {
    if (f) std::fclose(f); // best-effort cleanup
  }

  void close() {
    if (std::fclose(f) != 0)
      throw std::runtime_error("close failed");
    f = nullptr;
  }
};
```

Rule:

* Destructors clean up resources
* Errors are reported explicitly via functions
* Destructors must never throw

---

## Other Places `noexcept` Is Required or Critical

### `std::exception::what()`

```cpp
virtual const char* what() const noexcept;
```

Reason:

* Called during exception handling
* Must be safe and non-throwing

---

### Move Constructors and Move Assignment

```cpp
MyType(MyType&&) noexcept;
```

Why:

* Standard containers prefer move only if it is `noexcept`
* Otherwise they fall back to copying
* Enables faster reallocation and strong exception guarantees

---

## Summary

| Context          | Must be `noexcept`   | Reason                          |
| ---------------- | -------------------- | ------------------------------- |
| Destructors      | Yes                  | Prevent fatal double exceptions |
| `what()`         | Yes                  | Safe error reporting            |
| Move ctor/assign | Strongly recommended | Container optimizations         |
| Cleanup code     | Yes                  | Stack unwinding safety          |

---

## One-line Rule

A function must be `noexcept` if it can be executed during stack unwinding or cleanup, because throwing at that point immediately terminates the program.