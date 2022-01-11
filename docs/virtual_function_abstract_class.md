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

Refs: [1](https://www.geeksforgeeks.org/virtual-function-cpp/), [2](https://stackoverflow.com/questions/39932391/should-i-use-virtual-override-or-both-keywords)


[source code](virtual_function_abstract_class.md)




