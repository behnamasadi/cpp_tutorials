# Callbacks

A callback is a callable  accepted by a class or function, used to customize the current logic depending on that callback. `std::invoke` is a generic way to activate any callable. `std::invoke` takes something callable, and arguments to call it with, and does the call. `std::invoke( f, args... )` is a slight generalization of typing `f(args...)` that also handles a few additional cases.

## Function pointers (including pointers to member functions)
## std::function objects
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


Refs: [1](//https://www.youtube.com/watch?v=ZlHi8txU4aQ)
[code](../src/bind.cpp)
## Function objects (classes with overloaded function call operator operator())



Refs: [1](https://stackoverflow.com/questions/2298242/callback-functions-in-c)




[source code](../src/callbacks.cpp)
