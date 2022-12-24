#include <functional>
#include <iostream>

/*
Class template std::function is a general-purpose polymorphic function wrapper.
Instances of std::function can store, copy, and invoke any Callable target:

functions
lambda expressions
bind expressions
functors

*/
// template<typename T>
// void print(T value)
void print(int value) { std::cout << value << std::endl; }

int solver1(int a, int b) { return a + b; }

int solver2(int a, int b) { return a - b; }

int planner(int (*fn_solver)(int, int)) { return fn_solver(10, 12); }

int main() {
  // storing a functions
  {
    std::function<void(int)> funcPrint = print;
    print(3);
  }

  // store a bind expressions
  {
    std::function<void(int)> funcPrint =
        std::bind(&print, std::placeholders::_1);

    std::function<void(int)> f(funcPrint);
    f(3);
  }

  // store lambda expressions
  {
    auto lambda = [](int value) { std::cout << value << std::endl; };
    std::function<void(int)> f(lambda);
    f(3);
  }

  // store a functors
  {
    void (*printfunctor)(int);
    printfunctor = print;
    std::function<void(int)> f(printfunctor);
    f(3);
  }
  /*
  send a function as parameter to an other function, here in this case planner
  might use various solver for planning
  */
  {
    std::cout << "solver1" << std::endl;
    planner(solver1);
    std::cout << "solver2" << std::endl;
    planner(solver2);
  }

  // function pointer
  {
    auto func_ptr1 =
        std::bind(&solver1, std::placeholders::_1, std::placeholders::_2);
    std::function<int(int, int)> func_ptr2 =
        std::bind(&solver1, std::placeholders::_1, std::placeholders::_2);

    std::cout << func_ptr1(1, 2) << std::endl;
    std::cout << func_ptr2(1, 2) << std::endl;
  }
}
