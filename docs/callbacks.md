# Callbacks

A callback is a callable  accepted by a class or function, used to customize the current logic depending on that callback.


std::invoke is a generic way to activate any callable

std::invoke takes something callable, and arguments to call it with, and does the call. std::invoke( f, args... ) is a slight generalization of typing f(args...) that also handles a few additional cases.





1)Function pointers (including pointers to member functions)
2)std::function objects
3)Lambda expressions
4)Bind expressions
5)Function objects (classes with overloaded function call operator operator())



Refs: [1](https://stackoverflow.com/questions/2298242/callback-functions-in-c)




[source code](../src/callbacks.cpp)
