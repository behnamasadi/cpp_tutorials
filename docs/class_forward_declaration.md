#Class Forward Declaration
Suppose you want to define a new class `B` that uses objects of class `A`.

Assume you wish to create a new class `B` that makes use of class `A`.
In our example `B` only uses references/ pointers to `A`. By using forward declaration `class A;` there is no need create/ include `<A.h>`. 

```cpp
class A;

class B
{
   A* m_a_ptr;
   const A& m_a_ref;
};

class A
{

};
```


But if `B` derives from `A` 


```cpp
#include <A.h>

class B : public A 
{
};
```
or `B` explicitly (or implicitly) uses objects of class`A`, You then need to include `<A.h>`

```cpp
#include <A.h>
class B
{
  public:
    A m_A ;

    void mymethod(A par) ;   
};
```

[source code](../src/class/class_forward_declaration.cpp)
