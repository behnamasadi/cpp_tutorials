- [Callbacks](#callbacks)
  * [Function pointers (including pointers to member functions)](#function-pointers--including-pointers-to-member-functions-)
  * [std::function](#std--function)
  * [Lambda expressions](#lambda-expressions)
  * [Bind expressions](#bind-expressions)
  * [Function objects (classes with overloaded function call operator operator())](#function-objects--classes-with-overloaded-function-call-operator-operator---)
- [Sending a function as parameter to an other function](#sending-a-function-as-parameter-to-an-other-function)


# Callbacks

A callback is a callable  accepted by a class or function, used to customize the current logic depending on that callback. `std::invoke` is a generic way to activate any callable. `std::invoke` takes something callable, and arguments to call it with, and does the call. `std::invoke( f, args... )` is a slight generalization of typing `f(args...)` that also handles a few additional cases.

## Function pointers (including pointers to member functions)
## std::function 
Class template std::function is a general-purpose polymorphic function wrapper. Instances of std::function can store, copy, and invoke any Callable target:
1. functions
2. lambda expressions
3. bind expressions
4. functors

```cpp
void print(int value)
{
    std::cout<< value<<std::endl;
}
```

storing a functions:
```cpp
std::function<void(int)> funcPrint=print;
funcPrint(3);
```

store a bind expressions:

```cpp
std::function<void (int)> funcPrint=std::bind(&print,std::placeholders::_1);
std::function<void (int)> f(funcPrint);
f(3);
```

store lambda expressions:
```cpp
auto lambda=[](int value){std::cout<< value<<std::endl;};
std::function<void(int)>f(lambda);
f(3);
```

store a functors:
```cpp
void (*printfunctor)(int);
printfunctor=print;
std::function<void(int)>f(printfunctor);
f(3);
```

## Lambda expressions
## Bind expressions
`std::bind` works as a Functional Adaptor i.e. it takes a function as input and returns a new function Object as an output with with one or more of the arguments of passed function bound or rearranged (partial function application).

first example:
```cpp
void print(int firstParam, int secondParam)
{
	std::cout << "First Param is: " << firstParam << " Second Param is: " << secondParam << std::endl;
}
```
in the main:
```cpp
int firstParam=3;
int secondParam=5;
auto MyPrinter=std::bind(&print,firstParam,secondParam);
MyPrinter();
```
You can use `std::placeholders::_` to set the order of the parameters:

```cpp
auto reversePrintFunc = std::bind(&print, std::placeholders::_2, std::placeholders::_1);

int firstParam = 3;
int secondParam = 5;

print(firstParam, secondParam);
reversePrintFunc(firstParam, secondParam);

```
You can use it like a lambda expression:


```cpp
std::vector<int> vec={1,2,3};
std::vector<int> values(vec.size(),0);

// raise every value in vec to the power of 3
auto f=std::bind(&std::pow<int,int>,std::placeholders::_1,3);
std::transform(vec.begin(), vec.end(), values.begin(), f);
        
for(auto value:values)
    std::cout<< value<<std::endl;
```
an other example:

```cpp
int param=3;
auto printer=std::bind(&printTemplate<int>,param);
```
or
```cpp
auto printer=std::bind(&printTemplate<int>,std::placeholders::_1);
```
an calling it:
```cpp
std::function<void (int)>f(printer);
f(param);
```


Refs: [1](//https://www.youtube.com/watch?v=ZlHi8txU4aQ)

[code](../src/bind.cpp)
## Function objects (classes with overloaded function call operator operator())



Refs: [1](https://stackoverflow.com/questions/2298242/callback-functions-in-c), [2](https://stackoverflow.com/questions/6610046/stdfunction-and-stdbind-what-are-they-and-when-should-they-be-used), [2](https://en.wikipedia.org/wiki/Partial_application)
[source code](../src/callbacks.cpp)


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
