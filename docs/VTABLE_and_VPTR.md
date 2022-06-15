# Virtual Method Table and Virtual Table Pointer

As you might know, we have two types of polymorphism in C++:
1. Static Polymorphism : is done at compile time according to the type.
2. Dynamic Polymorphism: is done at run-time in accordance with the content of pointer.

The Virtual Method Table or `vtable` is a mechanism used for supporting run-time method binding.


Let say we have the following classes:

```cpp
class base
{
public:

    void virtual foo()
    {
        std::cout<<"foo base" <<std::endl;
    }

    void print()
    {
        std::cout<<"print base" <<std::endl;
    }

};


class derived : public base
{
public:

    void foo() override
    {
        std::cout<<"foo derived" <<std::endl;
    }
    void print()
    {
        std::cout<<"print derived" <<std::endl;
    }
};
```

Now if we execute the followings in our main:

```cpp
base *bptr;
derived d;
bptr = &d;
```
virtual function, bound at run-time, we got the `foo()` from derived

```cpp
bptr->foo();
```

Non-virtual function, bound at compile time, we get `print()` from base:
```cpp
bptr->print();
```


## vtable
Every class that has virtual function will get a table name `vtable`. It is a  table of only "virtual function" memory addresses and it is maintained **per class**. `vtable`is created by compiler at compile time.
## vptr
when you create an object that which has virtual function, compiler adds a hidden member to your class called `__vptr`. It is pointer to the `vtable`, and it is maintained **per object instance**.
`vptr` is inherited to all the derived classes. If you create multiple from a class, the `__vptr` in all of them  point to the same `vtable` of a particular class.
 


```
   ____________________                                              _____________________________________
   |    base           |                                             |   vtable for class base            |
   |  _vptr------------|-------------------------------------------->|                                    |
   | virtual foo()<----|---------------------------------------------|--address of base version of foo()  |
---|->virtual bar()<---|---------------------------------------------|--address of base version of bar()  |
|  |___________________|                                             |____________________________________|
|
|
|
|
|
|   ____________________                                             ________________________________________________
|   |  derived         |                                             |   vtable for class derived                    |
|   |  _vptr-----------|-------------------------------------------->|                                               |
|   |         foo()<---|---------------------------------------------|--address of derived implementation of foo()   |
|   |         print()  |                                      |------|--address of base implementation of bar()      |
|   |__________________|                                      |      |_______________________________________________|
|                                                             |
|-------------------------------------------------------------|
```




Refs: [1](https://www.geeksforgeeks.org/virtual-functions-and-runtime-polymorphism-in-c-set-1-introduction/), [2](https://www.geeksforgeeks.org/virtual-function-cpp/), [3](https://www.learncpp.com/cpp-tutorial/125-the-virtual-table/)

[source code](../src/VTABLE_and_VPTR.cpp)
