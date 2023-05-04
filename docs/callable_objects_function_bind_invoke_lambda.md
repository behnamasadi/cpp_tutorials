# Callable object
In C++, a callable object is an object that can be invoked like a function. Callable objects can be used in a variety of ways, including as function objects, function pointers, and lambdas.

There are several types of callable objects in C++, including:

1. Function pointers: These are pointers that point to functions. They can be used to call the function that they point to.

2. Function objects: These are objects that behave like functions. They implement the function call operator, operator(), and can be invoked like functions.

3. Lambda expressions: These are anonymous functions that can be defined inline. They can capture variables from their enclosing scope and can be used as function objects.

4. Member function pointers: These are pointers that point to member functions of a class. They can be used to call the member function that they point to.

5. Function references: These are references to functions. They can be used to call the function that they refer to.

Callable objects provide a lot of flexibility and can be used in many different situations, from implementing algorithms to passing functions as arguments to other functions.



#Partial application
In computer science, partial function application refers to the process of fixing a number of arguments to a function, 

The C++ standard library provides `bind(function, args..)` to return a function object that is the result of partial application of the given arguments to the given function. Alternatively, lambda expressions can be used.

## std::bind

Refs: [1](https://www.youtube.com/watch?v=ZlHi8txU4aQ), [2](https://stackoverflow.com/questions/6610046/stdfunction-and-stdbind-what-are-they-and-when-should-they-be-used), [3](https://en.wikipedia.org/wiki/Partial_application)

[code](../src/bind.cpp)
