#include <iostream>

/*

https://www.youtube.com/watch?v=Mzi5SHi13MM
https://www.youtube.com/watch?v=1gNzhE-Tn40
https://www.youtube.com/watch?v=CU3VYN6xGzM
https://www.youtube.com/watch?v=o1EvPhz6UNE
https://www.youtube.com/watch?v=VXi0AOQ0PF0

https://kevinushey.github.io/blog/2016/01/27/introduction-to-c++-variadic-templates/


1)We use typename... Ts to declare Ts as a so-called template parameter pack.
Other common names are Args, or Rest. The ellipsis (...) operator is used here
to declare that Ts truly is a template parameter pack.


2)Our function signature accepts a closely-related function parameter pack – in
other words, a bag of parameters, whose types are given by the aforementioned
template parameter pack. It is declared as Ts... ts, with the ellipsis operator
used to indicate that Ts does refer to a template parameter pack.

template <typename... Ts>  // (1)
void ignore(Ts... ts) {}   // (2)


When it comes to handling variadic functions, you can’t think in the standard
‘iterative’ C++ style. You need to write such functions recursively – with a
‘base’ case, and a ‘recursive’ case that reduces, eventually, into a ‘base’
case. This implies a separate function for each case.




*/

/////////////////////////////////First Example////////////////////////////
// The base case
template <typename T> T sum(T num) { return num; }

// The recursive case
template <typename T, typename... Args> T sum(T t, Args... args) {
  return t + sum(args...);
}

//////////////////////////////////Second Example////////////////////////////////
/*
the expression square(rest).... Recall that the ... operator will expand an
entire expression, so for example, when it’s called with square(4.0, 6.0)...,
the compiler expands this as square(4.0), square(6.0)


power_sum(2.0, 4.0, 6.0);
2.0 + power_sum(square(rest)...);
2.0 + power_sum(square(4.0), square(6.0));
2.0 + (square(4.0) + power_sum(square(rest)...))
2.0 + (square(4.0) + power_sum(square(square(6.0)));
2.0 + (square(4.0) + (square(square(6.0))))

*/

template <typename T> T square(T t) { return t * t; }

// base
template <typename T> T power_sum(T t) { return t; }

// recursive
template <typename T, typename... Args> T power_sum(T t, Args... args) {
  return t + power_sum(square(args)...);
}

int main() {

  std::cout << sum(1.0, 2.0, 3.0) << std::endl;

  std::cout << power_sum(2.0, 4.0, 6.0) << std::endl;
}
