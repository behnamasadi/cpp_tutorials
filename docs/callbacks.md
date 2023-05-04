# Callbacks

A callback function is a callable passed as an argument to a class or function, used to customize the current logic depending on that callback. 
For instance imagine you have a class for modeling a robot and you want to give this freedom to user to define and use different motion planner. You can ask the user to pass the solver function to the robot class so the robot class use that one for moving around.

```cpp
typedef std::function<std::vector<double>(double, double)> CallbackFunction;

class robot {

public:
  CallbackFunction m_callback;

  void makeMotion(std::vector<double> trajectory) {
    std::cout << "robot is traversing the generated trajectory:" << std::endl;
    for (const auto &p : trajectory)
      std::cout << p << std::endl;
  }

  void move(double start, double goal) {

    std::vector<double> trajectory = m_callback(start, goal);
    makeMotion(trajectory);
  }
};
```

Now we can have different solvers:

```cpp
std::vector<double> planer1(double start, double goal) {
  return {start, (start + goal) / 2, goal};
}
std::vector<double> planer2(double start, double goal) {
  return {start, start + (start + goal) / 3, 2 * (start + goal) / 3, goal};
}
```
Now we can assign and use different solvers:

```cpp
robot myrobot1;
myrobot1.m_callback = planer1;
myrobot1.move(start, goal);
```
  
or 

```cpp
std::function<std::vector<double>(double, double)> planer2_ptr =
      std::bind(&planer2, std::placeholders::_1, std::placeholders::_2);

myrobot1.m_callback = planer2_ptr;
myrobot1.move(start, goal);
```

or

```cpp
 auto planer_lambda = [](double start, double goal) {
    return std::vector<double>(10, 2);
  };

  myrobot1.m_callback = planer_lambda;
  myrobot1.move(start, goal);
```
[code](../src/callbacks.cpp)

  

Refs: [1](https://stackoverflow.com/questions/2298242/callback-functions-in-c) 




