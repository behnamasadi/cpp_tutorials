# Allocators

Allocators are objects responsible for encapsulating memory management. `std::allocator` is used when you want
to separate allocation and do construction in two steps. It is also used when separate destruction and
deallocation is done in two steps.

All the STL containers in C++ have a type parameter Allocator that is by default std::allocator.
The default allocator simply uses the operators new and delete to obtain and release memory

## Member functions associated with std::allocator()

1. address: It is used for obtaining the address of an object although it is removed in C++20.
2. construct: It is used to construct an object.It is also removed in C++20.
3. destroy: It is used to destruct an object in allocated storage.It is also removed in C++20.
4. max_size: It returns the largest supported allocation size.It is deprecated in C++17 and removed in
C++20.
5. allocate: Used for allocation of memory.
6. deallocate: Used for deallocation of memory.



## Advantage of using std::allocator
`std::allocator` is the memory allocator for the STL containers. This container can separate the memory
 allocation and de-allocation from the initialization and destruction of their elements.
Therefore, a call of vec.reserve(n) of a std::vector vec allocates only memory for at least n elements.
The constructor for each element will not be executed.

`std::allocator` can be adjusted according to the container of your need, for example, std::vector where you only want to allocate occasionally.
On the contrary, new doesn’t allow to have control over which constructors are called and simply construct all objects at the same time. That’s an advantage of std:: allocator over new

Refs: [1](https://stackoverflow.com/questions/31358804/whats-the-advantage-of-using-stdallocator-instead-of-new-in-c)


allocator for integer values:
```cpp
std::allocator<int> myAllocator;
```
allocate space for five ints:
```cpp
int* arr = myAllocator.allocate(4);
```
construct `arr[0]` and `arr[3]`:

```cpp
myAllocator.construct(arr, 10);
myAllocator.construct(arr+3, 100);
std::cout << arr[0] << std::endl;
std::cout << arr[3] << std::endl;
std::cout << myAllocator.max_size() << std::endl;
```

deallocate space for five ints
```cpp
myAllocator.deallocate(arr, 5);
```

