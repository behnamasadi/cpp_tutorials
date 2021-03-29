/*
A derived class object can be assigned to a base class object, but the other way is not possible.
Object slicing happens when a derived class object is assigned to a base class object,
additional attributes of a derived class object are sliced off to form the base class object.

We can avoid above unexpected behavior with the use of pointers or references.
Object slicing doesn’t occur when pointers or references to objects are passed as function
 arguments since a pointer or reference of any type takes same amount of memory

Refs: https://www.geeksforgeeks.org/object-slicing-in-c/
*/

class Base { int x, y; };

class Derived : public Base { int z, w; };

int main()
{
    Derived d;
    Base b = d; // Object Slicing,  z and w of d are sliced off
}



