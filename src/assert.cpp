#include <iostream>
// uncomment to disable assert()
//#define NDEBUG
#include <cassert>
#include <climits>
#include <cmath>

int assertExample() {
  int idx = 2;
  int array[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  // we're asserting that idx is between 0 and 9
  assert((idx >= 0 && idx <= 9) && "idx should be between 0 and 9");

  assert(("idx should be between 0 and 9", (idx >= 0 && idx <= 9)));

  return array[idx];
}

void static_assertExample() {
  static_assert(sizeof(unsigned int) * CHAR_BIT == 32);
  static_assert(CHAR_MIN < 0);
  static_assert(-5 / 2 == -2);
  // static_assert(__cplusplus > 201103L);
}

void NDEBUGExample() {
  double x = +1.0;
  assert(x >= 0.0);
  printf("sqrt(x) = %f\n", sqrt(x));
}

int main(int argc, char *argv[]) {
  // assertExample();
  // static_assertExample();
  // NDEBUGExample();
  return 0;
}
