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
