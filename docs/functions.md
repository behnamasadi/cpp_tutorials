

# Guideline for declaring functions
## using constexpr
Don’t make all functions `constexpr` since most of the computation done at run time but if a function might have to be evaluated at compile time, declare it `constexpr`.


```cpp
constexpr long int fib(int n)
{
    constexpr int max_exp = 17;      // constexpr enables max_exp to be used in Expects
    Expects(0 <= n && n < max_exp);  // prevent silliness and overflow
    return (n <= 1)? n : fib(n-1) + fib(n-2);
}
```  
if  call it like:
```cpp
const long int res = fib(30);
```
[more about constexpr](const_constexpr_mutable.md)

## Declaring functions as inline
If a function tiny and time-critical, it is better to declare it as inline. [more about inline](#inline-function)

## Declare function as noexcept

# Guideline for parameter passing to functions

## Take `T*` or `T&` arguments rather than smart pointers


# Passing/ returning smart pointers from/to functions
Please read full article [here](passing_returning_smart_pointers_to_from_functions.md)

# Guideline for value return from functions

Refs: [1](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-functions)

# Returning from function
##  Returning a reference
it's okay to return a reference if the lifetime of the object won't end after the call but never retrun a reference to a local object. Also okay for accessing things where you know the lifetime is being kept open on a higher-level, e.g.:
```cpp
struct S {
    S(int i) : m_i(i) {}

    const int& get() const { return m_i; }
private:
    int m_i;
};
```
Here we know it's okay to return a reference to `m_i` because whatever is calling us manages the lifetime of the class instance, so `m_i` will live at least that long.


When you return a value, a copy is made. The scope of the local variable ends, but a copy is made.
when a function is called, a temporary space is made for the function to put its local variables, called a frame. When the function (callee) returns its value, it puts the return value in the frame of the function that called it (caller), and then the callee frame is destroyed.

The "frame is destroyed" part is why you can't return pointers or references to local variables from functions. A pointer is effectively a memory location, so returning the memory location of a local variable (by definition: a variable within the frame) becomes incorrect once the frame is destroyed. Since the callee frame is destroyed as soon as it returns its value, any pointer or reference to a local variable is immediately incorrect.


The stack-allocated `i` will go away and you are referring to nothing.
```cpp
int& getInt() 
{
    int i;
    return i;  
}
```
even if you allocate it on the heap, the caller has to free the space:
```cpp
int& getInt() 
{
    int* i = new int;
    return *i;
}
```

delete would be totally weird and evil
```cpp
int& myInt = getInt();
delete &myInt;
```


 undefined behavior, we're wrongly deleting a copy, not the original
 ```cpp
int oops = getInt(); 
delete &oops;
```


This is very bad and the result is undefined
```cpp
int *myBadAddingFunction(int a, int b)
{
    int result;

    result = a + b;
    return &result; 
}
```

Also allocated on the stack, also bad
```cpp
char *myOtherBadFunction()
{
    char myString[256];

    strcpy(myString, "This is my string!");
    return myString;
}
```

Refs: [1](https://stackoverflow.com/questions/275214/scope-and-return-values-in-c), [2](https://stackoverflow.com/questions/752658/is-the-practice-of-returning-a-c-reference-variable-evil)




## Returning a reference to a `std::vector`
Returning a reference to a `std::vector` (or any other local object) that is defined within a function is not a good practice in C++ because it leads to undefined behavior. When a function returns, all its local variables (those defined inside the function) are destroyed. If you return a reference to such a local variable, you end up with a reference to a destroyed (or "dangling") object. Accessing this reference later in your program can cause crashes or unpredictable behavior.

Here's what typically happens when you try to return a local object by reference:

```cpp
#include <vector>

std::vector<int>& dangerousFunction() {
    std::vector<int> localVector = {1, 2, 3};
    return localVector; // Dangerous: localVector is destroyed when the function exits
}

int main() {
    std::vector<int>& myVectorRef = dangerousFunction(); // myVectorRef is now a dangling reference
    // Using myVectorRef here is unsafe and leads to undefined behavior
}
```

In the code above, `dangerousFunction` creates a local `std::vector` and returns a reference to it. However, as soon as `dangerousFunction` exits, `localVector` is destroyed, and the reference returned becomes invalid.

### Safe Alternatives


**Use Smart Pointers**

The following code is safe, but it's not as efficient as it could be. Here's the breakdown of the function:

```cpp
std::unique_ptr<std::vector<int>> foo() {
    std::vector<int> localVector;
    localVector.push_back(2);
    localVector.push_back(4);
    localVector.push_back(5);

    return std::make_unique<std::vector<int>>(localVector);
}
```

In this function:
1. You create a local `std::vector<int>` named `localVector`.
2. You populate `localVector` with some integers.
3. Then, you create a `std::unique_ptr<std::vector<int>>` that points to a new `std::vector<int>` which is constructed using the copy constructor of `std::vector<int>`. This means you are copying `localVector` into a new `std::vector<int>` that is managed by the `std::unique_ptr`.

The code is safe in the sense that it won't lead to undefined behavior or memory leaks. The `std::unique_ptr` will manage the memory of the new `std::vector<int>` created on the heap, and this memory will be properly deallocated when the `std::unique_ptr` goes out of scope or is otherwise destroyed.

However, the inefficiency lies in the fact that you are making a copy of `localVector` when you create the `std::unique_ptr`. This is an unnecessary operation, as you could directly allocate the vector on the heap and avoid the copy. A more efficient approach would be to directly create the vector in the heap:

```cpp
std::unique_ptr<std::vector<int>> foo() {
    auto ptr = std::make_unique<std::vector<int>>();
    ptr->push_back(2);
    ptr->push_back(4);
    ptr->push_back(5);

    return ptr;
}
```

In this revised version, you avoid the extra copy by directly manipulating the vector on the heap. This is generally a better practice when dealing with unique pointers and dynamically allocated objects.




## Returning a std::vector

In C++, when a function returns a `std::vector`, the behavior regarding copying depends on the context and the version of the C++ standard you are using.

1. **C++98 and C++03 (Before Move Semantics):** In these older versions of C++, returning a `std::vector` from a function would typically result in a copy of the vector being made. This is because move semantics were not yet introduced, so the copy constructor would be invoked to create a copy of the vector when returning from the function.

2. **C++11 and Later (With Move Semantics):** Starting with C++11, move semantics were introduced. This means that when you return a `std::vector` from a function, the compiler can utilize the move constructor instead of the copy constructor. The move constructor is more efficient because it transfers the internal data from the original vector to the new one, avoiding the need to copy all the elements. This operation is called Return Value Optimization (RVO) or copy elision.

   - **Automatic Move if RVO Doesn't Apply:** Even if RVO is not applied by the compiler, the vector will still be moved rather than copied, provided that the vector being returned is a local object and the move constructor is available (which it is for `std::vector`).

   - **Named Return Value Optimization (NRVO):** There's also a specific case called NRVO, where the compiler can optimize the return of a named local variable. This optimization depends on the compiler and the specific scenario.

3. **Lambdas and Captured Vectors:** If a lambda function captures a vector and the lambda is returned, it will carry its captured context along with it, which might involve copying the vector, depending on how the lambda captures it (by value or by reference).

4. **Function Parameters and Return-by-Reference:** If the function's purpose is to modify a vector passed by reference and then return it, there is no copying involved, as the function operates on the original vector.

In summary, whether a `std::vector` is copied when returned from a function in C++ depends on the version of C++ and the specific circumstances. With modern C++ (C++11 and later), move semantics usually prevent unnecessary copies, making the operation more efficient.



## Returning a const reference

```cpp
class Complx {
private:
	double real;
	double imag;
public:
	Complx() {}
	Complx(double r, double i): real(r), imag(i) {}
	Complx  operator+(const Complx & c) const 
    {
		return Complx(real + c.real, imag + c.imag);
	}
	Complx & operator=(const Complx &c;) 
    {
		real = c.real;
		imag = c.imag;
		return *this;
	}

	double size() const 
    {
		return sqrt(real*real + imag*imag);
	}
};
```

Here in the Maximum function,
```cpp
const Complx & Maximum(const Complx &c1, const Complx &c2) 
{
	
	if (c1.size() > c2.size()) 
		return c1;
	else
		return c2;
}
```
Returning an object invokes the **copy constructor** while returning a reference doesn't. Also, the reference should be to an object that exists when the calling function is execution which in this case both of them are class members so they will exist as long as the class exist.

Refs: [1](https://www.bogotobogo.com/cplusplus/object_returning.php)


## Returning a reference to a non-const object





Refs: [1](https://stackoverflow.com/questions/10643563/how-to-return-smart-pointers-shared-ptr-by-reference-or-by-value)


# Sending a function as parameter to an other function
Here in this case planner might use various solver for planning:

```cpp
int planner( int(*fn_solver)(int,int))
{
    return fn_solver(10,12);
}

int solver1(int a,int b)
{
    return a+b;
}

int solver2(int a,int b)
{
    return a-b;
}
```

Sending `solver1` or `solver2`:
    
```cpp
std::cout<< "solver1"<<std::endl;
planner( solver1);
std::cout<< "solver2"<<std::endl;
planner( solver2);
```

function pointer
```cpp
auto func_ptr1=std::bind( &solver1, std::placeholders::_1, std::placeholders::_2);
std::function<int (int,int)> func_ptr2=std::bind( &solver1, std::placeholders::_1, std::placeholders::_2);

std::cout<< func_ptr1(1,2)<<std::endl;
std::cout<< func_ptr2(1,2)<<std::endl;
```

# Function Objects (Functors)
Function objects (aka "functors"), Functors are objects that can be treated as they are a function or function pointer. You could write code that looks like this:

```cpp
Foo foo_obj( 5 );
std::cout << foo_obj( 6 )<<std::endl;
```

This code works because C++ allows you to overload `operator()`, the "function call" operator. 


Refs: [1](http://www.cprogramming.com/tutorial/functors-function-objects-in-c++.html)

```cpp
class Foo
{
    public:
        Foo (int x) : m_x( x ) {}
        int operator() (int y) { return m_x + y; }
    private:
        int m_x;
};
```
This will call the constructor:
```cpp
Foo foo_obj( 5 );
```
This calls the `operator()`:
```cpp
std::cout << foo_obj( 6 )<<std::endl;
```
# Function Pointer
A function pointer is a variable that stores the address of a function that can later be called through that function pointer.

```cpp
int adder(int a, int b)
{
    return a+b;
}
```

now in your code you can have the following variable `adder_fn_ptr`:

```cpp
int (*adder_fn_ptr)(int, int);
```
binding:

```cpp
adder_fn_ptr=adder; 
```
or
```cpp 
adder_fn_ptr=&adder
```

calling:

```cpp
std::cout<<adder_fn_ptr(2,3) <<std::endl;
```
or
```cpp
std::cout<< (*adder_fn_ptr) (2,3) <<std::endl;
```

Another example

```cpp
void foreach(std::vector<int> values, void(UnaryFunc)(int))
{
    for(auto value:values)
        UnaryFunc(value);
} 
```
some unary functions:

```cpp 
void print(int i)
{
    std::cout<<i <<std::endl;
}
```

Now we can have the followings:

```cpp
std::vector<int> values={1,2,3,4};
foreach(values,print);
```    
or

```cpp        
std::vector<int> values={1,2,3,4};
foreach(values,[](int value){std::cout<<value <<std::endl;});
```


# Inline Function

Calling a function has lot of overhead, calling, copying arguments, push/pop on the stack, and return. Inline function will be added to your code so there is no call and return. The `inline` keyword is a hint to the compiler and it is up to the compiler to decide weather to inline a function or not




What to consider:

- The function should be very small and is called very often
- Use inline function over **macros**.


Refs: [1](http://www.cplusplus.com/articles/2LywvCM9/), [2](https://stackoverflow.com/questions/1932311/when-to-use-inline-function-and-when-not-to-use-it), [3](https://www.mygreatlearning.com/blog/inline-functions-in-cpp/#:~:text=Inline%20functions%20are%20commonly%20used,definition%20of%20the%20called%20function.)


```cpp
class foo
{
 public:
    int add(int a, int b);
};

inline int foo::add(int a, int b)
{
   return (a + b);
}
```

[code](inline_functions.cpp)


# Extern Function

C++ mangle the name of functions so it will be able to overload function, while in "c", there is no overloading and function will have the same name in object file as what they had in their
respective "c" file, so if we want to compile a "c" function in c++, we have to use it extern 

in your `foo.h` you have 
```cpp
void foo();
```

in your `foo.cpp` you have 

```cpp
#include "foo.h"

void foo()
{
	printf("foo here");
}
```

in your application:

```cpp
extern  "C"
{
#include "foo.h"
}

int main()
{
    foo();
}
```
[code](../src/extern/function)  
