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
