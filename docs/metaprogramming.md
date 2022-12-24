# Metaprogramming in C++


Metaprogramming in C++ is a programming technique in which a program generates or manipulates source code at compile-time, rather than during runtime. It involves writing code that manipulates or generates other code, rather than just executing instructions directly.

One way to achieve metaprogramming in C++ is through the use of template metaprogramming, which is a form of compile-time computation that uses templates and type information to generate code. Template metaprogramming allows you to write code that is executed at compile-time, rather than at runtime, by using templates and the type system to perform operations and generate code.

Here is an example of template metaprogramming in C++:

```cpp
template<int N>
struct Fibonacci {
    static constexpr int value = Fibonacci<N-1>::value + Fibonacci<N-2>::value;
};

template<>
struct Fibonacci<0> {
    static constexpr int value = 0;
};

template<>
struct Fibonacci<1> {
    static constexpr int value = 1;
};

int main() {
    std::cout << Fibonacci<10>::value << std::endl;
    return 0;
}
```

In this example, the `Fibonacci` template is a recursive template that calculates the nth Fibonacci number at compile-time. The `Fibonacci<10>::value` expression will be evaluated at compile-time, and the result will be printed at runtime.

Metaprogramming can be a powerful tool for optimizing code and writing generic, reusable code, but it can also be difficult to understand and debug. As with any advanced programming technique, it should be used with caution and only when necessary.

Refs: [1](https://en.cppreference.com/w/cpp/meta)
[source code](../src/metaprogramming.cpp)
