## Raw pointer

 The size of a pointer is dependent upon the architecture of the computer — a 32-bit computer uses 32-bit memory addresses — consequently, 
a pointer on a 32-bit machine is 32 bits (4 bytes). On a 64-bit machine, a pointer  would be 64 bits (8 bytes).
This is true regardless of what is being pointed to:

```cpp
std::cout<<"sizeof(char *)= " << sizeof(char *)<<std::endl;
std::cout<<"sizeof(int *)= " <<sizeof(int *)<<std::endl;
std::cout<<"sizeof(void *)= " <<sizeof(void *)<<std::endl;
std::cout<<"sizeof(double *)= " <<sizeof(double *)<<std::endl;
std::cout<<"sizeof(person *)= " <<sizeof(person *)<<std::endl;
std::cout<<"sizeof(employee *)= " <<sizeof(employee *)<<std::endl;
```

## shared_ptr
There are seevral ways to create a shared pointer:

Single line with new:
```cpp
std::shared_ptr<manager> ceo=std::shared_ptr<manager>(new manager);
std::shared_ptr<manager> ceo1(new manager);
```

Multi line:
```cpp
std::shared_ptr<manager> cto;
cto.reset(new manager);
```

Prefered way using make_shared:
```cpp
std::shared_ptr<manager> boss=std::make_shared<manager>();
```
When you create an object with new , and then create a shared_ptr , there are two dynamic memory allocations that happen: 
one for the object itself from the new, and then a second for the manager object created by the shared_ptr constructor.
 when you use make_shared, C++ compiler does a single memory allocation big enough to hold both the manager object and the new object.

when  you erase items in a vector the occupied space will be freed, but if you have stored pointers inside a vector you have to manualy call delete 
function to free the space.

The destructor for `std::vector<T>` ensures that the destructor for T is called for every element stored in the vector.


## unique_ptr
unique pointer, they have less overhead.A unique pointer has no copy constructor can not be copied, can be moved. Use a unique pointer instead of a 
shared pointer where an unique pointer suffices.

First way:
```cpp
std::unique_ptr<person> entity=std::unique_ptr<person>(new person);
```

Second way(recommended) for exception safety
```cpp
std::unique_ptr<person> entity=std::make_unique<person>();
```

This will fail because it is a unique pointer, it can not be copied:
```cpp
std::unique_ptr<person> secondentity=entity;
```

This will work because because we have move constructor
```cpp
std::unique_ptr<person> secondentity=std::move(entity);
```

## weak_pointer

Weak pointer, they don't increase the ref count:

```cpp
std::shared_ptr<person> person0;
{
    std::shared_ptr<person> sharedEntity0(new person(0));
    person0=sharedEntity0;
}
std::cout<<"sharedEntity0 is still alive because of person0=sharedEntity0" <<std::endl;
```

if we change the above code to the following the object will die after the {} block

```cpp
std::weak_ptr<person> person1;
{
    std::shared_ptr<person> sharedEntity1(new person(1));
    person1=sharedEntity1;
}
std::cout<<"sharedEntity1 died befor reaching here since person1 is a weak_ptr" <<std::endl;
```

## Pointer casting

```cpp
std::shared_ptr<Base> b(new Base());
std::shared_ptr<Derived> d = std::static_pointer_cast<Derived>(b);

std::shared_ptr<person> b(new person());
std::shared_ptr<manager> d = std::static_pointer_cast<manager>(b);

```

## Avoiding cyclic references when using shared pointers 

## Passing smart pointers to functions
Smart pointers can be passed to functions in following ways
```cpp
void f(std::unique_ptr<Object>);    // (1)
void f(std::shared_ptr<Object>);    // (2)
void f(std::weak_ptr<Object>);      // (3)
void f(std::unique_ptr<Object>&);   // (4)
void f(std::shared_ptr<Object>&);   // (5) also const &
void f(Object&);                    // (6) also const &
void f(Object*);                    // (7) also const *
```

According to the [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#r30-take-smart-pointers-as-parameters-only-to-explicitly-express-lifetime-semantics) a function should take a smart pointer as parameter only if it examines/manipulates the smart pointer itself, i.e:
counting the references of a std::shared_ptr, increasing them by making a copy, change of ownership (moving data from a std::unique_ptr to another one).

### Pass by value to lend the ownership
Cases in (1), (2), (3), Pass smart pointers by value to lend their ownership to the function, that is when the function wants its own copy of 
the smart pointer in order to operate on it.


1) A std::unique_ptr can't be passed by value because it can't be copied, so it is usually moved around with the special function std::move from the Standard Library. This is move semantics in action:

```cpp
std::unique_ptr<Object> up = std::make_unique<Object>();
function(std::move(up)); // Usage of (1)
```
After the call `up` is a hollow object. This is known as a sink: the ownership of the dynamically-allocated resource flows down an imaginary sink from one point to another;

2) There's no need to move anything with std::shared_ptr: it can be passed by value (i.e. can be copied). Just remember that its reference count increases when you do it


3) std::weak_ptr can be passed by value as well. Do it when the function needs to create a new std::shared_ptr out of it, which would increase the reference count:
```cpp
void f(std::weak_ptr<Object> wp)
{
  if (std::shared_ptr<Object> sp = wp.lock())
      sp->doSomething();
}
```

### Passing by reference to manipulate the ownership

Pass by reference when the function is supposed to modify the ownership of existing smart pointers. More specifically:
1) pass a non-const reference to std::unique_ptr if the function might modify it, e.g. delete it, make it refer to a different object and so on.

2) the same applies to std::shared_ptr, but you can pass a const reference if the function will only read from it (e.g. get the number of references) 
or it will make a local copy out of it and share ownership.

### Passing simple raw pointers/references

Go with a simpler raw pointer (can be null) or a reference (can't be null) when your function just needs to inspect the underlying object or do something with it without messing with the smart pointer. Both std::unique_ptr and std::shared_ptr have the get() member function that returns the stored pointer. For example:

```cpp
std::unique_ptr<Object> pu = std::make_unique<Object>();
function(*pu.get());  // Usage of (6)
function(pu.get());   // Usage of (7)
```
A std::weak_ptr must be converted to a std::shared_ptr first in order to take the stored pointer.

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

3) returning std::shared_ptr by reference doesn't properly increment the reference count, which opens up the risk of deleting something at the wrong time.

Thanks to point 2. you don't need move anything when returning a std::unique_ptr:
```cpp
std::unique_ptr<Object> getUnique()
{
    std::unique_ptr<Object> p = std::make_unique<Object>();
    return p; 
    // also return std::make_unique<Object>();
}
```

## reference_wrapper
List elements cannot be references, because references have to be initialized and cannot be reassigned, so the following won't compile:

```cpp
std::vector<const person&> m_people{};
```
Essentially, std::reference_wrapper is a class that acts like a reference, but also allows assignment and copying, so it’s compatible with lists like std::vector.

```cpp
std::vector<std::reference_wrapper<std::unique_ptr<person>>> people;
std::unique_ptr empolyee1= std::make_unique<person>();
people.push_back(empolyee1 );
```

Refs: 
        [1](https://www.internalpointers.com/post/move-smart-pointers-and-out-functions-modern-c),
        [2](https://www.acodersjourney.com/top-10-dumb-mistakes-avoid-c-11-smart-pointers/),
        [3](https://www.learncpp.com/cpp-tutorial/103-aggregation/),
        [4](https://www.nextptr.com/tutorial/ta1441164581/stdref-and-stdreference_wrapper-common-use-cases)
