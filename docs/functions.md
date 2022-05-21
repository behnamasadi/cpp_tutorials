- [Extern Function](#extern-function)
- [Function Objects (Functors)](#function-objects--functors-)
- [Function Pointer](#function-pointer)
- [Inline Function](#inline-function)
- [Returning object from function](#returning-object-from-function)
  * [Returning a reference](#returning-a-reference)
  * [Returning a const reference](#returning-a-const-reference)
  * [Returning a reference to a non-const object](#returning-a-reference-to-a-non-const-object)
  * [Return smart pointers from functions](#return-smart-pointers-from-functions)
- [Passing smart pointers to functions](#passing-smart-pointers-to-functions)
- [Sending a function as parameter to an other function](#sending-a-function-as-parameter-to-an-other-function)



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




# Returning object from function
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


## Return smart pointers from functions
If you want to allocate something that lives beyond the scope of the function, use a smart pointer (or in general, a container):

```cpp
std::unique_ptr<int> getInt() 
{
    return std::make_unique<int>(0);
}
```
and the caller:

```cpp
std::unique_ptr<int> x = getInt();
```


Refs: [1](https://stackoverflow.com/questions/10643563/how-to-return-smart-pointers-shared-ptr-by-reference-or-by-value)


# Passing smart pointers to functions



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
  
