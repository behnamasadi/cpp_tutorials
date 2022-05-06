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
