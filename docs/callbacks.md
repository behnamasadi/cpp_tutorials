# Callbacks

Callbacks in C++ are mechanisms that allow a piece of code to be called at a later time, often in response to some kind of event or condition. They are frequently used in event-driven programming, such as in graphical user interfaces, or for handling asynchronous operations. Here are a few common ways to implement callbacks in C++:

1. **Function Pointers:** This is the most basic form of callback in C++. A function pointer can point to a function with a matching signature and can be passed as an argument to other functions.

   ```cpp
   void MyCallbackFunction(int a) {
       // Do something
   }

   void RegisterCallback(void (*callback)(int)) {
       // Store the function pointer and call it later
       callback(10);
   }

   // Usage
   RegisterCallback(MyCallbackFunction);
   ```

2. **Functors (Function Objects):** These are objects that can be used like functions. They are instances of a class that has the `operator()` defined.

   ```cpp
   class Functor {
   public:
       void operator()(int a) {
           // Do something
       }
   };

   void RegisterCallback(Functor functor) {
       // Call the functor
       functor(10);
   }

   // Usage
   Functor myFunctor;
   RegisterCallback(myFunctor);
   ```

3. **Lambdas (since C++11):** These are anonymous functions that can be defined inline and used as callbacks. They are very powerful and flexible.

   ```cpp
   void RegisterCallback(std::function<void(int)> callback) {
       // Call the callback
       callback(10);
   }

   // Usage
   RegisterCallback([](int a) {
       // Do something
   });
   ```

4. **std::function (since C++11):** This is a standard library feature that can store and invoke any callable target—functions, lambda expressions, bind expressions, or other function objects.

   ```cpp
   void RegisterCallback(std::function<void(int)> callback) {
       // Store and call the callback
       callback(10);
   }

   // Usage
   std::function<void(int)> myCallback = [](int a) { /* Do something */ };
   RegisterCallback(myCallback);
   ```

5. **Member Function Pointers:** These are used when you need to call member functions of a class as callbacks.

   ```cpp
   class MyClass {
   public:
       void MyMemberFunction(int a) {
           // Do something
       }
   };

   void RegisterCallback(void (MyClass::*callback)(int), MyClass& obj) {
       // Call the member function
       (obj.*callback)(10);
   }

   // Usage
   MyClass obj;
   RegisterCallback(&MyClass::MyMemberFunction, obj);
   ```

Each of these methods has its own use cases and advantages. The choice depends on factors such as the need for stateful callbacks, the requirement to capture local variables, performance considerations, and the complexity of the callback logic.


A callback function is a callable passed as an argument to a class or function, used to customize the current logic depending on that callback. 

Callback functionality can be achieved in several ways:

1. std::function
2. Function pointers
3. Lambda expressions
4. Bind expressions
5. Function objects (classes with overloaded function call operator operator())

Example: imagine you have a class for modeling a robot and you want to give this freedom to the user to define and use different motion planners. You can ask the user to pass the solver function to the robot class so the robot class uses that one for moving around.

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

## std::function

```cpp
std::function<std::vector<double>(double, double)> planer2_ptr =
      std::bind(&planer2, std::placeholders::_1, std::placeholders::_2);

myrobot.m_callback = planer2_ptr;
myrobot.move(start, goal);
```

## Function pointers

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


## Lambda expressions

```cpp
 auto planer_lambda = [](double start, double goal) {
    return std::vector<double>(10, 2);
  };

myrobot.m_callback = planer_lambda;
myrobot.move(start, goal);
```

## Bind expressions

[code](../src/callbacks.cpp)

  

Refs: [1](https://stackoverflow.com/questions/2298242/callback-functions-in-c) 




