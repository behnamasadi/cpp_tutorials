# Memory Layout

The term layout refers to how the members of an object of class, struct or union type are arranged in memory. In some cases, the layout is well-defined by the language specification. But when a class or struct contains certain C++ language features such as virtual base classes, virtual functions, members with different access control, then the compiler is free to choose a layout.

That layout may vary depending on what optimizations are being performed and in many cases the object might not even occupy a contiguous area of memory.

For example, if a class has virtual functions, all the instances of that class might share a single [virtual function table](VTABLE_and_VPTR.md)


Because the layout is undefined they cannot be passed to programs written in other languages, such as C, and because they might be non-contiguous they cannot be reliably copied with fast low-level functions such as memcopy, or serialized over a network.


C++14 introduced three categories of **simple classes/structs**: 
- trivial
- standard-layout
- POD (Plain Old Data). 

To determine whether a given type belongs to a given category the Standard Library has the following function templates
- is_trivial<T>
- is_standard_layout<T> 
- is_pod<T> 


Refs: [1](https://docs.microsoft.com/en-us/cpp/cpp/trivial-standard-layout-and-pod-types?view=msvc-170)
