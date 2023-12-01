- [Exception Handling](#exception-handling)
- [C++ Standard Exceptions](#c-standard-exceptions)
  - [bad\_alloc](#bad_alloc)
    - [nothrow](#nothrow)
  - [bad\_cast](#bad_cast)
  - [bad\_typeid](#bad_typeid)
  - [logic\_error](#logic_error)
  - [domain\_error](#domain_error)
  - [invalid\_argument](#invalid_argument)
  - [length\_error](#length_error)
  - [out\_of\_range](#out_of_range)
  - [overflow\_error](#overflow_error)
  - [range\_error](#range_error)
- [User defined exceptions](#user-defined-exceptions)
- [](#)
- [Catching All Exceptions With Parameter Pack Expansion ...](#catching-all-exceptions-with-parameter-pack-expansion-)
- [noexcept](#noexcept)
  - [noexcept specifier](#noexcept-specifier)
  - [noexcept operator](#noexcept-operator)
  - [when should we use noexcept](#when-should-we-use-noexcept)



# Exception Handling
exception handling has the following form:

```cpp
try
{
    some code
}
catch (Exception1 e)
{
    some code
}
catch (Exception2 e)
{
    some code
}
catch (...)
{
    some code
}
```




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
    1) std::overflow_error
    2) std::underflow_error  
    3) std::range_error  
    
7) std::ios_base::failure
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

# User defined exceptions

Definition:
```cpp
struct CustomException : public std::exception
{
   const char * what () const throw ()
   {
      return "CustomException happened";
   }
};
```

usage:
```cpp
try
{
    throw CustomException();
} catch(CustomException& e)
{
    std::cout << "CustomException caught" << std::endl;
    std::cout << e.what() << std::endl;
} catch(std::exception& e)
{
//Other errors
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





# noexcept 

The `noexcept` specification in C++ is used to indicate that a function is not expected to throw exceptions. This helps in optimizing the code, as the compiler can make certain optimizations knowing that no exceptions will be thrown from that function. However, if an exception is thrown from a `noexcept` function, the program will call `std::terminate`, resulting in a potential program crash.

### When to Use `noexcept`

1. **Performance Critical Functions**: In functions where performance is critical, and you're sure that no exceptions will be thrown, using `noexcept` can improve performance.

2. **Move Constructors and Move Assignments**: It's generally good practice to mark move constructors and move assignments as `noexcept`. This is because many standard library implementations will only use move semantics if these operations are marked as `noexcept`.

3. **Functions Guaranteed Not to Throw**: If you're certain that a function won't throw an exception (like simple getters or setters that don't do any complex operations), marking them as `noexcept` can be a good practice.

### Real-life Scenario

Imagine you're developing a real-time game engine where performance is critical. You have a function that updates the position of a game object based on its velocity and the elapsed time. This function is straightforward and doesn't involve operations that might throw exceptions (like memory allocation, file I/O, etc.).

```cpp
class GameObject {
public:
    // Other members...

    // Update position - noexcept since it's a simple calculation
    void updatePosition(float elapsedTime) noexcept {
        position.x += velocity.x * elapsedTime;
        position.y += velocity.y * elapsedTime;
        // Other simple calculations...
    }

    // Other members...
};

// In the game loop
gameObject.updatePosition(elapsedTime);
```

In this scenario, using `noexcept` for `updatePosition` makes sense because:
1. The function is simple and unlikely to throw exceptions.
2. The function is likely called very frequently (every frame), so any performance improvement is beneficial.
3. If an exception does occur here, it likely indicates a severe logic error or a bug that should terminate the program, which is the behavior `noexcept` enforces.

Marking move constructors and move assignments as `noexcept` in C++ is considered good practice for several key reasons:

1. **Optimizations in Standard Library Containers**: Many standard library containers, like `std::vector` and `std::deque`, can perform certain optimizations if they know that move operations do not throw exceptions. For instance, when a `std::vector` resizes, it may choose to move its elements to the new memory location instead of copying them, but only if the move operations are `noexcept`. This can lead to significant performance improvements.

2. **Strong Exception Safety Guarantee**: By marking move operations as `noexcept`, you are ensuring that these operations won't throw exceptions, which aids in providing strong exception safety guarantees. This is particularly important in scenarios where maintaining system state consistency is crucial, and exceptions can lead to partial state changes or leaks.

3. **Better Resource Management and Safety**: In the context of resource management (like memory, file handles, network connections), move semantics allow for efficient transfer of resources. When these operations are `noexcept`, it ensures that the resource transfer is safe and no exceptions will be thrown during the process, preventing resource leaks or undefined states.

4. **Improved Compiler Error Messages**: If you mistakenly use a type in a context that requires a `noexcept` move operation and your type doesn't provide it, the compiler can give a clear and specific error message. This helps in catching potential issues at compile time.

5. **Compatibility with Move-Only Types**: Some types in C++ are move-only (like `std::unique_ptr`). If your class holds move-only members and your move operations are not `noexcept`, this can lead to complications or even prevent your class from being used in certain standard library containers or algorithms.

6. **Semantical Clarity**: Marking move operations as `noexcept` clearly communicates your intent to other developers that these operations are safe and won't throw exceptions. This enhances code readability and maintainability.

In summary, using `noexcept` with move constructors and assignments improves performance and exception safety, ensures better resource management, improves code clarity, and ensures compatibility with certain types and containers in the C++ standard library.


## noexcept specifier
This means if a function specified with noexcept it shouldn't throw exception (evaluation of its operand can propagate an exception).

The bodies of called functions are not examined to check if they actually throw exception or not, and `noexcept` can yield false negatives. 
In the case of exception `std::sterminate` will be called
## noexcept operator
It tests if a function noexcept specification evaluate to true or false at compile time.
noexcept(some compile time expression) and this returns a boolean

examples:


equals to `noexcept(true),`this `func` can not throw exception
```cpp
void func1() noexcept 
{
}
```

equals to not using noexcept, means this func2 can throw exception
```cpp
void func2() noexcept (false)
{
}
```



```cpp
void bar() noexcept
{
}

void baz() noexcept
{
    throw std::range_error("range error");
}

void foo()
{
    return;
}
```


now running the followings:

```cpp
std::cout << std::boolalpha;
std::cout << noexcept(bar()) << '\n';
std::cout << noexcept(baz()) << '\n';
std::cout << noexcept(foo()) << '\n';
std::cout << noexcept(1 + 1) << '\n'; 
```
the output is:

```
true
true
false
true
```

[code](../src/noexcept_operator_specifier.cpp) 


## when should we use noexcept
1. When using c++ functions in c
2. when c++ standard requires us.


Almost every optimization in the compiler uses something called a "flow graph" of a function to reason about what is legal. A flow graph consists of what are generally called "blocks" of the function (areas of code that have a single entrance and a single exit) and edges between the blocks to indicate where flow can jump to. Noexcept alters the flow graph.

A noexcept specification on a function is merely a method for a programmer to inform the compiler whether or not a function should throw exceptions.

The compiler can use this information to enable certain optimizations on non-throwing functions as well as enable the noexcept operator, which can check at compile time if a particular expression is declared to throw any exceptions.


Declaring a function noexcept helps optimizers by reducing the number of alternative execution paths. It also speeds up the exit after failure.




Refs: [1](https://akrzemi1.wordpress.com/2014/04/24/noexcept-what-for/), [2](https://stackoverflow.com/questions/10787766/when-should-i-really-use-noexcept), [3](https://stackoverflow.com/questions/33210169/how-to-use-noexcept-in-c-or-how-does-it-work), [4](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-constexpr), [5](https://www.modernescpp.com/index.php/c-core-guidelines-the-noexcept-specifier-and-operator#:~:text=throw()%20is%20equivalent%20to%20noexcept,be%20used%20for%20function%20overloading.)
