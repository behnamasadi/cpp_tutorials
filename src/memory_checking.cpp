// Demos for valgrind --tool=memcheck (see docs/memory_leaking_valgrind.md).
// Each function contains a tiny, obvious bug that Memcheck would report.
// Dangerous demos are gated behind a CLI flag so a plain run does not crash.
//
//   ./memory_checking                 # describes each bug, leaks 5 bytes
//   ./memory_checking dangling        # actually triggers use-after-free
//   ./memory_checking heap_overflow   # actually triggers heap overflow
//   ./memory_checking stack_overflow  # actually triggers stack buffer overflow
//   ./memory_checking blow_stack      # actually blows the stack

#include <cstring>
#include <iostream>

// Memcheck: "definitely lost: 5 bytes in 1 blocks".
void memory_leak() {
  unsigned char *x = new unsigned char[5];
  (void)x; // never deleted
}

// Memcheck: "Invalid write of size 1" at x[0] = 1 after delete[].
void dangling_pointer() {
  unsigned char *x = new unsigned char[5];
  delete[] x;
  x[0] = 1;
}

// Memcheck: "Invalid write of size 1" once i reaches 5.
void heap_buffer_overflow() {
  unsigned char *x = new unsigned char[5];
  for (int i = 0; i < 10; i++) {
    x[i]++;
  }
  delete[] x;
}

// Memcheck flags this less reliably (stack), but the bug is the same shape.
void stack_buffer_overflow() {
  char x[5];
  for (int i = 0; i < 10; i++) {
    x[i]++;
  }
}

// Allocating ~400 MB on the stack: crashes with a stack overflow.
void stack_overflow() {
  int nStack[100000000];
  nStack[0] = 0;
}

int main(int argc, char *argv[]) {
  std::cout << "memory_leak: allocates 5 bytes with new[] and never deletes.\n"
            << "  valgrind reports: 5 bytes definitely lost.\n";
  memory_leak();

  std::cout << "\ndangling_pointer: writes to x[0] after delete[] x.\n"
            << "  valgrind reports: Invalid write of size 1.\n"
            << "  pass 'dangling' to actually run it.\n";

  std::cout << "\nheap_buffer_overflow: writes x[0..9] into a new[5] buffer.\n"
            << "  valgrind reports: Invalid write past end of heap block.\n"
            << "  pass 'heap_overflow' to actually run it.\n";

  std::cout << "\nstack_buffer_overflow: writes x[0..9] into char x[5].\n"
            << "  pass 'stack_overflow' to actually run it.\n";

  std::cout << "\nstack_overflow: declares int nStack[100000000] on the stack.\n"
            << "  pass 'blow_stack' to actually run it.\n";

  if (argc > 1) {
    if (std::strcmp(argv[1], "dangling") == 0) {
      dangling_pointer();
    } else if (std::strcmp(argv[1], "heap_overflow") == 0) {
      heap_buffer_overflow();
    } else if (std::strcmp(argv[1], "stack_overflow") == 0) {
      stack_buffer_overflow();
    } else if (std::strcmp(argv[1], "blow_stack") == 0) {
      stack_overflow();
    }
  }

  return 0;
}
