// fork() — create a child process that is an (almost) identical copy of the
// parent. The code after fork() runs in BOTH processes; we tell them apart by
// the return value:
//   pID  > 0  -> parent (the value is the child's PID)
//   pID == 0  -> child
//   pID  < 0  -> fork failed
//
// Each process has its own address space, so changes a child makes to globals
// or stack variables are NOT visible to the parent.

#include <iostream>
#include <stdlib.h>     // exit()
#include <string>
#include <sys/types.h>
#include <sys/wait.h>   // wait()
#include <unistd.h>     // fork(), getpid()

int globalVariable = 2;

int main() {
  std::cout << "[pid " << getpid() << "] before fork(), runs only in parent"
            << std::endl;

  std::string sIdentifier;
  int iStackVariable = 20;

  pid_t pID = fork();
  // From here on, the code below runs in BOTH parent and child.
  // We check pID to decide which branch each process should take.

  if (pID == 0) {
    // child
    sIdentifier = "Child Process: ";
    globalVariable++;
    iStackVariable++;
  } else if (pID < 0) {
    // fork failed
    std::cerr << "Failed to fork" << std::endl;
    exit(1);
  } else {
    // parent
    sIdentifier = "Parent Process:";
  }

  // Code executed by both parent and child.
  std::cout << "[pid " << getpid() << "] " << sIdentifier
            << " global=" << globalVariable
            << " stack=" << iStackVariable << std::endl;

  // Parent waits for the child so its output isn't interleaved with the shell
  // prompt and so we don't leave a zombie behind.
  if (pID > 0) {
    wait(nullptr);
  }
}
