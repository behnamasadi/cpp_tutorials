# Callbacks

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




