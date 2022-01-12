# Virtual Destructor
lets write the following code:

```cpp
class Base
{
    int * data;
public:
    Base()
    {
        std::cout<<"base constructor" <<std::endl;
        int size=10;
        data=new int[size];
    }
    ~Base()
    {
        std::cout<<"base destructor" <<std::endl;
        delete []data;
    }
};

class Derived: public Base
{
    double * points;
    public:
        Derived()
        {
            std::cout<<"derived constructor" <<std::endl;
            int size=5;
            points=new double[size];
        }
        ~Derived()
        {
            delete [] points;
            std::cout<<"derived destructor" <<std::endl;
        }
};
```
If we execute the following, everything fine:
```
    std::cout<<"=================== base =================== " <<std::endl;
    Base * objBase=new Base;
    delete objBase;
    std::cout<<"=================== derived =================== " <<std::endl;
    Derived * objDerived=new Derived;
    delete objDerived;
```
and we have the following in the output:
```cpp
=================== base =================== 
base constrcutor
base destrcutor
=================== derived =================== 
base constrcutor
derived constrcutor
derived destrcutor
base destrcutor

```

But if we write a code with run time polymorphism:
```cpp
    Base * objPolymorphic=new Derived;
    delete objPolymorphic;
```
    
we would have the followings in the output:

```
base constrcutor
derived constrcutor
base destrcutor
```


if we don't make the destrcutor virtual in the **Base**, we only call the destructor of **Base** and not
**Derived** class so we will have memory leaking.
**Whenever writing a class that you extend it later you need to make the destrcutor of your base class virtual.**

```cpp
    virtual ~Base()
    {
        std::cout<<"base destrcutor" <<std::endl;
        delete []data;
    }
```


Refs: [1](https://stackoverflow.com/questions/461203/when-to-use-virtual-destructors), [2](https://www.youtube.com/watch?v=jELbKhGkEi0)


# Virtual Copy Constructor

We can't make the constructor of a class virtual,
compiler must be aware of the class type to create the object.
If we make constructor virtual, compiler flags an error. In fact
except inline, no other keyword is allowed in the declaration of constructor.


[source code](../src/class/virtual_copy_constructor_destructor.cpp)
