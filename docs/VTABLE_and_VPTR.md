# Virtual Table and Late(Runtime) Binding 
Late binding (Runtime) is done in accordance with the content of pointer (i.e. location pointed to by pointer)
and Early binding(Compile time) is done according to the type of pointer, since print() function is declared
with virtual keyword so it will be bound at run-time.

## vtable
Every class that has virtual function will get a table name `vtable`. It is a  table of only "virtual function" memory addresses.  and it is maintained **per class**. `vtable`is created by compiler at compile time.
## vptr
when you create an object that which has virtual function, compiler adds a hidden member to your class called `vptr`. It is pointer to the vtable, (address of that vtable). It is maintained **per object instance**.
`vptr` is inherited to all the derived classes. If you create multiple from a class, the `vptr` in all of them  point to the same `vtable` of a particular class.
 


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
|   ____________________                                             ________________________________________________
|   |      Derived     |                                             |   vtable for class derived                    |
|   |  _vptr-----------|-------------------------------------------->|                                               |
|   |         foo()<---|---------------------------------------------|--address of derived implementation of foo()   |
|   |         print()  |                                      |------|--address of base implementation of func1()    |
|   |__________________|                                      |      |_______________________________________________|
|                                                             |
|-------------------------------------------------------------|
```


```cpp

base *bptr;
derived d;
bptr = &d;

//virtual function, binded at runtime, we got the foo() from derived
bptr->foo();

// Non-virtual function, binded at compile time, we get print() from base
bptr->print();

//we get print() from derived
d.print();

```


Refs: [1](https://www.geeksforgeeks.org/virtual-functions-and-runtime-polymorphism-in-c-set-1-introduction/), [2](https://www.geeksforgeeks.org/virtual-function-cpp/), [3](https://www.learncpp.com/cpp-tutorial/125-the-virtual-table/)

[source code](../src/VTABLE_and_VPTR.cpp)
