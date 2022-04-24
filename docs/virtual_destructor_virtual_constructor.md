# virtual Destructor
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
