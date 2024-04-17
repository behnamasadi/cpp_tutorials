#include <cmath>
#include <exception>
#include <iomanip>
#include <iostream>
#include <memory>

int main() {
  // double number = 3.1914;

  // std::cout << "default std::cout.precision(): " << std::cout.precision()
  //           << "\n";

  // std::cout << number << "\n";
  // std::cout << "std::setprecision(1): " << std::setprecision(1) << number
  //           << "\n";

  // std::cout << "std::fixed <<std::setprecision(7): " << std::fixed
  //           << std::setprecision(7) << number << "\n";

  int n = 10;

  std::cout << "Using pow : " << pow(10, n) << std::endl;
  std::cout << "std::fixed <<" << std::fixed << pow(10, n) << std::endl;
}