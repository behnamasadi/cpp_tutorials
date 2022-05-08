# Memory Layout of Simple Classes/ Structs

The term layout refers to how the members of an object of class, struct or union type are arranged in memory. In some cases, the layout is well-defined by the language specification. But when a class or struct contains certain C++ language features such as virtual base classes, virtual functions, members with different access control, then the compiler is free to choose a layout.

That layout may vary depending on what optimizations are being performed and in many cases the object might not even occupy a contiguous area of memory.

For example, if a class has virtual functions, all the instances of that class might share a single [virtual function table](VTABLE_and_VPTR.md)


Because the layout is undefined they cannot be passed to programs written in other languages, such as C, and because they might be non-contiguous they cannot be reliably copied with fast low-level functions such as memcopy, or serialized over a network.


C++14 introduced three categories of **simple classes/ structs**: 
- trivial
- standard-layout
- POD (Plain Old Data). 

To determine whether a given type belongs to a given category the Standard Library has the following function templates
- is_trivial<T>
- is_standard_layout<T> 
- is_pod<T> 


## Trivial types
  
When a class or `struct` in C++ has compiler-provided or **explicitly defaulted special member functions**, then it is a trivial type. It occupies a contiguous memory area. It can have members with different access specifiers. In C++, the compiler is free to choose how to order members in this situation. Therefore, you can `memcopy` such objects but you cannot reliably consume them from a C program. A trivial type `T` can be copied into an array of char or unsigned char, and safely copied back into a `T` variable.  
  
Trivial types have a trivial default constructor, trivial copy constructor, trivial copy assignment operator and trivial destructor. In each case, trivial means the **constructor/operator/destructor** is NOT user-provided and belongs to a class that has  
  
In `Foo`, the presence of the `Foo(int a, int b)` constructor requires that you provide a **default constructor**. For the type to qualify as trivial, you must explicitly default that constructor. Please note that the member variables have different access control (public and private mixed)

  
```cpp
struct Foo
{
   int i;
   Foo(int a, int b) : i(a), j(b) {}
   Foo() = default;
private:
   int j;   
};  
```  

  
## Standard layout types  
When a class or struct does not contain certain C++ language features such as virtual functions which are not found in the C language, and all members have the same access control, it is a standard-layout type.  It is memcopy-able and the layout is sufficiently defined that it can be consumed by C programs. Standard-layout types can have user-defined [special member functions](class_special_member_functions.md).
  
In the following example all members have same access and we have user-defined constructor (`std::is_standard_layout<Foo>` is true).
```cpp
struct Foo
{
   
   int i;
   int j;
   Foo(int a, int b) : i(a), j(b) {} 
};  
```  
## POD types
  
When a class or struct is both trivial and standard-layout, it is a POD (Plain Old Data) type. The memory layout of POD types is therefore contiguous and each member has a higher address than the member that was declared before it, so that byte for byte copies and binary I/O can be performed on these types.  
  
  
  
```cpp
#include <type_traits>
#include <iostream>

using namespace std;

struct B
{
protected:
   virtual void Foo() {}
};

// Neither trivial nor standard-layout
struct A : B
{
   int a;
   int b;
   void Foo() override {} // Virtual function
};

// Trivial but not standard-layout
struct C
{
   int a;
private:
   int b;   // Different access control
};

// Standard-layout but not trivial
struct D
{
   int a;
   int b;
   D() {} //User-defined constructor
};

struct POD
{
   int a;
   int b;
};

int main()
{
   cout << boolalpha;
   cout << "A is trivial is " << is_trivial<A>() << endl; // false
   cout << "A is standard-layout is " << is_standard_layout<A>() << endl;  // false

   cout << "C is trivial is " << is_trivial<C>() << endl; // true
   cout << "C is standard-layout is " << is_standard_layout<C>() << endl;  // false

   cout << "D is trivial is " << is_trivial<D>() << endl;  // false
   cout << "D is standard-layout is " << is_standard_layout<D>() << endl; // true

   cout << "POD is trivial is " << is_trivial<POD>() << endl; // true
   cout << "POD is standard-layout is " << is_standard_layout<POD>() << endl; // true

   return 0;
}  
  
```  
  
Refs: [1](https://docs.microsoft.com/en-us/cpp/cpp/trivial-standard-layout-and-pod-types?view=msvc-170)
