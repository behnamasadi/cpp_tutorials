#include <cmath>
#include <exception>
#include <iomanip>
#include <iostream>
#include <memory>

int main() {
  int array[5] = {0, 1, 2, 3, 4};

  std::cout << "Original array values:\n";
  for (int i = 0; i < 5; i++) {
    std::cout << "array[" << i << "] = " << array[i] << "\n";
  }

  // Overflowing the array
  // Writing beyond the bounds of the array
  std::cout << "\nWriting beyond the bounds of the array...\n";
  for (int i = 0; i < 10;
       i++) { // This loop intentionally goes well beyond the array's bounds
    array[i] = i;
  }
}