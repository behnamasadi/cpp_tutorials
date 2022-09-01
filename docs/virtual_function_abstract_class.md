# Virtual Functions

1) Virtual functions are always defined in base class.**Declaration is not enough**, you have to define them as well in the base and they
(may or may not) overridden in derived class. The original base class declaration needs the keyword **virtual** to mark it as virtual.
 In the derived class the function is virtual by way of having the same type as the base class function.

These are exactly the same. The first one requires more typing and is potentially clearer.

```cpp
struct A {
    virtual void hello();
};

struct B : public A {
    virtual void hello() { ... }
};


struct B : public A {
    void hello() { ... }
};
```


2) An override can help avoid bugs by producing a compilation error when the intended override isn't technically an override. For instance,
the function type isn't exactly like the base class function. Or that a maintenance of the base class changes that function's type,
e.g. adding a defaulted argument.

3) It is **not** mandatory for derived class to override (or re-define the virtual function), in that case base class version of function is used.

4) A class may have virtual destructor but it cannot have a virtual constructor.

5) A pure virtual function or pure virtual method is a virtual function that is required to be implemented by a derived class,
if that class is not abstract.

6) Each function declaration should specify exactly one of
- virtual
- override
- final

**virtual**: For the "first" appearance of a function in the base class.

**override**: For overrides of that virtual function in a class derived from some base class providing a virtual function of the same (or covariant) signature.

**final**: For marking an override as unoverrideable. That is, derivatives of a class with a final virtual function override cannot have that virtual function override overridden.



# Making a class abstract without any pure virtual methods

Declare a pure virtual destructor, but give it a definition in the .cpp file. The class will be abstract, but any inheriting classes will not by default be abstract.


```cpp
class Abstract
{
public:
     virtual ~Abstract() = 0;
};
```
in cpp file:
```cpp
Abstract::~Abstract() {}
```

```cpp
class derived: public Abstract
{
        // Notice you don't need to actually overide the base
        // classes pure virtual method as it has a default
};
```
now in your application:
```
int main()
{
    // Abstract        a;  // This line fails to compile as Abstract is abstract
    Valid           v;  // This compiles fine.
}
```

Refs: [1](https://www.geeksforgeeks.org/virtual-function-cpp/), [2](https://stackoverflow.com/questions/39932391/should-i-use-virtual-override-or-both-keywords)


# Virtual Friend Function Idiom
Refs: [1](https://stackoverflow.com/questions/12142893/virtual-friend-functions-for-a-base-class)

[source code](../src/class/virtual_function_abstract_class.cpp)




