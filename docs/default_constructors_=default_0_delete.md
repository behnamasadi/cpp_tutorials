# Default Constructors
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


# Explicitly Defaulted and Deleted Special Member Functions

C++ compiler automatically generates the **default constructor**, **copy constructor**, **copy-assignment** operator, and **destructor**  if it does not declared. These functions are known as the **special member functions**. That's enable you to create, copy, and destroy objects without any additional code. 
C++11 introduced **move semantics** to the language and added the **move constructor** and **move-assignment** operator to the list of special member functions that the compiler can automatically generate.




You can make any of class  member functions to use the default implementation (automatic generation).

Because of the performance benefits of trivial special member functions, we recommend that you prefer automatically generated special member functions over empty function bodies when you want the default behavior. 

Refs: [1](https://docs.microsoft.com/en-us/cpp/cpp/explicitly-defaulted-and-deleted-functions?view=msvc-170)


## =default

In `Foo`,  For the type to qualify as trivial the presence of the `Foo(int a, int b)` constructor requires that you provide a default constructor., you must explicitly default that constructor.


```cpp
struct noncopyable
{
  noncopyable() =default;
  noncopyable(const noncopyable&) =delete;
  noncopyable& operator=(const noncopyable&) =delete;
};
```


## =delete
Deleted functions enale you to prevent problematic type promotions from occurring.


## = default vs empty user defined function {}

Refs: [1](https://stackoverflow.com/questions/20828907/the-new-syntax-default-in-c11#:~:text=If%20you%20want%20your%20class,you%20need%20to%20use%20%3D%20default%20.), [2](https://stackoverflow.com/questions/6502828/what-does-default-mean-after-a-class-function-declaration)
