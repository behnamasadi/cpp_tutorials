# Allocators


In most cases, you don't need to use `std::allocator` directly in C++. The standard library containers like `std::vector`, `std::list`, `std::map`, etc., already use `std::allocator` as their default memory allocator. When you create instances of these containers, they automatically manage memory allocation and deallocation for you.

However, there are certain scenarios where you might need to use `std::allocator` directly:

1. **Custom Memory Management**: If you have specific requirements for memory management that are not met by the default allocator, you can implement a custom allocator and use it with standard containers. 

2. **Advanced Performance Optimization**: In performance-critical applications, a custom allocator can be used to optimize memory usage patterns, reduce fragmentation, or improve allocation/deallocation speed.

3. **Learning and Understanding**: Understanding how `std::allocator` works is beneficial for gaining deeper insights into C++ memory management and the workings of the standard library containers.

4. **Low-Level Operations**: In low-level programming or library development, you might need to interact with memory in a more controlled or specific way than what standard containers provide.

In summary, while `std::allocator` is a fundamental part of the C++ Standard Library's container implementation, most developers do not need to interact with it directly in everyday programming. It's primarily used for advanced scenarios involving custom memory management or performance optimization.



Let's consider a real scenario where using a custom allocator with `std::allocator` might be beneficial: a high-performance game engine.

### Scenario: High-Performance Game Engine

**Context**: In game development, especially for high-performance game engines, efficient memory management is crucial. Games often require rapid allocation and deallocation of objects like particles, NPCs (non-player characters), and temporary data structures for rendering. The default memory allocator provided by the C++ standard library might not be optimized for these frequent, small allocations and deallocations, which can lead to memory fragmentation and performance degradation over time.

**Custom Allocator Implementation**: To address this, a game engine developer decides to implement a custom allocator, using `std::allocator` as a base, to improve memory management for frequently used game objects.

**Goals**:
1. **Reduce Fragmentation**: By using a pool allocator strategy, where memory is pre-allocated in chunks, the allocator can reduce fragmentation caused by frequent small allocations.
2. **Improve Allocation Speed**: Pool allocators can offer faster allocation and deallocation since the memory is already allocated in bulk.
3. **Custom Memory Management Policies**: Implement custom policies for handling memory in scenarios unique to the game, like handling memory for objects that have a short lifespan or require real-time deallocation.

**Usage with Standard Containers**: The custom allocator is then used with standard library containers like `std::vector` or `std::list`. For example, particles in a particle system might be stored in a `std::vector` that uses this custom allocator.

```cpp
// Example definition of a custom allocator
template <typename T>
class GameAllocator : public std::allocator<T> {
    // Custom allocation and deallocation logic
    // ...
};

// Using the custom allocator with a standard container
std::vector<Particle, GameAllocator<Particle>> particleList;
```

**Outcome**: By employing this custom allocator, the game engine can manage memory more effectively, leading to smoother gameplay, reduced lag during dynamic object creation and destruction, and overall better performance, especially in memory-intensive scenarios like large, dynamic game worlds.

### Summary
This scenario illustrates how a custom allocator, building upon `std::allocator`, can be used in a high-performance game engine to optimize memory management for scenarios that involve frequent and rapid allocation/deallocation of objects. The use of such specialized allocators can lead to significant performance improvements in applications where default memory management techniques are insufficient.

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

Refs: [1](https://www.geeksforgeeks.org/stdallocator-in-cpp-with-examples/), [2](https://stackoverflow.com/questions/21081796/why-not-to-inherit-from-stdallocator?noredirect=1&lq=1), [3](https://stackoverflow.com/questions/55451468/what-is-stdallocator-and-why-do-i-need-it?noredirect=1&lq=1), [4](https://stackoverflow.com/questions/826569/compelling-examples-of-custom-c-allocators?noredirect=1&lq=1), [5](https://stackoverflow.com/questions/31358804/whats-the-advantage-of-using-stdallocator-instead-of-new-in-c), [6](https://medium.com/@vgasparyan1995/what-is-an-allocator-c8df15a93ed)
