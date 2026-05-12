#include <cstdlib>
#include <cstring>
#include <iostream>

// malloc: allocates `size` bytes, returns pointer to the block (or nullptr).
void mallocDemo() {
  std::cout << "--- malloc ---\n";
  int *x = (int *)malloc(10 * sizeof(int));
  if (x == nullptr)
    exit(1);

  x[0] = 12;
  x[1] = 2;
  std::cout << "x[0] = " << x[0] << ", x[1] = " << x[1] << "\n";

  free(x);
}

// calloc: allocates an array of `num` elements of `size` bytes each and
// zero-initializes every byte.
void callocDemo() {
  std::cout << "--- calloc ---\n";
  int *p = (int *)calloc(4, sizeof(int));
  if (p == nullptr)
    exit(1);

  for (int n = 0; n < 4; ++n)
    std::cout << "p[" << n << "] = " << p[n] << "\n";

  free(p);
}

// realloc: resizes a previously allocated block. On shrink, existing bytes are
// preserved up to the new size. On grow, the new tail bytes are undefined.
void reallocDemo() {
  std::cout << "--- realloc ---\n";
  int *x = (int *)malloc(4 * sizeof(int));
  if (x == nullptr)
    exit(1);

  x[0] = 12;
  x[1] = 2;
  x[2] = 4;
  x[3] = 6;

  x = (int *)realloc(x, 10 * sizeof(int));
  if (x == nullptr)
    exit(1);

  x[4] = 14;
  x[5] = 61;
  std::cout << "x[0] = " << x[0] << ", x[4] = " << x[4] << ", x[5] = " << x[5]
            << "\n";

  free(x);
}

// realloc with a C string: grow the buffer, then append more text.
void reallocStringDemo() {
  std::cout << "--- realloc (string) ---\n";
  char *str = (char *)malloc(10);
  if (str == nullptr)
    exit(1);
  std::strcpy(str, "Behnam");

  str = (char *)realloc(str, 15);
  if (str == nullptr)
    exit(1);
  std::strcat(str, "Asadi");

  std::cout << "str = " << str << "\n";

  free(str);
}

int main() {
  mallocDemo();
  callocDemo();
  reallocDemo();
  reallocStringDemo();
  return 0;
}
