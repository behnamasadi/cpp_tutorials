- [Process](#process)
  * [Signals](#signals)
- [Process vs Thread](#process-vs-thread)
- [Inter Process Communication (IPC)](#inter-process-communication--ipc-)
- [Printing Process Tree](#printing-process-tree)
- [Process Life Cycle](#process-life-cycle)
- [Process Control Block (PCB)](#process-control-block--pcb-)

  
# Process 
A process is just a running  program. The execution of a process must progress in a sequential fashion.

## Signals
Signals in computers are a way of communication between the process and the OS.
When a running program undergoes some serious error then the OS sends a signal to
the process and the process further may not execute.
Some processes may have a signal handler which does some important tasks before the
process leaves the CPU.
List of signals: 
`SIGILL, SIGSEGV, SIGABRT, SIGFPE, SIGBUS, SIGSYS, SIGTRAP, SIGTERM`

[more about signals](signals.md)

# Process vs Thread
There are two model of concurrent programming, multi processing and multi threading.
processes communicate with each other via **Inter Process Communication (IPC)**.

# Inter Process Communication (IPC)
Processes can communicate with each other through:
1. Shared Memory
2. Message passing
3. Socket 
4. Message queue
5. Anonymous pipe
6. Message passing


Threads communicate via shared memory. Multi threading program can not be run over distributed system (unless your threads communicate like multi 
process program), however multi processing program can.  

Refs: [1](https://www.geeksforgeeks.org/inter-process-communication-ipc/)  


# Printing Process Tree

In Linux/Unix OS you can use the following command to get the process tree:

`ps -aef --forest` 

`ps f -g<PID>`


# Process Life Cycle
1. Start
This is the initial state when a process is first started/created.

2. Ready
The process is waiting to be assigned to a processor. Ready processes are waiting to have the processor
allocated to them by the operating system so that they can run. Process may come into this state after
Start state or while running it by but interrupted by the scheduler to assign CPU to some other process.

3. Running
Once the process has been assigned to a processor by the OS scheduler, the process state is set to running and the processor executes its instructions.

4. Waiting
Process moves into the waiting state if it needs to wait for a resource, such as waiting for user input,
or waiting for a file to become available.

5. Terminated or Exit
Once the process finishes its execution, or it is terminated by the operating system, it is moved to the
terminated state where it waits to be removed from main memory.

```
Start --🡪Ready 🡨--------🡪 Running----🡪Terminated
            \            🡭
             \          /
              \        /
                                     🡮Wait 🡭
```
             

# Process Control Block (PCB)
A Process Control Block is a data structure maintained by the Operating System for every process.
The PCB is identified by an integer process ID (PID). A PCB keeps all the information needed to keep track of a process:
1. Process State: The current state of the process:
New (Create) – In this step, the process is about to be created but not yet created, it is the program which is present in secondary memory that will be picked up by OS to create the process.
Ready – New -> Ready to run. After the creation of a process, the process enters the ready state i.e. the process is loaded into the main memory. The process here is ready to run and is waiting to get the CPU time for its execution. Processes that are ready for execution by the CPU are maintained in a queue for ready processes.
Run – The process is chosen by CPU for execution and the instructions within the process are executed by any one of the available CPU cores.
Blocked or wait – Whenever the process requests access to I/O or needs input from the user or needs access to a critical region(the lock for which is already acquired) it enters the blocked or wait state. The process continues to wait in the main memory and does not require CPU. Once the I/O operation is completed the process goes to the ready state.
Terminated or completed – Process is killed as well as PCB is deleted.
Suspend ready – Process that was initially in the ready state but were swapped out of main memory(refer Virtual Memory topic) and placed onto external storage by scheduler are said to be in suspend ready state. The process will transition back to ready state whenever the process is again brought onto the main memory.
Suspend wait or suspend blocked – Similar to suspend ready but uses the process which was performing I/O operation and lack of main memory caused them to move to secondary memory.
When work is finished it may go to suspend ready.
2. Process privileges: user mode vs kernel mode.
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
3. Process ID Unique identification for each of the process in the operating system.
4. Pointer A pointer to parent process.
5. Program Counter: Program Counter is a pointer to the address of the next instruction to be executed for this process.
6. CPU registers: Various CPU registers where process need to be stored for execution for running state.
7. CPU Scheduling Information Process: priority and other scheduling information which is required to schedule the process.
8. Memory management information: This includes the information of page table, memory limits, Segment table depending on memory used by the operating system.
9. Accounting information: This includes the amount of CPU used for process execution, time limits, execution ID etc.
10. IO status information: This includes a list of I/O devices allocated to the process.
CPU Scheduling in Operating Systems
The OS maintains all PCBs in Process Scheduling Queues. The OS maintains a separate queue for each of the process
states and PCBs of all processes in the same execution state are placed in the same queue.
When the state of a process is changed, its PCB is unlinked from its current queue and moved to its new state queue.
The Operating System maintains the following important process scheduling queues −
Job queue − This queue keeps all the processes in the system.
Ready queue − This queue keeps a set of all processes residing in main memory, ready and waiting to execute. A new process is always put in this queue.
Device queues − The processes which are blocked due to unavailability of an I/O device constitute this queue.



```

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

```

In this scheme, the operating system retrieves data from secondary storage (disk) in same-size blocks called pages
Paging is a memory management scheme that allows a process to be stored in a memory in a non-contiguous manner.
Storing process in a non-contiguous manner solves the problem of external fragmentation.
For implementing paging the physical and logical memory spaces are divided into the same fixed-sized blocks.
These fixed-sized blocks of physical memory are called frames, and the fixed-sized blocks of logical memory are called pages.
List of Process Attributes:
1) Process ID
2) Process Counter: after resuming the process, from which line should we continue
3) Process State:
4)
5)
6)
7)
8)

Refs: [1](https://www.youtube.com/watch?v=qlH4-oHnBb8), 
[2](https://www.youtube.com/watch?v=dz9Tk6KCMlQ), 
[3](https://www.youtube.com/watch?v=xD5PB_g1rIE),
[4](https://en.wikipedia.org/wiki/Paging#Ferranti_Atlas),
[4](https://en.wikipedia.org/wiki/Virtual_address_space),
[5](https://en.wikipedia.org/wiki/Page_(computer_memory)) , 
[6](https://en.wikipedia.org/wiki/Page_replacement_algorithm)

