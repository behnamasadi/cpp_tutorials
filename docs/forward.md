# std::forward

`std::forward` has a single use case: to cast a templated function parameter (inside the function)
to the value category (lvalue or rvalue) the caller used to pass it.

`std::move` takes an object and allows you to treat it as a temporary (an rvalue). Although it isn’t a
semantic requirement, typically a function accepting a reference to an rvalue will invalidate it.
When you see std::move, it indicates that the value of the object should not be used afterwards,
but you can still assign a new value and continue using it.

Let's say you have the following:
```cpp
struct S { };

void f(S& s)
{
    std::cout<<"S&" <<std::endl;
}

void f(S&& s)
{
    std::cout<<"S&&" <<std::endl;
}

template< typename T>
void wrap(T t)
{
    f(t);
}

```
In your main, this will call lvalue function of `f()`

```
S s;
f(s);
```

This will call rvalue function of `f()`

```cpp
f(S());
```
now if we have wrapper class for `f()` and we call the following we get lvalue which is wrong
```cpp
wrap(S());
```

so we have to change the warp function to the following to forward to the correct value category:

```cpp
template< typename T>
void wrap_forward(T t)
{
    f(std::forward<T>(t));
}
```

if we use forward we will get the rvalue `f()` which is the correct one:
```cpp
wrap_forward(S());
```


Refs: [1](https://stackoverflow.com/questions/3582001/what-are-the-main-purposes-of-using-stdforward-and-which-problems-it-solves), [2](https://www.youtube.com/watch?v=0xcCNnWEMgs), [3](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2002/n1385.htm)

