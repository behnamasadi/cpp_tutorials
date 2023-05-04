## std::invoke
`std::invoke` is a generic way to activate any callable. `std::invoke` takes something callable, and arguments to call it with, and does the call. `std::invoke( f, args... )` is a slight generalization of typing `f(args...)` that also handles a few additional cases.

The advantage of using std::invoke over directly invoking a callable object is that it provides a uniform syntax that works for all callable objects. This means that you can use the same syntax to invoke a function pointer, member function, function object, or lambda, without needing to know the specific type or signature of the callable object.



```cpp
void foo(int x) {
    std::cout << "foo(" << x << ")" << std::endl;
}

struct bar {
    void operator()(int x) const {
        std::cout << "bar(" << x << ")" << std::endl;
    }
};
```
now you can invoke them as followings:


1. Invoke a function pointer:
```cpp
int x = 42;
std::invoke(foo, x);
```

2. Invoke a member function:
```cpp
bar b;
std::invoke(&bar::operator(), b, x);
```

3. Invoke a function object:
```cpp
std::invoke(std::function<void(int)>([](int x) { std::cout << "lambda(" << x << ")" << std::endl; }), x);
````
