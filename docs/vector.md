# Vectors
Working with vectors in C++ involves several advanced topics and techniques. Here's a guide to some of these advanced topics:

1. **Iterators**:
   - Iterators are used to navigate through the elements of a vector.
   - Types of iterators: `iterator`, `const_iterator`, `reverse_iterator`, `const_reverse_iterator`.
   - Common operations: `begin()`, `end()`, `rbegin()`, `rend()`, `++` (increment), `--` (decrement), `*` (dereference).

2. **Capacity and Size Management**:
   - `resize()`: Changes the size of the vector, can truncate or expand the vector.
   - `reserve()`: Requests a change in capacity without changing the size.
   - `shrink_to_fit()`: Requests the vector to reduce its capacity to fit its size (C++11).

3. **Algorithms**:
   - The C++ Standard Library provides many algorithms that can be used with vectors, such as `std::sort()`, `std::reverse()`, `std::find()`.
   - These algorithms often work with iterators to specify a range within the vector.

4. **Custom Comparator for Sorting**:
   - You can define a custom comparator function or functor to use with `std::sort()` for sorting elements based on custom criteria.

5. **Lambda Expressions**:
   - Introduced in C++11, lambda expressions provide a convenient way to define anonymous functions. They can be used in conjunction with STL algorithms.

6. **Nested Vectors**:
   - Vectors can contain other vectors, creating a matrix-like structure.
   - Useful for multidimensional data.

7. **Type Inference with auto**:
   - The `auto` keyword (C++11) allows the compiler to infer the type of a variable, making code cleaner especially when dealing with complex iterator types.

8. **Move Semantics and Emplace Operations**:
   - C++11 introduced move semantics, which can be used to efficiently add elements to a vector without unnecessary copying.
   - `emplace_back()` and `emplace()` methods for constructing elements in place.

9. **Range-based For Loops**:
   - C++11 introduced range-based for loops, which provide a simpler syntax for iterating over elements in a vector.

10. **Understanding Vector Reallocation**:
    - Understanding how vectors grow, reallocation, and its impact on pointers and iterators.

11. **Using std::vector with Custom Objects**:
    - Using vectors with user-defined classes or structs, including handling of copy/move constructors and assignment operators.

12. **Exception Safety and Vectors**:
    - Ensuring your vector usage is exception-safe, especially in the context of resource management and RAII (Resource Acquisition Is Initialization).

13. **Interoperability with C-style Arrays and Data**:
    - Using `data()` member function for C++11 and later to get a pointer to the data, which is useful for interfacing with C-style functions.

14. **Advanced Memory Management**:
    - Custom allocators can be used with vectors for specialized memory management.

To effectively master these topics, you should not only understand the syntax and usage but also the underlying principles and the impact on performance and memory management. Practice by writing various types of programs and experimenting with these features will solidify your understanding.




### Example 1: Passing a `std::vector` to a C-style Function

Assuming you have a C-style function that expects an array pointer and the size of the array, you can use `std::vector`'s `data()` method to pass the vector's internal array to this function. Here's an example:

```cpp
#include <vector>
#include <iostream>

// A C-style function expecting a pointer to an array and its size
void processArray(int* arr, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        arr[i] *= 2; // Example operation: doubling each element
    }
}

int main() {
    std::vector<int> vec = {1, 2, 3, 4, 5};

    // Passing the vector to a C-style function
    processArray(vec.data(), vec.size());

    // Printing the modified vector
    for (int i : vec) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    return 0;
}
```

In this example, `vec.data()` returns a pointer to the first element of the vector, which is compatible with C-style array pointers.

### Example 2: Initializing a `std::vector` from a C-style Array

You might also encounter situations where you need to initialize a `std::vector` from a C-style array. Here’s how you can do it:

```cpp
#include <vector>
#include <iostream>

int main() {
    // A C-style array
    int arr[] = {10, 20, 30, 40, 50};

    // Initialize a vector from a C-style array
    std::vector<int> vec(std::begin(arr), std::end(arr));

    // Printing the vector
    for (int i : vec) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    return 0;
}
```

In this example, `std::begin(arr)` and `std::end(arr)` are used to create iterators that point to the beginning and end of the C-style array, which are then used to initialize the vector.

### Example 3: Converting a `std::vector` to a C-style Array

Sometimes, you may need to convert a `std::vector` to a C-style array. This can be done using `data()`:

```cpp
#include <vector>
#include <iostream>

int main() {
    std::vector<int> vec = {1, 2, 3, 4, 5};

    // Getting a pointer to the vector's data
    int* cArray = vec.data();

    // Use the C-style array
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << cArray[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
```

In this example, `vec.data()` gives you a pointer to the first element of the vector, which can be used as a C-style array.

These examples demonstrate the flexibility of `std::vector` in interfacing with C-style arrays, making it easier to integrate C++ code with legacy C code or libraries that expect C-style arrays.



