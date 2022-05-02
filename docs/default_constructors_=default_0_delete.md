# Default constructors
A default constructor is a constructor that 
1. Has no parameters:


2. If it has parameters, all the parameters have default values.


```cpp

class Foo {
public:
  Foo();                           // user-defined default constructor with no arguments
  Foo(int n= 0);                   // user-define default constructor with one default argument
  Foo(int a, int b, int v= 0);     // Constructor
};
```
A default constructor is a constructor which can be called with no arguments. If no user-defined constructor exists for a class `Foo` and one is needed, the compiler implicitly declares a default parameterless constructor Foo::Foo(). This constructor is an inline public member of its class. The compiler will implicitly define `Foo::Foo()` when the compiler uses this constructor to create an object of type `Foo`. The constructor will have no constructor initializer and a null body.  No default constructor is created for a class that has any constant or reference type members. 




```cpp
struct A
{
    int x;
    A(int x = 1): x(x) {} // user-defined default constructor
};
 
struct B: A
{
    // B::B() is implicitly-defined, calls A::A()
};
 
struct C
{
    A a;
    // C::C() is implicitly-defined, calls A::A()
};
 
struct D: A
{
    D(int y): A(y) {}
    // D::D() is not declared because another constructor exists
};
 
struct E: A
{
    E(int y): A(y) {}
    E() = default; // explicitly defaulted, calls A::A()
};
 
struct F
{
    int& ref; // reference member
    const int c; // const member
    // F::F() is implicitly defined as deleted
};
 
// user declared copy constructor (either user-provided, deleted or defaulted)
// prevents the implicit generation of a default constructor
 
struct G
{
    G(const G&) {}
    // G::G() is implicitly defined as deleted
};
 
struct H
{
    H(const H&) = delete;
    // H::H() is implicitly defined as deleted
};
 
struct I
{
    I(const I&) = default;
    // I::I() is implicitly defined as deleted
};
 
int main()
{
    A a;
    B b;
    C c;
//  D d; // compile error
    E e;
//  F f; // compile error
//  G g; // compile error
//  H h; // compile error
//  I i; // compile error
}
```



Refs: [1](https://www.ibm.com/docs/en/zos/2.2.0?topic=only-default-constructors-c), [2](https://en.cppreference.com/w/cpp/language/default_constructor)


[code](../src/class/default_0_delete_meaning.cpp)
