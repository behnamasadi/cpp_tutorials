# The Rule of 5
The Rule of 5 in C++ is a modern extension of the Rule of 3, which was a guideline in earlier versions of the language. It's relevant when managing resources like memory, file handles, or network connections in a class. The Rule of 5 suggests that if a class needs to explicitly declare or define **any** one of the following functions, it probably needs to explicitly declare or define **all five** of them:

1. Destructor (`~Class()`): To clean up resources.
2. Copy Constructor (`Class(const Class&)`): For making a copy of an object.
3. Copy Assignment Operator (`Class& operator=(const Class&)`): For assigning one object to another.
4. Move Constructor (`Class(Class&&)`): For transferring resources from a temporary object (rvalue).
5. Move Assignment Operator (`Class& operator=(Class&&)`): For transferring resources from one object to another, where the source is a temporary object.

### When It's Needed
- **Managing Resources:** If your class manages resources that are not automatically handled by C++ (like dynamic memory, file handles, sockets, etc.), you need the Rule of 5 to ensure proper resource management.
- **Avoiding Resource Leaks:** Without proper destructors, copy/move constructors, and assignment operators, you might end up with resource leaks (e.g., memory not being freed).
- **Preventing Shallow Copies:** Default copy constructors and assignment operators perform shallow copies, which can be problematic for dynamic resources.

### How to Determine if You Need It
- **Check Resource Management:** If your class is responsible for a resource that needs explicit management (like dynamic memory allocation with `new` and `delete`), you likely need to implement the Rule of 5.
- **Look for Non-trivial Operations:** If your class performs non-trivial operations on construction, destruction, copy, or move, it's a hint that you need custom implementations of these methods.
- **Deep Copy Needs:** If your class contains pointers or references to external resources, you'll need to manage how these are copied or moved.

### Example in C++
```cpp
class ResourceHolder {
public:
    // Constructor
    ResourceHolder(size_t size) : data(new int[size]), size(size) {}

    // Destructor
    ~ResourceHolder() { delete[] data; }

    // Copy Constructor
    ResourceHolder(const ResourceHolder& other) : data(new int[other.size]), size(other.size) {
        std::copy(other.data, other.data + size, data);
    }

    // Copy Assignment Operator
    ResourceHolder& operator=(const ResourceHolder& other) {
        if (this != &other) {
            delete[] data;
            size = other.size;
            data = new int[size];
            std::copy(other.data, other.data + size, data);
        }
        return *this;
    }

    // Move Constructor
    ResourceHolder(ResourceHolder&& other) noexcept : data(other.data), size(other.size) {
        other.data = nullptr;
        other.size = 0;
    }

    // Move Assignment Operator
    ResourceHolder& operator=(ResourceHolder&& other) noexcept {
        if (this != &other) {
            delete[] data;
            data = other.data;
            size = other.size;
            other.data = nullptr;
            other.size = 0;
        }
        return *this;
    }

private:
    int* data;
    size_t size;
};
```

In this example, `ResourceHolder` manages a dynamic array, so it needs to implement all five functions to handle resource management correctly. The copy constructor/assignment creates a deep copy, while the move constructor/assignment transfers ownership of the resource, preventing unnecessary copying and ensuring proper cleanup.