Sure, I can explain more about move semantics and emplace operations in the context of `std::vector` in C++, with examples.

### 1. Move Semantics with `std::vector`

Move semantics, introduced in C++11, is a feature that allows the resources of an rvalue (a temporary object) to be "moved" rather than copied. This is particularly useful for `std::vector` when dealing with objects that manage dynamic memory, as it avoids unnecessary deep copies, leading to performance improvements.

#### Example: Using Move Semantics with `std::vector`

```cpp
#include <iostream>
#include <vector>
#include <string>

int main() {
    std::vector<std::string> vec;

    std::string str = "Hello World";

    // Copying the string (pre-C++11 way)
    vec.push_back(str); // str is copied

    // Moving the string (C++11 and later)
    vec.push_back(std::move(str)); // str's resources are moved

    // Note: str is now in a valid but unspecified state

    for (const auto& item : vec) {
        std::cout << item << std::endl;
    }

    return 0;
}
```

In this example, `std::move(str)` casts `str` as an rvalue, which allows `push_back` to invoke the move constructor of `std::string`, transferring the internal buffer from `str` to the new element in the vector without copying.

### 2. Emplace Operations

`emplace_back` and `emplace` are methods provided by `std::vector` to construct elements in place. They can construct objects directly within the vector's memory, potentially providing performance benefits by eliminating temporary objects and redundant copies.

#### Example: Using `emplace_back`

```cpp
#include <iostream>
#include <vector>
#include <string>

int main() {
    std::vector<std::string> vec;

    // Emplace back a string
    vec.emplace_back("Hello, World!");

    // Emplace back constructs the string in place
    vec.emplace_back(10, 'x'); // Creates a string of 10 'x's

    for (const auto& item : vec) {
        std::cout << item << std::endl;
    }

    return 0;
}
```

In the above example, `emplace_back("Hello, World!")` constructs a `std::string` directly inside the vector. Similarly, `emplace_back(10, 'x')` calls the `std::string` constructor that creates a string with 10 'x' characters, again directly inside the vector. This avoids the extra step of creating a temporary `std::string` object and then moving or copying it into the vector.

#### Example: Using `emplace` for Insertion

`emplace` is used to construct elements at a specific position in the vector.

```cpp
#include <iostream>
#include <vector>
#include <string>

int main() {
    std::vector<std::string> vec = {"Start", "End"};

    // Emplace a string in the middle
    auto it = vec.begin() + 1;
    vec.emplace(it, "Middle");

    for (const auto& item : vec) {
        std::cout << item << std::endl;
    }

    return 0;
}
```

Here, `emplace` is used to insert a new string directly into the vector at the position specified by the iterator `it`. This is more efficient than inserting a temporary `std::string` object.

These examples demonstrate how move semantics and emplace operations can be used to optimize the performance of `std::vector` in C++, especially when dealing with objects that manage their own memory.



Using `std::vector` with custom objects involves understanding how vectors interact with the user-defined classes, particularly in terms of construction, copying, moving, and destruction. Let's explore some examples to illustrate these concepts.

### 1. Basic Example with a Custom Class

First, let's define a simple custom class and see how it works with `std::vector`.

```cpp
#include <iostream>
#include <vector>
#include <string>

class MyClass {
    std::string name;
    int value;

public:
    MyClass(const std::string& n, int v) : name(n), value(v) {
        std::cout << "Constructing " << name << std::endl;
    }

    ~MyClass() {
        std::cout << "Destructing " << name << std::endl;
    }

    // Copy constructor
    MyClass(const MyClass& other) : name(other.name), value(other.value) {
        std::cout << "Copying " << name << std::endl;
    }

    // Move constructor
    MyClass(MyClass&& other) noexcept : name(std::move(other.name)), value(other.value) {
        std::cout << "Moving " << name << std::endl;
    }

    // Utility function to display the object
    void display() const {
        std::cout << name << ": " << value << std::endl;
    }
};

int main() {
    std::vector<MyClass> vec;

    vec.emplace_back("Object1", 10);  // Direct construction
    vec.emplace_back("Object2", 20);

    MyClass obj("Object3", 30);
    vec.push_back(obj);               // Copying

    vec.push_back(std::move(obj));    // Moving

    for (const auto& item : vec) {
        item.display();
    }

    return 0;
}
```

In this example, `MyClass` has a copy constructor and a move constructor. The `emplace_back` method constructs objects directly inside the vector. The `push_back` method demonstrates copying and moving elements into the vector.

### 2. Managing Resources in a Custom Class

When your class manages resources (like dynamic memory), it's crucial to properly implement the copy constructor, move constructor, assignment operators, and destructor to adhere to the Rule of Three/Five.

