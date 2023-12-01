# virtual Destructor

### The Issue with Non-Virtual Destructors in Inheritance:

In C++, when you have a class hierarchy with base and derived classes, and you deallocate an object through a base class pointer, the destructor for the derived class is not called if the base class destructor is not virtual. This can lead to resource leaks and undefined behavior due to improper cleanup.

### Example:

Consider a simple class hierarchy:

```cpp
class Base {
public:
    Base() { /* Constructor code */ }
    ~Base() { /* Non-virtual destructor */ }
};

class Derived : public Base {
public:
    Derived() { /* Constructor code */ }
    ~Derived() { /* Destructor code */ }
};
```

Here, `Derived` is inheriting from `Base`. The destructor of `Base` is non-virtual.

Now, let's see the issue:

```cpp
Base* base = new Derived();
delete base;
```

In this case, only the destructor of `Base` is called, not the destructor of `Derived`. If `Derived` has allocated any dynamic resources or has other cleanup tasks, these won't be executed, leading to resource leaks or other issues.

### Solution - Virtual Destructor:

To resolve this, make the base class destructor virtual:

```cpp
class Base {
public:
    Base() { /* Constructor code */ }
    virtual ~Base() { /* Virtual destructor */ }
};

class Derived : public Base {
    // ...
};
```

With the virtual destructor in `Base`, when you delete an object of `Derived` through a pointer to `Base`, both destructors (for `Base` and `Derived`) will be called in the correct order, ensuring proper cleanup.

### Key Takeaways:

- Always declare destructors as virtual in base classes when you have a class hierarchy and you deallocate objects through base class pointers.
- This is crucial for proper resource management and avoiding memory leaks in C++ applications involving inheritance.




In the case of inheritance, the execution of constructors is as followings:

```cpp
constructor Base
constructor Derived

```
and the execution of destructor is as followings:

```cpp
Destructor Derived
Destructor Base
```

The following case has no problem:
```cpp
 Mamal mamal_obj;
``` 
or 

```cpp
 Mamal *mamal_ptr=new Mamal;
``` 
But in the folliwng case:

```cpp
Animal * obj=new Mamal;
delete obj;
```

The compiler will look at the type of the `obj` and according to what discussed in [Virtual Functions and VTABLE](VTABLE_and_VPTR.md), if a function is not virtual it will be linked via its type therefore we will only call the destructor of `Animal` and the destructor of `Mamal` won't be called.
Therefore we have to make both destructors virtual so the compiler pick the destructor based on the content of `obj` and picked the destructor of `Mamal` which eventually will call the destructor of `Animal` as well.


[code](../src/class/virtual_destructor_virtual_constructor.cpp) 
