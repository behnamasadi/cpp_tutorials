# Immutable Objects 
In C++, immutable objects are those that, once created, cannot be modified. This concept is important for ensuring that the state of an object remains consistent and predictable throughout its lifetime, particularly in multi-threaded environments. To create an immutable object in C++, you typically define a class where:

1. All data members are declared as `const`.
2. There are no setter methods that modify the data members.
3. All member functions that don't modify the state of the object are declared as `const` methods.

Here is an example to illustrate this:

```cpp
#include <iostream>
#include <string>

class ImmutablePerson {
private:
    const std::string name;
    const int age;

public:
    // Constructor
    ImmutablePerson(std::string n, int a) : name(n), age(a) {}

    // Getter methods (const functions, do not modify the object)
    std::string getName() const {
        return name;
    }

    int getAge() const {
        return age;
    }
};

int main() {
    // Creating an immutable object
    ImmutablePerson person("Alice", 30);

    // Accessing the data members
    std::cout << "Name: " << person.getName() << ", Age: " << person.getAge() << std::endl;

    // Attempting to modify the object will result in a compile-time error
    // person.setAge(31); // Uncommenting this line will cause a compilation error

    return 0;
}
```

In this example:

- The `ImmutablePerson` class has two `const` data members: `name` and `age`.
- The constructor initializes these members and, after initialization, their values cannot be changed.
- The `getName` and `getAge` methods are declared as `const` methods, indicating that they do not modify the state of the object.
- There are no methods provided to modify `name` or `age` after the object is created, ensuring immutability.

If you try to modify the object after its creation, like by adding a setter method or trying to modify its members, the compiler will throw an error, enforcing immutability. This pattern is useful when you want to ensure that an object's state is read-only after its creation, providing safety against unintended modifications.




### Example: Immutable Point Class

```cpp
#include <iostream>

class ImmutablePoint {
private:
    const int x;
    const int y;

public:
    // Constructor to initialize the immutable point
    ImmutablePoint(int x_val, int y_val) : x(x_val), y(y_val) {}

    // Getter for x
    int getX() const {
        return x;
    }

    // Getter for y
    int getY() const {
        return y;
    }

    // Prevent assignment by deleting the assignment operator
    ImmutablePoint& operator=(const ImmutablePoint&) = delete;

    // Prevent modification by deleting the mutator methods
    void setX(int) = delete;
    void setY(int) = delete;
};

int main() {
    ImmutablePoint point(10, 20);

    std::cout << "Point X: " << point.getX() << std::endl;
    std::cout << "Point Y: " << point.getY() << std::endl;

    // The following lines will cause compilation errors
    // point.setX(30); // Error: 'void ImmutablePoint::setX(int)' is deleted
    // point.setY(40); // Error: 'void ImmutablePoint::setY(int)' is deleted
    // point = ImmutablePoint(30, 40); // Error: 'ImmutablePoint& ImmutablePoint::operator=(const ImmutablePoint&)' is deleted

    return 0;
}
```

### Explanation

1. **Class Definition**: The `ImmutablePoint` class has two private member variables, `x` and `y`, both declared as `const`. This ensures that their values cannot be changed after they are initialized.
2. **Constructor**: The constructor initializes the `x` and `y` values. Since they are `const`, they must be initialized in the initializer list.
3. **Getters**: The class provides getter methods `getX()` and `getY()` to access the values of `x` and `y`. These methods are marked as `const` to indicate that they do not modify the state of the object.
4. **Deleted Assignment Operator**: The assignment operator is deleted to prevent assigning a new value to an existing object, ensuring immutability.
5. **Deleted Mutator Methods**: Mutator methods `setX()` and `setY()` are declared and deleted to prevent any modification of the member variables after initialization.

### Usage in `main()`

- An `ImmutablePoint` object is created with `x` and `y` values.
- The values of `x` and `y` are printed using the getter methods.
- Any attempt to modify the object, either through mutator methods or assignment, will result in a compilation error.

This example demonstrates how to create and use an immutable class in C++. The key aspects are the use of `const` for member variables and the deletion of any methods that could modify the object's state.
