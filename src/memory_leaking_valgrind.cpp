/*

disable the followings first:
#set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
#set(CMAKE_CXX_FLAGS "-fsanitize=address ${CMAKE_CXX_FLAGS}")
#set(CMAKE_CXX_FLAGS "-fno-omit-frame-pointer ${CMAKE_CXX_FLAGS}")

valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./memory_leaking_valgrind


*/
#include <stdlib.h>
#include <iostream>

void heapBufferOverflow()
{
    int *x=new int[3];
    for(int i=0;i<100000000;i++)
    {
        x[i]++;
    }
}

void stackBufferOverflow()
{
    char x[2];
    for(int i=0;i<100000000;i++)
    {
        x[i]++;
    }

//    Here is an example program that causes a stack overflow:
//    int nStack[100000000];


}

void memoryLeaking()
{
    char *x = new char[10]; /* or, in C, char *x = malloc(100) */
//    delete[] x;
}

int main()
{
    heapBufferOverflow();
    stackBufferOverflow();
    memoryLeaking();
    return 0;
}
