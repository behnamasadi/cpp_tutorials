# Virtual Table and Late(Runtime) Binding 
Late binding (Runtime) is done in accordance with the content of pointer (i.e. location pointed to by pointer)
and Early binding(Compile time) is done according to the type of pointer, since print() function is declared
with virtual keyword so it will be bound at run-time.

## vtable
A table of only "virtual function" pointers, maintained **per class**.

## vptr
A pointer to vtable, maintained **per object instance**. When an object is created, a pointer to vtable,
called the virtual table pointer, vpointer or VPTR, is added as a hidden member of this object.
As such, the compiler must also generate "hidden" code in the constructors of each class to initialize
a new object's virtual table pointer to the address of its class's virtual method table.


```

   ____________________                                              _____________________________________
   |      Base         |                                             |   vtable for class base            |
   |  _vptr------------|-------------------------------------------->|                                    |
   | virtual foo()<--- |---------------------------------------------|--address of base version of foo()  |
---|->virtual func1()<-|---------------------------------------------|--address of base version of func1()|
|  |___________________|                                             |____________________________________|
|
|
|
|
|
|   ____________________                                             ______________________________________
|   |      Derived     |                                             |   vtable for class derived          |
|   |  _vptr-----------|-------------------------------------------->|                                     |
|   |         foo()<---|---------------------------------------------|--address of derived version of foo()|
|   |                  |                                      |------|--address of base version of func1() |
|   |__________________|                                      |      |_____________________________________|
|                                                             |
|-------------------------------------------------------------|
```


Refs: [1](https://www.geeksforgeeks.org/virtual-functions-and-runtime-polymorphism-in-c-set-1-introduction/), [2](https://www.geeksforgeeks.org/virtual-function-cpp/), [3](https://www.learncpp.com/cpp-tutorial/125-the-virtual-table/)

[source code](VTABLE_and_VPTR.cpp)
