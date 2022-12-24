
void dangling_pointer() {
  unsigned char *x = new unsigned char[5];
  delete[] x;
  x[0] = 1;
}

void memory_leak() { unsigned char *x = new unsigned char[5]; }

void heap_buffer_overflow() {
  unsigned char *x = new unsigned char[5];
  for (int i = 0; i < 10; i++) {
    x[i]++;
  }
  delete[] x;
}

void stack_buffer_overflow() {
  char x[5];
  for (int i = 0; i < 10; i++) {
    x[i]++;
  }
}

void stack_overflow() { int nStack[100000000]; }

int main() {
  // dangling_pointer();
  // memory_leak();
  // heap_buffer_overflow();
  // stack_buffer_overflow();
  // stack_overflow();
}
