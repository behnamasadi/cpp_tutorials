#include <iostream>
#include <stdlib.h> // Declaration for exit()
#include <string>
#include <sys/types.h>
#include <unistd.h>

int globalVariable = 2;

int main() {
  std::cout << "The code before fork(), it runs only by parent" << std::endl;
  std::string sIdentifier;
  int iStackVariable = 20;

  pid_t pID = fork();
  /*
  after calling fork function, the below code would be executed by both process,
  parent and child to discriminet which process should do what, we check pID
  */
  if (pID == 0) // child
  {
    // Code only executed by child process

    sIdentifier = "Child Process: ";
    globalVariable++;
    iStackVariable++;
  } else if (pID < 0) // failed to fork
  {
    std::cerr << "Failed to fork" << std::endl;
    exit(1);
    // Throw exception
  } else // parent
  {
    // Code only executed by parent process

    sIdentifier = "Parent Process:";
  }

  // Code executed by both parent and child.
  pid_t pid = getpid();
  std::cout << sIdentifier << " pid:  " << pid << std::endl;
  std::cout << " Global variable: " << globalVariable;
  std::cout << " Stack variable: " << iStackVariable << std::endl;
}
