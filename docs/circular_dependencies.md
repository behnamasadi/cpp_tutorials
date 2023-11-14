# Circular Dependency
Circular dependencies in C++ occur when two or more classes or files depend on each other directly or indirectly, creating a loop in the dependency graph. This can lead to issues like incomplete type errors, linking errors, or even runtime errors. To resolve circular dependencies, you can consider the following strategies:

1. **Forward Declarations**: Use forward declarations to declare a class without defining it. This can break the dependency cycle by letting you refer to a class in another class without needing its full definition.

2. **Refactoring the Code**: Sometimes, circular dependencies indicate a design issue. Consider refactoring your classes or code structure. This could involve splitting a class into multiple classes, combining classes, or moving part of the functionality to a new class.

3. **Using Interfaces or Abstract Classes**: Define interfaces or abstract classes that classes depend on, rather than depending on concrete implementations. This can reduce coupling between classes.

4. **Dependency Inversion**: Implement dependency inversion, a principle where high-level modules should not depend on low-level modules, but both should depend on abstractions.

5. **Using Pointers or References**: In some cases, using pointers or references instead of value objects can help, especially when combined with forward declarations.

6. **Header File Management**: Organize your header files and use include guards (`#ifndef`, `#define`, `#endif`) to prevent multiple inclusions.

## 1. Forward Declarations

Here's a basic example of using forward declarations to resolve a circular dependency:

Let's create a full example with `ClassA` and `ClassB`. This will illustrate how to resolve circular dependencies using forward declarations and pointers.

### classA.hpp
```cpp
#ifndef CLASSA_H
#define CLASSA_H

// Forward declaration of ClassB
class ClassB;

class ClassA {
public:
  ClassA();
  void setClassB(ClassB *b);
  void doSomethingWithB();
  void exampleMethod();

private:
  ClassB *b;
};

#endif // CLASSA_H
```

### classA.cpp
```cpp
#include "classA.hpp"
#include "classB.hpp"
#include <iostream>

ClassA::ClassA() : b(nullptr) {}

void ClassA::setClassB(ClassB *b) { this->b = b; }

void ClassA::doSomethingWithB() {
  if (b) {
    // Assuming ClassB has a method exampleMethod() that we want to call
    std::cout << "ClassA is calling method from classB." << std::endl;
    b->exampleMethod();
  } else {
    std::cout << "ClassB instance is not set in ClassA." << std::endl;
  }
}

void ClassA::exampleMethod() {
  std::cout << "Method in ClassA called." << std::endl;
}
```

### classB.hpp
```cpp
#ifndef CLASSB_H
#define CLASSB_H

#include "classA.hpp"

class ClassB {
public:
  ClassB();
  void setClassA(ClassA *a);
  void exampleMethod();
  void doSomethingWithA();

private:
  ClassA *a;
};

#endif // CLASSB_H
```

### classB.cpp
```cpp
#include "classB.hpp"
#include <iostream>

ClassB::ClassB() : a(nullptr) {}

void ClassB::exampleMethod() {
  std::cout << "Method in ClassB called." << std::endl;
}

void ClassB::doSomethingWithA() {
  std::cout << "ClassB is calling method from classA." << std::endl;
  a->exampleMethod();
}
void ClassB::setClassA(ClassA *a) { this->a = a; }
```

### Explanation:
- **ClassA.hpp**: Forward declares `ClassB` and defines `ClassA`. It includes a method to set a pointer to a `ClassB` object and a method to interact with `ClassB`.
- **ClassA.cpp**: Includes both `ClassA.hpp` and `ClassB.hpp`. It defines the methods of `ClassA`.
- **ClassB.hpp**: Includes `ClassA.hpp` and defines `ClassB`. It could also include a pointer to a `ClassA` object if needed.
- **ClassB.cpp**: Implements the methods of `ClassB`.

### Usage:
To use these classes, create instances of `ClassA` and `ClassB` in your main program, and set the `ClassB` instance in `ClassA` using `setClassB`.

### Note:
In this setup, `ClassB` doesn't hold a reference to `ClassA`. If you need bidirectional communication, you would add a pointer to `ClassA` in `ClassB` and set it similarly. Be mindful of object ownership and lifetimes to avoid memory leaks or dangling pointers.



## 2. Dependency Inversion


To implement Dependency Inversion in your current setup with `ClassA` and `ClassB`, you'll need to introduce an interface or an abstract class. Dependency Inversion Principle (DIP) states that high-level modules should not depend on low-level modules but should depend on abstractions. Abstractions, in this context, should not depend on details; instead, details should depend on abstractions.

Here's how you can refactor your code:

1. **Create Abstract Classes or Interfaces**: Define abstract classes or interfaces that `ClassA` and `ClassB` will implement. These abstract classes or interfaces will declare the methods that need to be overridden.

2. **Change ClassA and ClassB to Depend on Abstractions**: Instead of having `ClassA` and `ClassB` directly depend on each other, make them depend on these new abstractions.

### Example Refactoring:

1. **Define Abstract Classes/Interfaces**

   **IA.h**
   ```cpp
   #ifndef IA_H
   #define IA_H

   class IA {
   public:
       virtual void exampleMethod() = 0;
       virtual void doSomethingWithB() = 0;
       virtual ~IA() {}
   };

   #endif // IA_H
   ```

   **IB.h**
   ```cpp
   #ifndef IB_H
   #define IB_H

   class IB {
   public:
       virtual void exampleMethod() = 0;
       virtual void doSomethingWithA() = 0;
       virtual ~IB() {}
   };

   #endif // IB_H
   ```

2. **Modify ClassA and ClassB to Implement These Interfaces**

   **ClassA.h**
   ```cpp
   #ifndef CLASSA_H
   #define CLASSA_H

   #include "IA.h"
   #include "IB.h"

   class ClassB; // Forward declaration

   class ClassA : public IA {
   public:
       ClassA();
       void setClassB(IB* b);
       void doSomethingWithB() override;
       void exampleMethod() override;

   private:
       IB* b;
   };

   #endif // CLASSA_H
   ```

   **ClassB.h**
   ```cpp
   #ifndef CLASSB_H
   #define CLASSB_H

   #include "IB.h"
   #include "IA.h"

   class ClassA; // Forward declaration

   class ClassB : public IB {
   public:
       ClassB();
       void setClassA(IA* a);
       void exampleMethod() override;
       void doSomethingWithA() override;

   private:
       IA* a;
   };

   #endif // CLASSB_H
   ```

3. **Implement the Classes**

   In your `.cpp` files for `ClassA` and `ClassB`, you will implement these methods as before, but now they depend on the interfaces (`IA` and `IB`) instead of the concrete classes.

### Advantages of This Approach:

- **Reduced Coupling**: Classes depend on abstractions, not concrete implementations, reducing coupling.
- **Flexibility**: Makes it easier to substitute different implementations of `IA` or `IB` without changing `ClassA` or `ClassB`.
- **Testability**: Easier to mock or stub out dependencies for unit testing.

By following this approach, you adhere to the Dependency Inversion Principle, one of the SOLID principles of object-oriented design, which leads to more maintainable and flexible code.







