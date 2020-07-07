#include <iostream>
#include <memory>
#include <stdexcept>
#include <sys/resource.h>
/*
=================== Memory Layout of C Programs ===================
 1)Text
     code and data (variable, functions parameters,...) are stored in separate memory. All codes in the functions
     go into the code (Text) region.
 2)Data
    global/static initialized data
 3)BSS
    global/static uninitialized data
 4) Heap
 5) Stack
 It is a dynamic data structure maintained by OS to control the way procedure calling each other and parameter they pass.
 Call stack maintained for each thread. The actual implementation of a stack depends on the microprocessor architecture.
 It can grow up or down in memory and can move either before or after the push/pop operations

 stack is used for functions. It is on top of the memory and moves downward. each function will get its own frame.
 Each instance of the function has its own frame. Data that go into frame are:
 1)Arguments
 2)Local variables (object, data structure)
 3)Saved registers.
 In high level languages stack will be managed automatically. In low level languages
 it is being done with stack pointer and  frame pointer.
 Stack pointer:It tell us where is the boundary between allocated and unallocated memory is.
 Frame pointer: is a reference pointer allowing us to know where local variable or an argument starts.

 ---------------------------------
 |   command line arguments      |
 |   argv[0], argv[1] ...argv[n] |
 ---------------------------------
 |       Allocated memory        |
 |-------------------------------|<--- frame pointer      ┐
 |        Return Address         |                        |
 |          Arguments            |                        |  stack frame1
 |        Local Variables        |                        |
 |        Saved registers        |                        |
 |-------------------------------|<---- stack pointer     ┘
 |                               |
 |                               |  <-stack frame2
 |-------------------------------|
 |                               |
 |                               |  <-stack frame3
 |-------------------------------|
 |                               |
 |                               |
 |                               |
 |                               |
 |                               |
 |                               |
 |              HEAP             |
 |                               |
 |-------------------------------|            ┐
 |  global/static uninitialized  |            |
 |             data              |            |<- BSS
 |                               |            |
 |-------------------------------|            ┘  ┐
 | global/static initialized data|               |<-  Data
 |-------------------------------|               ┘
 |          Code(Text)           |
 |                               | <- Program counter point to line in this region
 |                               |
 ---------------------------------
 |          Reserved             |
 |                               |
 ---------------------------------
The "size" command reports the sizes (in bytes) of the Text, Data, and BSS segments.
int main(void)
{
    return 0;
}
Output of size:
text       data        bss
960        248          8
int global_var;
int main(void)
{
    return 0;
}
Output of size: (because of "global_var" 4 bytes added to bss)
text       data        bss
 960        248         12
int global_var;
int main(void)
{
    static int i;
    return 0;
}
Output of size: (because of "i" 4 bytes added to bss)
text       data        bss
 960        248         16
int main(void)
{
    static int i = 100;
    return 0;
}
Output of size: (because "i" initialized 4 bytes added removed from bss and added to data)
text       data        bss
 960        252         12
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
=================== Stack overflow ===================
If the call stack pointer exceeds the stack bound, because of:
Infinite recursion:
int infiniteRecursion()
{
     return infiniteRecursion();
}
Very large stack variables
int largeStackVariables()
{
     double x[1000000000];
}
=================== Memory segmentation ===================
 Memory segmentation is dividing main memory into segments. using segmentation, a reference to a memory location
 includes a value that identifies a segment and an offset (memory location) within that segment.
 Segmentation was originally invented as a method by which system software could isolate different processes and data
 they are using. It was intended to increase reliability of the systems running multiple processes simultaneously.
 The size of a memory segment is generally not fixed.
 In a x86-64 architecture it is considered legacy and most x86-64-based modern system software don't use memory segmentation.
 Instead they handle programs and their data by utilizing memory-paging which also serves as a way of memory protection.
 However most x86-64 implementations still support it for backward compatibility reasons
 Each segment has a length and set of permissions (for example, read, write, execute) associated with it.
 A process is only allowed to make a reference into a segment if the type of reference is allowed by the permissions,
 and if the offset within the segment is within the range specified by the length of the segment.
 Otherwise, a hardware exception such as a segmentation fault is raised.
 Segmentation has been implemented in several different ways on different hardware, with or without paging.
 1)Segmentation without paging
 2)Segmentation with paging
===================  Paging (x86-64 architecture) ===================
Process
A process is just a running  program. The execution of a process must progress in a sequential fashion.
Process Life Cycle
Start
This is the initial state when a process is first started/created.
Ready
The process is waiting to be assigned to a processor. Ready processes are waiting to have the processor
allocated to them by the operating system so that they can run. Process may come into this state after
Start state or while running it by but interrupted by the scheduler to assign CPU to some other process.
Running
Once the process has been assigned to a processor by the OS scheduler, the process state is set to running and the
processor executes its instructions.
Waiting
Process moves into the waiting state if it needs to wait for a resource, such as waiting for user input,
or waiting for a file to become available.
Terminated or Exit
Once the process finishes its execution, or it is terminated by the operating system, it is moved to the
terminated state where it waits to be removed from main memory.
Start --🡪Ready 🡨--------🡪 Running----🡪Terminated
            \            🡭
             \          /
              \        /
               🡮Wait 🡭
Process Control Block (PCB)
A Process Control Block is a data structure maintained by the Operating System for every process.
The PCB is identified by an integer process ID (PID). A PCB keeps all the information needed to keep track of a process:
1) Process State: The current state of the process:
New (Create) – In this step, the process is about to be created but not yet created, it is the program which is present in secondary memory that will be picked up by OS to create the process.
Ready – New -> Ready to run. After the creation of a process, the process enters the ready state i.e. the process is loaded into the main memory. The process here is ready to run and is waiting to get the CPU time for its execution. Processes that are ready for execution by the CPU are maintained in a queue for ready processes.
Run – The process is chosen by CPU for execution and the instructions within the process are executed by any one of the available CPU cores.
Blocked or wait – Whenever the process requests access to I/O or needs input from the user or needs access to a critical region(the lock for which is already acquired) it enters the blocked or wait state. The process continues to wait in the main memory and does not require CPU. Once the I/O operation is completed the process goes to the ready state.
Terminated or completed – Process is killed as well as PCB is deleted.
Suspend ready – Process that was initially in the ready state but were swapped out of main memory(refer Virtual Memory topic) and placed onto external storage by scheduler are said to be in suspend ready state. The process will transition back to ready state whenever the process is again brought onto the main memory.
Suspend wait or suspend blocked – Similar to suspend ready but uses the process which was performing I/O operation and lack of main memory caused them to move to secondary memory.
When work is finished it may go to suspend ready.
2) Process privileges: user mode vs kernel mode.
User mode
When the computer system run user applications like creating a text document or using any application program,
then the system is in the user mode. When the user application requests for a service from the operating system
or an interrupt occurs or system call, then there will be a transition from user to kernel mode to fulfill the requests.
Kernel Mode
When the system boots, hardware starts in kernel mode and when operating system is loaded, it start user application in user mode.
To provide protection to the hardware, we have privileged instructions which execute only in kernel mode.
If user attempt to run privileged instruction in user mode then it will treat instruction as illegal and traps to OS.
Some of the privileged instructions are:
Handling Interrupts
To switch from user mode to kernel mode.
Input-Output management.
3) Process ID Unique identification for each of the process in the operating system.
4) Pointer A pointer to parent process.
5) Program Counter: Program Counter is a pointer to the address of the next instruction to be executed for this process.
6) CPU registers: Various CPU registers where process need to be stored for execution for running state.
7) CPU Scheduling Information Process: priority and other scheduling information which is required to schedule the process.
8) Memory management information: This includes the information of page table, memory limits, Segment table depending on memory used by the operating system.
9) Accounting information: This includes the amount of CPU used for process execution, time limits, execution ID etc.
10) IO status information: This includes a list of I/O devices allocated to the process.
CPU Scheduling in Operating Systems
The OS maintains all PCBs in Process Scheduling Queues. The OS maintains a separate queue for each of the process
states and PCBs of all processes in the same execution state are placed in the same queue.
When the state of a process is changed, its PCB is unlinked from its current queue and moved to its new state queue.
The Operating System maintains the following important process scheduling queues −
Job queue − This queue keeps all the processes in the system.
Ready queue − This queue keeps a set of all processes residing in main memory, ready and waiting to execute. A new process is always put in this queue.
Device queues − The processes which are blocked due to unavailability of an I/O device constitute this queue.
▴▾
Types of schedulers:
Long term
Short term
Medium term
CPU Scheduling in Operating Systems
Arrival Time: Time at which the process arrives in the ready queue.
Completion Time: Time at which process completes its execution.
Burst Time: Time required by a process for CPU execution.
Turn Around Time: Time Difference between completion time and arrival time.
Turn Around Time = Completion Time – Arrival Time
Waiting Time(W.T): Time Difference between turn around time and burst time.
Waiting Time = Turn Around Time – Burst Time
Different Scheduling Algorithms
In segmented memory management system scheme, each process is atomic, it can not be split up, it either fully
in the memory or not. Segments are swapped between disc and main memory. Segments vary in size. The OS
knows start and physical addresses of each segment. Segmentation can result in externally memory fragmentation.
Logical Memory, Physical Memory
A program view of the memory is called logical memory. Each program is divided into equal size pages.
Each page might be anywhere in the physical memory or even in the swap. To make it possible, OS maintains a
page table, that maps which logical correspond to which physical location.
If there not enough space on the memory, some pages might be move into the swap.
Paged memory can led to fragmented process which run more slowly, but make better use of space.
Paging may lead to internal fragmentation, segmentation may lead to external fragmentation.
 Processes              Logical Memory                            Physical Memory
┌-------------┐         ┌-------------┐                          ┌-------------┐
|  process1   |         |--process1---|                          |--process3---|
└-------------┘         |--process1---|                         /|-------------|
                        |-------------|                        / |-------------|
┌-------------┐         |-------------|     Page Table        /  |-------------|
|  process2   |         |-------------|    ┌---------┐       /   |-------------|           Virtual Memory Swap
|             |         |-------------|    |---------|      /    |-------------|             ┌-------------┐
|             |         |-------------|    |---------|     /     |-------------|             |-------------|
└-------------┘         |-------------|    |---------|    /      |-------------|             |-------------|
                        |-------------|    |---------|   /       |-------------|             |-------------|
┌-------------┐         |-------------|   /|---------|--/--------|-------------|-------------|--process3---|
|             |         |-------------|  //|---------|\/         |-------------|             |-------------|
|             |         |-------------| ///|---------|/ \        |-------------|             └------------ ┘
|  process3   |         |--process3---|////|---------|----\------|--process3---|
└-------------┘         |--process3---|/// |---------|      \    |-------------|
                        |--process3---|//  |---------|        \  |-------------|
┌-------------┐         |--process3---|/   └---------┘          \|--process3---|
|  process4   |         └-------------┘                          └------------ ┘
└-------------┘
 In this scheme, the operating system retrieves data from secondary storage (disk) in same-size blocks called pages
 Paging is a memory management scheme that allows a process to be stored in a memory in a non-contiguous manner.
 Storing process in a non-contiguous manner solves the problem of external fragmentation.
 For implementing paging the physical and logical memory spaces are divided into the same fixed-sized blocks.
 These fixed-sized blocks of physical memory are called frames, and the fixed-sized blocks of logical memory are called pages.
 List of Process Attributes:
 1)Process ID
 2)Process Counter: after resuming the process, from which line should we continue
 3)Process State:
 4)
 5)
 6)
 7)
 8)
 https://www.youtube.com/watch?v=qlH4-oHnBb8
 https://www.youtube.com/watch?v=dz9Tk6KCMlQ
 https://www.youtube.com/watch?v=xD5PB_g1rIE
 https://en.wikipedia.org/wiki/Paging#Ferranti_Atlas
 https://en.wikipedia.org/wiki/Virtual_address_space
 https://en.wikipedia.org/wiki/Page_(computer_memory)
 https://en.wikipedia.org/wiki/Page_replacement_algorithm
===================  Segmentation fault  ===================
The following are some typical causes of a segmentation fault:
Attempting to access a nonexistent memory address (outside process's address space)
Attempting to access memory the program does not have rights to (such as kernel structures in process context)
Attempting to write read-only memory (such as code segment)
These often happens while dereferencing or assigning null pointer/ wild pointer/ dangling pointer, heap overflow, stack overflow
When you access an array index, C and C++ don't do bound checking. Segmentation faults only happen when you try to
read or write to a page that was not allocated (or try to do something on a page which isn't permitted,
e.g. trying to write to a read-only page), but since pages are usually pretty big
(multiples of a few kilobytes), it often leaves you with lots of room to overflow.
By setting the followings flag you can find the issue:
set(CMAKE_CXX_FLAGS "-fsanitize=address ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "-fno-omit-frame-pointer ${CMAKE_CXX_FLAGS}")
┌┐
└┘
 */



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


#include <iostream>

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
