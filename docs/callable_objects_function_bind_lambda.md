
`std::invoke` is a generic way to activate any callable. `std::invoke` takes something callable, and arguments to call it with, and does the call. `std::invoke( f, args... )` is a slight generalization of typing `f(args...)` that also handles a few additional cases.



## std::bind

Refs: [1](https://www.youtube.com/watch?v=ZlHi8txU4aQ)

[code](../src/bind.cpp)
