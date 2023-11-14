#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void signalHandler(int signum) {
  std::cout << "Caught signal " << signum << ", generating core dump."
            << std::endl;
  // Perform cleanup or logging if necessary
  signal(signum, SIG_DFL); // Reset to default handler
  raise(signum);           // Generate core dump
}

int main() {
  signal(SIGSEGV, signalHandler);
  // Your code here

  // Example: cause segmentation fault for demonstration
  int *p = nullptr;
  *p = 10;

  return 0;
}