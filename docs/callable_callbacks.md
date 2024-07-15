# Callbacks and Callable Objects


- Callable: Any object or function that can be called like a function.
Examples: Function pointers, functors, lambdas, std::function.

 Callback: A callable object passed to another function to be invoked at a later time.
Used for: Event handling, asynchronous operations, decoupling function logic.



* [1. Function Pointers](#1-function-pointers)
* [2. Functor (Function Objects)](#2-functor--function-objects-)
* [3. std::function, std::placeholders, and std::bind](#3-std--function--std--placeholders--and-std--bind)
* [4. Lambda Functions](#4-lambda-functions)
* [5. Member Function Pointers](#5-member-function-pointers)
* [6. Signals and Slots](#6-signals-and-slots)
* [7. std::invoke](#7-std--invoke)
* [8. Packaged Tasks](#8-packaged-tasks)
* [9. Coroutines](#9-coroutines)
* [10. Auto-generated operator() from a Captureless Lambda](#10-auto-generated-operator---from-a-captureless-lambda)




A callback is a function passed as an argument to another function, which can then call this function. 
Example: Imagine you have a class for modeling a robot and you want to give this freedom to the user to define and use different motion planners. You can ask the user to pass the solver function to the robot class so the robot class uses that one for moving around.


```cpp
typedef std::function<std::vector<double>(double, double)> CallbackFunction;

class robot {

public:
  CallbackFunction m_planner;
  // robot(const CallbackFunction &callback) : m_planner(callback) {}

  void executePlan(std::vector<double> trajectory) {
    std::cout << "robot is traversing the generated trajectory:" << std::endl;
    for (const auto &p : trajectory)
      std::cout << p << std::endl;
  }

  void move(double start, double goal) {

    std::vector<double> trajectory = m_planner(start, goal);
    executePlan(trajectory);
  }
};
```


There are several mechanisms to implement callbacks. Here are the most common ones:
## 1. Function Pointers
**Function Pointers**: The simplest form of callback mechanism in C++. This involves passing a pointer to a function. Simple and efficient but lacks type safety and flexibility.

syntax:
```cpp
typedef   void      (*FunctionFunc)  ( );
           ^                ^         ^
        return type      type name  arguments
```

syntax with member function pointers
```cpp
int (MyClass::*MyTypedef)( int); //MyTypedef is a variable
typedef int (MyClass::*MyTypedef)( int); //MyTypedef is a type
typedef void (Receiver::* Action)();
```
so you can have this in your class:
```cpp
Action action;
```


we define the following planners:

```cpp
std::vector<double> planer1(double start, double goal) {
  return {start, (start + goal) / 2, goal};
}

std::vector<double> planer2(double start, double goal) {
  double step = (goal - start) / 20;
  std::vector<double> values;
  for (double value = start; value < goal; value += step)
    values.push_back(value);
  return values;
}
```
Now we can assign and use different solvers:

```cpp
robot myrobot;
myrobot.m_callback = planer1;
myrobot.move(start, goal);
```




## 2. Functor (Function Objects)
**Functor (Function Objects)**: Custom objects that overload the `operator()`. This allows objects to be used as if they were functions. It provides flexibility and can maintain state.

```cpp
#include <iostream>

// Functor class
class MyCallbackFunctor {
public:
    void operator()(int value) const {
        std::cout << "Callback functor called with value: " << value << std::endl;
    }
};

// Function that accepts a callback
template <typename Callback>
void processWithCallback(Callback callback, int value) {
    callback(value);
}

int main() {
    MyCallbackFunctor functor;
    processWithCallback(functor, 42);
    return 0;
}
```



## 3. std::function, std::placeholders, and std::bind
**std::function and std::bind**: More flexible and type-safe mechanisms introduced in C++11. `std::function` can hold any callable target (function pointers, functors, lambdas), and `std::bind` can bind arguments to functions or member functions. **std::function and std::bind** are very flexible, type-safe, and can wrap different types of callables.


```cpp
std::function<std::vector<double>(double, double)> planer2_ptr =
      std::bind(&planer2, std::placeholders::_1, std::placeholders::_2);

myrobot.m_callback = planer2_ptr;
myrobot.move(start, goal);
```


Another example:


```cpp
#include <iostream>

// Function to be used as a callback
void myCallbackFunction(int value) {
    std::cout << "Callback function called with value: " << value << std::endl;
}

// Function that accepts a callback
void processWithCallback(void (*callback)(int), int value) {
    callback(value);
}

int main() {
    processWithCallback(myCallbackFunction, 42);
    return 0;
}
```

Another example:



```cpp
#include <iostream>
#include <functional>

// Function to be used as a callback
void myCallbackFunction(int value) {
    std::cout << "Callback function called with value: " << value << std::endl;
}

// Function that accepts a callback
void processWithCallback(std::function<void(int)> callback, int value) {
    callback(value);
}

int main() {
    processWithCallback(myCallbackFunction, 42);

    // Using std::bind
    auto boundCallback = std::bind(myCallbackFunction, std::placeholders::_1);
    processWithCallback(boundCallback, 42);

    return 0;
}
```
[code](../src/bind.cpp)


## 4. Lambda Functions
**Lambda Functions**: Anonymous functions introduced in C++11. They provide a convenient and readable way to write inline callbacks. It provides a concise way to define callbacks inline.

```cpp
 auto planer_lambda = [](double start, double goal) {
    return std::vector<double>(10, 2);
  };

myrobot.m_callback = planer_lambda;
myrobot.move(start, goal);
```

Another example:



```cpp
#include <iostream>

// Function that accepts a callback
void processWithCallback(std::function<void(int)> callback, int value) {
    callback(value);
}

int main() {
    // Using a lambda as a callback
    processWithCallback([](int value) {
        std::cout << "Callback lambda called with value: " << value << std::endl;
    }, 42);

    return 0;
}
```

## 5. Member Function Pointers
**Member Function Pointers**: Used to call member functions of a class. This is more complex and usually combined with `std::function` or `std::bind`. It is useful for callbacks in class methods.

```cpp
#include <iostream>
#include <functional>

class MyClass {
public:
    void myMemberCallback(int value) {
        std::cout << "Member callback called with value: " << value << std::endl;
    }
};

// Function that accepts a callback
void processWithCallback(std::function<void(int)> callback, int value) {
    callback(value);
}

int main() {
    MyClass obj;
    // Using std::bind to bind member function
    auto boundCallback = std::bind(&MyClass::myMemberCallback, &obj, std::placeholders::_1);
    processWithCallback(boundCallback, 42);

    return 0;
}
```

## 6. Signals and Slots
6. **Signals and Slots**: Provided by libraries like Qt, this mechanism is often used in event-driven programming. It is a robust mechanism for event-driven programming, mainly used in GUI applications with frameworks like Qt.


```cpp
// This example requires the Qt framework

#include <QCoreApplication>
#include <QObject>
#include <QDebug>

class Emitter : public QObject {
    Q_OBJECT
public:
    void emitSignal() {
        emit mySignal(42);
    }

signals:
    void mySignal(int);
};

class Receiver : public QObject {
    Q_OBJECT
public slots:
    void mySlot(int value) {
        qDebug() << "Slot called with value:" << value;
    }
};

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    Emitter emitter;
    Receiver receiver;

    QObject::connect(&emitter, &Emitter::mySignal, &receiver, &Receiver::mySlot);

    emitter.emitSignal();

    return app.exec();
}

#include "main.moc"
```
[code](../src/callbacks.cpp)


## 7. std::invoke
`std::invoke` is a generic way to activate any callable. `std::invoke` takes something callable, and arguments to call it with, and does the call.

```cpp
int sub(int a, int b) { return a - b; }
```

```cpp
  std::cout << std::invoke(sub, 2, 5) << std::endl;
```
will give you: `-3`

```cpp
  std::cout << std::invoke(sub, 5, 2) << std::endl;
```
will give you: `+3`



```cpp
  auto subFunc1 = std::bind(sub, std::placeholders::_1, std::placeholders::_2);
  auto subFunc2 = std::bind(sub, std::placeholders::_2, std::placeholders::_1);

  std::cout << subFunc1(2, 5) << std::endl;
  std::cout << subFunc2(2, 5) << std::endl;
```
will give you: `-3` and `3`.


## 8. Packaged Tasks
**Packaged Tasks**: These are template classes that encapsulate a callable object and allow its result to be retrieved asynchronously.


## 9. Coroutines 
**Coroutines**: coroutines are functions that can suspend execution to be resumed later. They are callable and can be used in asynchronous programming.


## 10. Auto-generated operator() from a Captureless Lambda
**Auto-generated operator() from a Captureless Lambda**: Captureless lambdas can be implicitly converted to a function pointer, making them callable in a variety of contexts.


Refs: [1](https://stackoverflow.com/questions/2298242/callback-functions-in-c) 








