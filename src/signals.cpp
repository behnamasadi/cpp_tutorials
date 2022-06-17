#include <iostream>
#include <csignal>
#include <cstdlib>// for  exit()
#include <unistd.h>// getpid()


sig_atomic_t s_value = 0;
void handle(int signum)
{
    s_value = signum;
}

void SIGSEGVhandle(int signum)
{
    std::cout << "oh my god! segmenation fault happened"<< std::endl;
    printf("Process %d got signal %d\n", getpid(), signum);

    //kill(getpid(), signum);
    exit(signum);
}


void SIGExample()
{
    //SIGILL,SIGINT,SIGSEGV,SIGTERM,SIGABRT,SIGFPE
    signal(SIGTERM, handle);
    std::cout << "Before called Signal = " << s_value << std::endl;
    raise(SIGTERM);
    std::cout << "After called Signal = " << s_value << std::endl;
}

int main()
{
    //first example
    //SIGExample();

    //second example
    signal(SIGSEGV, SIGSEGVhandle);
    //signal(SIGSEGV, SIG_DFL);  // restore default behavior
    int *p;
    *p=10;
}
