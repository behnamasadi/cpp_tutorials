# Copy elision
Copy elision in C++ is a compiler optimization technique that reduces the overhead of copying and moving objects. It is especially important in C++ because it can significantly improve performance by eliminating unnecessary copy and move operations. Here's a deeper dive into the concept, along with various examples:

### 1. Return Value Optimization (RVO)

RVO is a form of copy elision that occurs when a function returns a local object. The compiler can construct the return value directly in the memory space allocated for it in the caller's context, avoiding the need to copy or move the object.

**Example:**
```cpp
class MyClass {
public:
    MyClass() {}
    MyClass(const MyClass&) {
        std::cout << "Copy constructor called" << std::endl;
    }
};

MyClass createObject() {
    MyClass obj;
    return obj; // RVO applies here
}

int main() {
    MyClass myObject = createObject(); // No copy is made due to RVO
}
```

In this example, without RVO, you would expect the copy constructor to be called when returning `obj` from `createObject()`. However, with RVO, the compiler can optimize away this copy.

### 2. Named Return Value Optimization (NRVO)

NRVO is similar to RVO but applies when the returned object has a name.

**Example:**
```cpp
MyClass createObject(bool condition) {
    MyClass obj1, obj2;
    if (condition) {
        return obj1; // NRVO can apply here
    } else {
        return obj2; // NRVO can also apply here
    }
}
```

In this case, the compiler may directly construct `obj1` or `obj2` in the return value's space, depending on the condition.

### 3. Elision of Copy/Move Constructors in Initialization

Copy elision can also occur when an object is initialized directly from a temporary object.

**Example:**
```cpp
MyClass obj = MyClass(); // Copy/move constructor may be elided
```

Here, the compiler can construct the temporary `MyClass()` object directly in the space allocated for `obj`, eliminating the need for a copy or move.

### 4. Elision in Throwing and Catching Exceptions

When an exception is thrown and caught, the compiler may elide the copy construction of the exception object.

**Example:**
```cpp
try {
    throw MyClass(); // Copy may be elided
} catch (MyClass obj) {
    // ...
}
```

In the above scenario, the compiler can optimize by constructing the `MyClass` exception object directly in the exception handling area.

### Important Notes:

- Copy elision is permitted by the C++ standard, but not guaranteed. It depends on the compiler and its optimization settings.
- Since C++17, the standard mandates certain cases of copy elision, particularly in returning and throwing situations, making it more predictable.
- It's important to write code that doesn't rely on copy elision for correctness, even though it can be used for performance optimization.


[code](../src/RVO_NRVO_copy_elision.cpp)

