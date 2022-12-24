#include <iostream>
// TODO
// FIXME
// https://cs.nyu.edu/courses/spring12/CSCI-GA.3033-014/Assignment1/function_pointers.html

/*

There are two ways of declaring new type aliases in modern C++. The first and
traditional one is with the typedef keyword:

1)typedef [original-type] [your-alias];
typedef int Pixel;
typedef std::map<std::string, std::vector<std::string>> Map;

2)The other one, introduced in C++11, is with the using keyword (using works
best with templates): using [your-alias] = [original-type];

using Pixel = int;
using Map   = std::map<std::string, std::vector<std::string>>;

template <typename T> using Matrix= std::vector<std::vector<T>> ;




typedef void (*Something)()

To make it a function that takes no arguments (in C), you'd use:
typedef void (*MCB)(void);

In C++:
typedef void (*MCB)();

*/

typedef void (*MCB)();

int main() {}
