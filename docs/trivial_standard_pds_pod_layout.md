# Memory Layout of Simple Classes/ Structs

Understanding the memory layout of simple classes and structs, particularly in C++, involves delving into concepts like Trivial types, Standard layout types, and Plain Old Data (POD) types. These concepts are crucial for understanding how objects are stored in memory, which can impact performance and compatibility, especially in low-level programming or interfacing with other languages like C.

### 1. Trivial Types
- **Definition**: A trivial type in C++ is a type that has a trivial default constructor, trivial copy constructor, trivial copy assignment operator, and trivial destructor. This means these operations do nothing special: they simply copy bytes without considering content.
- **Memory Layout**: 
  - Objects of trivial types are laid out in memory just like plain old data (like integers or floating-point numbers).
  - There is no dynamic memory allocation or complex construction/destruction logic.
  - They can be safely copied using `std::memcpy` or similar functions, maintaining the bit-exact copy of the object.

### 2. Standard Layout Types
- **Definition**: A standard layout type in C++ is a type that has no virtual functions or virtual base classes, and all its non-static data members, including any in its inherited classes, have the same access control (public, private, protected).
- **Memory Layout**:
  - The memory layout is contiguous and consistent.
  - Members are laid out in the order they are declared, but the compiler may insert padding for alignment purposes.
  - Good for interfacing with hardware or other programming languages, as the memory layout is predictable.

### 3. Plain Old Data (POD) Types
- **Definition**: A POD type is both a trivial and standard layout type. It is the most restrictive and simple class/struct type in C++, resembling traditional C-style structs.
- **Memory Layout**:
  - POD types can be thought of as a direct map to a contiguous block of memory.
  - They can be copied and serialized easily (e.g., written to a file or sent over a network) because they have no indirections or complex internal structures.
  - They are highly compatible with C structs, making them ideal for systems programming or C/C++ interoperability.

### Key Points
- **Compatibility**: Trivial, standard layout, and POD types offer different levels of compatibility and simplicity, with POD being the most straightforward.
- **Performance**: Since these types avoid complex memory layouts, they often lead to better performance due to more predictable memory access patterns.
- **Interoperability**: They are particularly important in contexts where data needs to be shared with hardware or other programming languages, as their simple layout ensures that the data structure will be interpreted correctly across different systems.

### Summary
In essence, these types represent different levels of simplicity and predictability in how objects are laid out in memory. Trivial types are simple to copy, standard layout types have predictable layouts, and POD types combine these features, resembling basic C structs. Understanding these types is key in scenarios that require direct memory manipulation, performance optimization, and cross-language compatibility.


The term layout refers to how the members of an object of class, struct or union type are arranged in memory. In some cases, the layout is well-defined by the language specification. But when a class or struct contains certain C++ language features such as virtual base classes, virtual functions, members with different access control, then the compiler is free to choose a layout.

That layout may vary depending on what optimizations are being performed and in many cases the object might not even occupy a contiguous area of memory.

For example, if a class has virtual functions, all the instances of that class might share a single [virtual function table](VTABLE_and_VPTR.md)


Because the layout is undefined they cannot be passed to programs written in other languages, such as C, and because they might be non-contiguous they cannot be reliably copied with fast low-level functions such as memcopy, or serialized over a network.


Yes, beyond trivial types, standard layout types, and Plain Old Data (POD) types, there are other kinds of class layouts in C++, especially when you start dealing with non-trivial and non-standard layouts. These include classes with virtual functions, multiple inheritance, and complex member objects. Here are examples for each type:

### 1. Trivial Type
A trivial type has a trivial default constructor, copy constructor, copy assignment operator, and destructor. It does nothing special; it's as simple as a basic data type.

```cpp
struct TrivialType {
    int a;
    float b;
};
```
In this example, `TrivialType` is trivial because it only contains basic data types and does not define any special member functions or constructors.

### 2. Standard Layout Type
A standard layout type is one that doesn't have virtual functions or virtual base classes, and all non-static members have the same access control.

```cpp
class StandardLayoutType {
public:
    int x;
    char y;
};
```
Here, `StandardLayoutType` is a standard layout type because it has no virtual functions or mixed access members.

### 3. Plain Old Data (POD) Type
A POD type is both trivial and standard layout. It's like a struct in C.

```cpp
struct PODType {
    int x;
    double y;
};
```
`PODType` is a POD because it's both trivial and has a standard layout.

### 4. Non-Trivial Type
A non-trivial type has custom behavior defined in its constructor, destructor, or copy operations.

```cpp
struct NonTrivialType {
    NonTrivialType() {
        // Custom constructor logic
    }

    ~NonTrivialType() {
        // Custom destructor logic
    }
};
```
`NonTrivialType` is non-trivial due to its custom constructor and destructor.

### 5. Non-Standard Layout Type
A non-standard layout type might have mixed access members, virtual functions, or virtual inheritance.

```cpp
class NonStandardLayoutType {
private:
    int a;
public:
    double b;
    virtual void doSomething() {}
};
```
`NonStandardLayoutType` is non-standard due to its virtual function and mixed access members.

### 6. Complex Class Layout (Multiple Inheritance, Virtual Functions)
Complex class layouts can involve multiple inheritance, virtual functions, and complex member objects.

```cpp
class Base1 { virtual void foo() {}; };
class Base2 { int b; };
class ComplexType : public Base1, public Base2 {
    std::string name;
    virtual void foo() override {}
};
```
`ComplexType` is an example of a complex class layout with multiple inheritance and virtual functions.

### Summary
Understanding these various layouts is crucial in C++ as they impact how objects are stored and accessed in memory. Trivial and standard layouts are simpler and more predictable, while non-trivial and non-standard layouts offer more flexibility at the cost of complexity. Complex class layouts with features like multiple inheritance and virtual functions are powerful but can lead to less predictable memory layouts and performance characteristics.



------------------------------------------------------

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
