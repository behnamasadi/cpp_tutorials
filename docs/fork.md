# fork
The `fork()` system call will spawn a new child process which is an identical process to the parent and returns a process ID. The purpose of `fork()` is to create a child process for the caller.
After calling fork function, the code below the `fork()` would be executed by both process, parent and child. To discriminate which process should do what, we check pID



```cpp
int globalVariable = 2;

int main()
{
    std::cout<<"The code before fork(), it runs only by parent" <<std::endl;
    std::string sIdentifier;
    int    iStackVariable = 20;

    pid_t pID = fork();
    /*
    after calling fork function, the below code would be executed by both process, parent and child
    to discriminet which process should do what, we check pID
    */
    if (pID == 0)                // child
    {
        // Code only executed by child process

        sIdentifier = "Child Process: ";
        globalVariable++;
        iStackVariable++;
    }
    else if (pID < 0)            // failed to fork
    {
        std::cerr << "Failed to fork" << std::endl;
        exit(1);
        // Throw exception
    }
    else                                   // parent
    {
    // Code only executed by parent process

        sIdentifier = "Parent Process:";
    }

    // Code executed by both parent and child.

    pid_t pid = getpid();
    std::cout <<sIdentifier<<" pid:  "<< pid<<std::endl;
    std::cout << " Global variable: " << globalVariable;
    std::cout << " Stack variable: "  << iStackVariable << std::endl;
}
```

Refs: [1](http://www.yolinux.com/TUTORIALS/ForkExecProcesses.html), [2](http://www.csl.mtu.edu/cs4411.ck/www/NOTES/process/fork/create.html)


[code](fork.cpp)
