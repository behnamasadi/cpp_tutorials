#include <iostream>
#include <memory>
#include <stdexcept>
#include <iostream>

#if defined(__GNUC__) || defined(__GNUG__)
#include <sys/resource.h>
#endif




/*================== Segmentation fault Example  ==================*/

void writingToReadOnlyMemory()
{
/*
This occurs when the program writes to part of its own code segment or the read-only portion of the data segment.
In the following the string "hello world" and constant variables is placed in the read-only section of the data segment.
*/
    char* s = "hello world";
    *s = 'H';
}

void stackBufferOverflowExample()
{
    char x[3] = "ab";// a,b and 0 for termination
    int y = 23;
/*
variable name  x[0]	x[1] x[2]        y
value	       'a'	 'b'   0	    23
*/
    std::cout << x << std::endl;
    std::cout << y << std::endl;
   for (int i = 0; i < 100; i++)
   {
            x[i] = x[i] + 1;
   }
/*
variable name  x[0]	x[1] x[2]        y
value	       'b'	 'c'   1	    unkown
*/
    std::cout << x << std::endl;
    std::cout << y << std::endl;
}

void heapBufferOverflowExample()
{
    char *x= new char[3];
    for (int i = 0; i < 100; i++)
    {
        x[i] = x[i] + 1;
    }
}

void nullPointerDereference()
{
    int* ptr = nullptr;
    std::cout<<*ptr;
}

void wildPointerExample()
{

}

void danglingPointerExample()
{
}

/*================== Segment/ Page/ Stack size  ==================*/
#ifdef _WIN32
#include <windows.h>

#elif __unix__ // all unices, not all compilers
    // Unix
#elif __linux__

#elif __APPLE__
    // Mac OS, not sure if this is covered by __posix__ and/or __unix__ though...
#endif



size_t getMemoryPageSize()
{
#ifdef _WIN32
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    printf("%s %d\n\n", "PageSize[Bytes] :", sysInfo.dwPageSize);
    return 0;
#elif __unix__ // all unices, not all compilers
    // Unix
#elif __linux__
    size_t n;
    char* p;
    int u;
    for (n = 1; n; n *= 2) {
        p = mmap(0, n * 2, PROT_NONE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        if (p == MAP_FAILED)
            return -1;
        u = munmap(p + n, n);
        munmap(p, n * 2);
        if (!u)
            return n;
    }
    return -1;
#elif __APPLE__
    // Mac OS, not sure if this is covered by __posix__ and/or __unix__ though...
#endif
    return 1;
}

std::size_t getStackPageSize() { return 0; }



void heapBufferOverflow()
{
    int *x=new int[3];
    for(int i=0;i<100000000;i++)
    {
        std::cout<<"i is: "<<i <<std::endl;
        std::cout<< x[i] <<std::endl;
    }
}

void stackBufferOverflow()
{
    char x[2];
    for(int i=0;i<100000000;i++)
    {
        std::cout<<"i is: "<<i <<std::endl;
        std::cout<< x[i] <<std::endl;
    }

//    Here is an example program that causes a stack overflow:
//    int nStack[100000000];

}

#if defined(__GNUC__) || defined(__GNUG__)

void stackSize()
{
    //https://www.quora.com/How-can-I-estimate-stack-size-by-writing-a-program

/*
you can call ulimit -s is shell:
$ ulimit -s
8192
The output of "lim.rlim_cur" is 8388608 which is 8192*1024
*/
    std::cout<<"Stack Size" <<std::endl;
    struct rlimit lim;
    getrlimit( RLIMIT_STACK, &lim );
    std::cout<<"The current (soft) limit: "<< lim.rlim_cur <<std::endl;
    std::cout<<"The hard limit: "<<lim.rlim_max <<std::endl;

}
#endif

/*
stack frame size:
If I run this program (with no command-line arguments) on my machine,
I get a segfault after around 261978 stack frames. The stack size limit on my system is 8192 kilobytes.
So, each stack frame in this case must be around 32 bytes.
int main(int argc, char **argv)
{
    static int frame_count = 0;
    printf("%d\n", frame_count++);
    main(argc, argv);
    return 0;
}
*/

int main( int argc, char *argv[] )
{
    //assertExample();
    //stackBufferOverflow();
    //heapBufferOverflow();
}
