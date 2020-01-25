/*
 *
 *  Created on: Jan 6, 2012
 *      Author: behnam
 */

#include <iostream>
#include <memory>
#include <stdexcept>
#include <sys/resource.h>
/*
1)Syntax errors:

A syntax error occurs when you write a statement that is not valid according to the grammar of the C++ language.
For example:
if 5 > 6 then write "not equal";

if (5 > 6)
    std::cout << "not equal";


2)Semantic errors:

A semantic error occurs when a statement is syntactically valid, but does not do what the programmer intended. For example:

for (int nCount=0; nCount<=3; nCount++)
    std::cout << nCount << " ";

The programmer may have intended this statement to print 0 1 2, but it actually prints 0 1 2 3.
*/

/*
---------------------------------
|                               |  <-heap
|                               |
|               z               |
|                               |
|                               |
|                               |
|                               |
|-------------------------------|
|                               |  <-stack
|                               |
|-------------------------------|
|            func2()            |  <-stack frame3
|             a,b               |
|-------------------------------|
|            func1()            |  <-stack frame2
|            a,b                |
|--------------------------------
|            main()             |  <-stack frame1
|             x,y               |
|                               |
|-------------------------------|
|             sum               | <- static/global vaiables
|-------------------------------|
|          code(Text)           |
|            main               |
|                               |
---------------------------------




heap:
    grow downward

The memory size for these three section is fixed and it is not growing
code:
    code of application

static/global vaiables:
    are avaiable during whole life cycle of application

stack:
    local variables/ function calls


*/


/*
When you access an array index, C and C++ don't do bound checking. Segmentation faults only happen when you try to
read or write to a page that was not allocated (or try to do something on a page which isn't permitted,
e.g. trying to write to a read-only page), but since pages are usually pretty big
(multiples of a few kilobytes), it often leaves you with lots of room to overflow.

If your array is on the stack, it can be even worse as the stack is usually pretty large (up to several megabytes).
 This is also the cause of security concerns: writing past the bounds of an array on the stack may overwrite the return address of the function
and lead to arbitrary code execution (the famous "buffer overflow" security breaches).

By setting the follwong flag you can find the issue:
set(CMAKE_CXX_FLAGS "-fsanitize=address ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "-fno-omit-frame-pointer ${CMAKE_CXX_FLAGS}")


*/


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
