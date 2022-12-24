/*
 * MacroExample.cpp
 *
 *  Created on: Jan 4, 2012
 *      Author: behnam
 */
#include <iostream>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
// define something for Windows (32-bit and 64-bit, this part is common)
#ifdef _WIN64
// define something for Windows (64-bit only)
#else
// define something for Windows (32-bit only)
#endif
#elif __APPLE__
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
// iOS Simulator
#elif TARGET_OS_IPHONE
// iOS device
#elif TARGET_OS_MAC
// Other kinds of Mac OS
#else
#error "Unknown Apple platform"
#endif
#elif __linux__
// linux
#elif __unix__ // all unices not caught above
// Unix
#elif defined(_POSIX_VERSION)
// POSIX
#else
#error "Unknown compiler"
#endif

/*
1)First Example
*/
#define SOFTWARE "Software Version= "
#define VERSION 10

/*
2)in the CMakeLists.txt ADD_DEFINITIONS(-DLOGING=0) or
ADD_DEFINITIONS(-DLOGING=1)
*/

#if LOGING == 1
#define LOG(x) std::cout << x << " has happened" << std::endl
#else
#define LOG(x)
#endif

/*
3)Since the macro will replace the text where they used, they are like inline
functions and they will speed up code.
If you have multi line put a \
*/

#define ADD(a, b)                                                              \
  { (a + b); }

#define SWAP(a, b)                                                             \
  {                                                                            \
    (a) ^= (b);                                                                \
    (b) ^= (a);                                                                \
    (a) ^= (b);                                                                \
  }

int main() {

  std::cout << SOFTWARE << VERSION << std::endl;

  int x, y;
  x = 2;
  y = 3;
  ADD(x, y);
  SWAP(x, y);

  LOG("loading");
  return 0;
}