```cpp
#include <iostream>
#include <vector>
#include <cstring>

class ResourceManagingClass {
    char* data;
    size_t size;

public:
    ResourceManagingClass(const char* d) {
        size = std::strlen(d);
        data = new char[size + 1];
        std::strcpy(data, d);
        std::cout << "Constructing: " << data << std::endl;
    }

    ~ResourceManagingClass() {
        std::cout << "Destructing: " << data << std::endl;
        delete[] data;
    }

    // Copy constructor
    ResourceManagingClass(const ResourceManagingClass& other) : size(other.size) {
        data = new char[size + 1];
        std::strcpy(data, other.data);
        std::cout << "Copying: " << data << std::endl;
    }

    // Move constructor
    ResourceManagingClass(ResourceManagingClass&& other) noexcept : data(other.data), size(other.size) {
        other.data = nullptr;
        std::cout << "Moving: " << data << std::endl;
    }

    // ... (Copy and move assignment operators should also be defined)
};

int main() {
    std::vector<ResourceManagingClass> vec;

    vec.emplace_back("Resource1");
    vec.push_back(ResourceManagingClass("Resource2"));
    vec.push_back(ResourceManagingClass("Resource3"));

    return 0;
}
```

In this example, `ResourceManagingClass` manages a dynamically allocated array. The copy constructor makes a deep copy of the data, while the move constructor transfers ownership of the resource, preventing unnecessary copying. Proper management of resources is crucial to prevent memory leaks and ensure correct behavior when objects are added to or removed from the vector.

These examples highlight key considerations when using `std::vector` with custom classes, especially regarding resource management and object life cycles.

# vector initialization

1. Initializing by pushing/ emplacing values one by one :
    std::vector<int> vect1;
```cpp
vect1.emplace_back(10);
vect1.emplace_back(20);
vect1.emplace_back(30);
```
2. Specifying size and initializing all values. Here we create a vector of size 10 with  all values as 0.5.
```cpp
int size = 10;
double value=0.5;
std::vector<double> vect2(size, value);
```

3. Initializing like arrays
```cpp
std::vector<int> vect3{ 1, 2, 3 };
std::vector<person> vectPerson={person(10),person(12),person(3)};
```


# vector iteration


# emplace_back vs push_bask

Using `emplace_back` instead of `push_back` in a C++ `vector` when you have move constructors and move assignment operators can lead to some distinct behaviors and advantages:

1. **Direct Construction**: `emplace_back` constructs the object directly in place at the end of the `vector`. It forwards its arguments to the constructor of the element type, thereby avoiding an extra copy or move of the object. This is more efficient than `push_back`, which constructs the object and then moves or copies it into the vector.

2. **Efficiency with Move Semantics**: If you have a move constructor and move assignment operator, `emplace_back` can be more efficient. When you use `push_back`, and if the vector needs to be resized, existing elements are moved to the new storage. With move semantics, these operations are more efficient compared to deep copying, but they still entail a move operation. `emplace_back` minimizes even these move operations by constructing the element directly in its final location within the vector.

3. **No Temporary Object Creation**: With `emplace_back`, you don't create a temporary object which you then move into the vector (as you would typically do with `push_back`). This can save both time and resources, especially if the move operation is non-trivial.

4. **Perfect Forwarding**: `emplace_back` uses perfect forwarding to pass arguments to the constructor of the element type. This means you can pass arguments that are perfectly suited for the constructor of the object being emplaced, which can include lvalue references, rvalue references, or any other type of argument.

5. **Code Clarity and Intent**: By using `emplace_back`, you make it clear that your intention is to construct an object directly in the place it's going to be used. This can make the code easier to understand in terms of object lifecycle and resource management.

In summary, when you have a move constructor and move assignment operator, using `emplace_back` in a C++ `vector` can lead to more efficient object construction and resource management, as it constructs objects directly in place and fully utilizes move semantics.





since here pushing during the loop, the destination vector 
might need to resize couple of times, so we resize it once in the beginning.

```cpp
int size=10;
int value =0;
std::vector<int> src(size,value);
std::vector<int> dst;
dst.resize(src.size());
for(auto i:src)
	dst.push_back(i);
```
Refs: [1](https://www.youtube.com/watch?v=jKS9dSHkAZY), [2](http://htmlpreview.github.io/?https://github.com/HowardHinnant/papers/blob/master/insert_vs_emplace.html)

# std::copy

# erase vs remove

# front()/back() vs begin()/end()

# copying Vectors

# reinterpret vector

# vector to/from C array

# string to vector of char 
```cpp
std::string str("ali baba");
str.begin();
std::vector<char> charVec(str.begin(),str.end() );
```

# vector accessing elements
we can access vector element both by `[i]` operator and by `.at(i)` . `at(i)` is a function call while `[]` is a direct access so it is cheaper and more efficient

[source code](../src/vector.cpp)
