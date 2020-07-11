# Process 
A process is just a running  program. The execution of a process must progress in a sequential fashion.

## Process Life Cycle

### Start
This is the initial state when a process is first started/created.

### Ready
The process is waiting to be assigned to a processor. Ready processes are waiting to have the processor 
allocated to them by the operating system so that they can run. Process may come into this state after
Start state or while running it by but interrupted by the scheduler to assign CPU to some other process.


### Running
Once the process has been assigned to a processor by the OS scheduler, the process state is set to running and the
processor executes its instructions.


### Waiting
Process moves into the waiting state if it needs to wait for a resource, such as waiting for user input,
or waiting for a file to become available.


### Terminated or Exit
Once the process finishes its execution, or it is terminated by the operating system, it is moved to the 
terminated state where it waits to be removed from main memory.


```
Start --→Ready →--------→ Running----→Terminated
            \            ↙
             \          /
              \        /
               ↖Wait ↙
```

### Process Control Block (PCB)
A Process Control Block is a data structure maintained by the Operating System for every process. 
The PCB is identified by an integer process ID (PID). A PCB keeps all the information needed to keep track of a process:


### Process State 
The current state of the process:
New (Create) – In this step, the process is about to be created but not yet created, it is the program which is present in secondary memory that will be picked up by OS to create the process.
Ready – New -> Ready to run. After the creation of a process, the process enters the ready state i.e. the process is loaded into the main memory. The process here is ready to run and is waiting to get the CPU time for its execution. Processes that are ready for execution by the CPU are maintained in a queue for ready processes.
Run – The process is chosen by CPU for execution and the instructions within the process are executed by any one of the available CPU cores.
Blocked or wait – Whenever the process requests access to I/O or needs input from the user or needs access to a critical region(the lock for which is already acquired) it enters the blocked or wait state. The process continues to wait in the main memory and does not require CPU. Once the I/O operation is completed the process goes to the ready state.
Terminated or completed – Process is killed as well as PCB is deleted.
Suspend ready – Process that was initially in the ready state but were swapped out of main memory(refer Virtual Memory topic) and placed onto external storage by scheduler are said to be in suspend ready state. The process will transition back to ready state whenever the process is again brought onto the main memory.
Suspend wait or suspend blocked – Similar to suspend ready but uses the process which was performing I/O operation and lack of main memory caused them to move to secondary memory.
When work is finished it may go to suspend ready.

### Process privileges (user mode vs kernel mode)
#### User mode 
When the computer system run user applications like creating a text document or using any application program, 
then the system is in the user mode. When the user application requests for a service from the operating system 
or an interrupt occurs or system call, then there will be a transition from user to kernel mode to fulfill the requests.

#### Kernel Mode
When the system boots, hardware starts in kernel mode and when operating system is loaded, it start user application in user mode.
To provide protection to the hardware, we have privileged instructions which execute only in kernel mode. 
If user attempt to run privileged instruction in user mode then it will treat instruction as illegal and traps to OS. 
Some of the privileged instructions are:

### Handling Interrupts
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



### CPU Scheduling in Operating Systems
The OS maintains all PCBs in Process Scheduling Queues. The OS maintains a separate queue for each of the process
states and PCBs of all processes in the same execution state are placed in the same queue.
When the state of a process is changed, its PCB is unlinked from its current queue and moved to its new state queue.

The Operating System maintains the following important process scheduling queues −

Job queue − This queue keeps all the processes in the system.

Ready queue − This queue keeps a set of all processes residing in main memory, ready and waiting to execute. A new process is always put in this queue.

Device queues − The processes which are blocked due to unavailability of an I/O device constitute this queue.



### Types of schedulers:

1) Long term 
2) Short term
3) Medium term

## CPU Scheduling in Operating Systems

Arrival Time: Time at which the process arrives in the ready queue.
Completion Time: Time at which process completes its execution.
Burst Time: Time required by a process for CPU execution.
Turn Around Time: Time Difference between completion time and arrival time.
Turn Around Time = Completion Time – Arrival Time
Waiting Time(W.T): Time Difference between turn around time and burst time.
Waiting Time = Turn Around Time – Burst Time



## Different Scheduling Algorithms


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
1)Process ID
2)Process Counter: after resuming the process, from which line should we continue
3)Process State:
4)
5)
6)
7)
8)


## Signals
Signals in computers are a way of communication between the process and the OS.
When a running program undergoes some serious error then the OS sends a signal to
the process and the process further may not execute.
Some processes may have a signal handler which does some important tasks before the
process leaves the CPU.

Signal and interrupt are basically same but a small distinction exists i.e interrupts
are generated by the processor and handled by the kernel but signals are generated by
the kernel and handled by the process.
Error signals generally causes termination of the program and a core dump file is
created named core, which stores the state of the process at the moment of termination.
This file can be investigated using debugger to know the cause of program termination.


### SIGILL:
This signal denotes illegal instruction. When a garbage instruction or instruction
which a program has no privilege to execute, is executed then this signal is
generated.
C does not produce illegal instruction so there is no chance of facing such error
signal, as the probable cause may be that the object file may be corrupted.
This signal is also generated when stack overflow occurs.


### SIGSEGV:
The signal is generated when process tries to access memory location not
allocated to it, like de-referencing a wild pointer which leads to
“segmentation fault”.

### SIGABRT:
If an error itself is detected by the program then this signal is
generated using call to abort(). This signal is also used by
standard library to report an internal error.
assert() function in c++ also uses abort() to generate this signal.

### SIGFPE:
Error that occurred like division by zero, floating point error.

### SIGBUS:
This signal is also produced when an invalid memory is accessed. It may
seem to be same like SIGSEGV but in SIGSEGV, the memory location referenced
is valid but in case of SIGBUS, memory referenced does not exist
i.e de-referencing a memory location out of memory space.


### SIGSYS:
This signal is sent to process when an invalid argument is passed to
a system call.

### SIGTRAP:
This signal is send to process when an exception is occurred.
This is requested by the debugger to get informed. For example,
if a variable changes its value then this will trigger it.

### SIGTERM:
The SIGTERM signal is a generic signal used to cause program termination.
Unlike SIGKILL, this signal can be blocked, handled, and ignored. It is
the normal way to politely ask a program to terminate. The shell command
kill generates SIGTERM by default.


### raise() function
csignal header file declared the function raise() to handle a particular signal. Signal learns some unusual behavior in a program, and calls the signal handler. It is implemented to check if the default handler will get called or it will be ignored.

```
int raise ( int signal_ )
```

## Process vs Thread
There are two model of concurrent programming, multi processing and multi threading.
processes communicate with each other via **Inter Process Communication (IPC)**.

### Inter Process Communication (IPC)
Processes can communicate with each other through both:
1) Shared Memory
2) Message passing


https://www.geeksforgeeks.org/inter-process-communication-ipc/

Socket, Message queue, Anonymous pipe, Message passing


Threads communicate via shared memory.
Multi threading program can not be run over distributed system (unless your threads communicate like multi process program), however multi pressing program can.










# Thread Basics
It is most effective on multi-processor or multi-core systems where the process flow can be scheduled to run on another processor 
thus gaining speed through parallel or distributed processing. Thread can be initiated faster because there is less for the 
Operating System to set up.
Threads require less overhead than "forking" or spawning a new process because the system does not initialize a new system virtual memory
 space and environment for the process. While most effective on a multiprocessor system, gains are also found on uni processor systems which exploit latency in I/O and other system functions which may halt process execution. (One thread may execute while another is waiting for I/O or some other system latency.)  

If you two thread running independently it is not deterministic which thread will run faster unless you put some synchronization mechanism.


Thread operations include **thread creation**, **termination**, **synchronization (joins,blocking)**, **scheduling**, **data management** and **process interaction**.
A thread does not maintain a list of created threads, nor does it know the thread that created it.
All threads within a process share the same address space.

Threads in the same process share:
 - Process instructions
 - Most data
 - open files (descriptors)
 - signals and signal handlers
 - current working directory
 - User and group id

Each thread has a unique:
 - Thread ID
 - set of registers, stack pointer
 - stack for local variables, return addresses
 - signal mask
 - priority
 - Return value: errno






# <a name="creation_and_termination"></a>Creation and Termination

std::thread is the thread class that represents a single thread in C++. To start a thread we simply need to create a new thread object and pass the executing code to be called (i.e, a callable object) into the constructor of the object. Once the object is created a new thread is launched which will execute the code specified in callable.

A callable can be either of the three
 - A function pointer
 - A function object
 - A lambda expression




## Function pointer
```
void functionPointer(int n)
{
    std::cout<<++n <<std::endl;
}

std::thread t1(functionPointer,n );
std::thread t2{functionPointer,n };
```


## Thread Callable Objects

```
class myClass
{
public:
    void f(int x, char c){}
    long g(double x){return 0;}
    int operator()(int n){return 0;}
};
```

This make a copy of `myObject`

```
myClass myObject;
std::thread t1(myObject,6);
```

This will launch `myObject()` in a different thread:
```
std::thread t2(std::ref(myObject),myObject,6);
```

This will launch a temporarily instance of `myClass` and it will be moved to thread object.
```
std::thread t3(myClass(),6);

```

This will create a copy of `myObject` and invoke `f()`
```
std::thread t5(&myClass::f,myObject,7,8,'w');
```
We can also avoid copying:

```
std::thread t6(&myClass::f, &myObject,7,8,'w');
```


We can move objects from parent thread to child thread but then `myObject` is no longer useable in the parent thread:
```
std::thread t8(&myClass::f, std::move(myObject),8,'w');
```

## Function object
```
class callableObject1
{
public:
    void operator ()(int n)
    {
        std::cout<<++n <<std::endl;
    }
};

class callableObject2
{
public:
    void operator()()
    {

    }
};


// function object
std::thread t3(callableObject1(),n);


// This will not compile and cause most vexing parse error
// std::thread t4(callableObject2());

// first solution
std::thread t4((callableObject2()));

// second solution
callableObject2 threadObj ;
std::thread t5(threadObj);

```

Parameters will be always passed by value even if you define your function by ref, so to pass them by ref you should use `std::ref()`

```
void functionPointerByRef(std::string &msg)
{
    msg="New value!";
}

std::string msg="Old Value";
std::thread  t7(functionPointerByRef, std::ref(msg));
t7.join();
std::cout<< msg<<std::endl;

```


Thread can not be copied and can only be moved:

```
t2=std::moved(t1);
```

## Lambda expression
```
auto lambdaExpression = [](int n) {std::cout<<++n <<std::endl;};
std::thread t6(lambdaExpression,n);

```

Full Example [here](creating_and_terminating_threads.cpp) 


# Over Subscrition 
If you create more thread than your hardware support it will reduce your performance since switching between thread is expensive.

```
unsigned int con_threads; 
// calculating number of concurrent threads  supported in the hardware implementation 
con_threads = std::thread::hardware_concurrency(); 
std::cout << "Number of concurrent threads supported are: " << con_threads << std::endl;

```

# <a name="differentiating_between_threads"/> Differentiating Between Threads

You can use    ```std::this_thread::get_id() ``` to get the thread ID.

# <a name="sleeping_threads"/> Sleeping Threads

```
std::this_thread::sleep_for(std::chrono::microseconds(numberOfMicroseconds));
usleep(numberOfMicroseconds);

```

Full example [here](sleeping_threads.cpp) 

# <a name="join_detach_threads"/> Joining Threads
Join will pause the current thread untill the called threads are done, imagine in your main you have 10 threads to load the GUI,...you need to wait until they all done then you can continue. if you don't put join thread, you main function might return before even your threads finish their jobs


```child_thread.join()``` will wait for the child_thread to finish it task otherwise if the main thread finish we might have some unfinished tasks in child_thread.


# Detaching Threads
If we instead of join() we call detach() the child thread will become demean process and it will run freely on its on.


Once you detattach a thread you can't call join() again so before joining check if it is joinable()

# <a name="yield"/> Yield

You can indicate to the OS that the current thread can be rescheduled so that other threads can run instead. 
For this, one uses the `std::this_thread::yield()` function. The precise result of this function depends on the 
OS and its scheduler. Within the case of a FIFO scheduler, it's likely that the calling thread will be put at the back of the line.




# <a name="racing_problem"/> Racing Problem

When two or more threads perform a set of operations in parallel, that access the same memory location.
Also, one or more thread out of them modifies the data in that memory location, then this can lead to an
unexpected results some times.

### Wallet Example
The following problem happens in the case for example:


```
class wallet 
{
public:
    int money;
    wallet():money(0){}
    void increaseMoney(int amount)
    {
       money=money+amount; 
    }
};

```


| Thread 1                                   |      Thread 2                             |
|----------                                  |:-------------:                            |
| load walletObject.money into memory        |                                           |
|                                            |load walletObject.money into memory        |
| increaseMoney() is being called            |                                           |
|                                            |increaseMoney() is being called            |
| update walletObject.money from CPU register|                                           | 
|                                            |update walletObject.money from CPU register|
               
                                            
                                             

                                             



                                             

let say "walletObject.money" was 10. 10 will be loaded. Both thread increase the value
to 11, while it should be increased to 11 and then 12. so the correct value is 12 while we write back into memory 11, which wrong.
                                             
### Common Source Example
In this example both threads race for accessing shared resource `std::cout`

```
void function2()
{
    unsigned int sleepTime=1;//milliseconds
    for(std::size_t i=0;i<100;i++)
    {
        std::cout<<"Message from function1: " <<i<<std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime) );
    }
}

void function1()
{
    unsigned int sleepTime=1;//milliseconds
    for(int i=0;i>-100;i--)
    {
        std::cout<<"Message from function1: " <<i<<std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime) );
    }
}

int main()
{
    std::thread t1(function1);
    std::thread t2(function2);
    t1.join();
    t2.join();
}
```
Therefore the output would be messy. 


```
Message from function1: 91
Message from function1: Message from function1: -9292

Message from function1: Message from function1: 93
-93
Message from function1: Message from function1: -94
94
Message from function1: -95
Message from function1: 95
```

First soluton would be using <a name="mutex"/> `mutex`.

```
std::mutex mu;
void sharedPrinter(std::string s,int id)
{
    mu.lock();
    std::cout<<"Message from function"<<id<<": " <<s<<std::endl;
    mu.unlock();
}

void function2()
{
    unsigned int sleepTime=1;//milliseconds
    for(std::size_t i=0;i<100;i++)
    {
        sharedPrinter( std::to_string(i) ,2);
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime) );
    }
}

void function1()
{
    unsigned int sleepTime=1;//milliseconds
    for(int i=0;i>-100;i--)
    {
        sharedPrinter( std::to_string(i) ,1);
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime) );
    }
}
```
This will solve the problem but we might create other problems. The first problem is if for any reason mutrx doesn't get unclocked, our program will get stock forever. 

Here when gaurd goes out of scope, mutex get unlocked automatically:
```
void sharedPrinter(std::string s,int id)
{
    
    std::lock_guard<std::mutex> gaurd(mu);
    std::cout<<"Message from function"<<id<<": " <<s<<std::endl;
}
```
But the other problem is that `std::cout` might be still manipulated outside of code and it is not still under protection of mutex. 



# <a name="thread_safe"/>Thread Safe Functions
Let say we have the following stack data structure:
```
template<typename T>
class stack
{
private:
    std::mutex mx;
    std::vector<T> data;
public:
    T top()
    {
        return data.back();
    }

    void pop()
    {
        data.pop_back();
    }

    void push(T element)
    {
        data.push_back(element);
    }

};

int main()
{
    stack<int> myStack;
    myStack.push(2);
    myStack.push(4);
    myStack.push(6);
}

```

and we have the following call from a function:



| Thread 1                                   |      Thread 2                             |
|----------                                  |:-------------:                            |
| int a= myStack.top(); //a=                 |                                           |
|                                            |int b= myStack.top();//b=6                 |
| myStack.pop();//6 is poped                 |                                           |
|                                            |myStack.pop();//4 is poped!                |
                                            
   
so we have poped the wrong value in the second thread. The solution is to merge `pop()` and `top()` fuctionality in one function:

```
class stack
{
private:
    std::mutex mx;
    std::vector<T> data;
public:
    T pop()
    {
        T tmp=data.back();
        data.pop_back();
        return tmp;
    }

    void push(T element)
    {
        data.push_back(element);
    }

};
```

# <a name="dead_lock"/>Dead Lock
In the following example two function are depending on each other lock:

```
std::mutex mu1, mu2;

void func1DeadLock()
{
    mu2.lock();
    std::cout<<"func1" <<std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    mu1.lock();
    mu2.unlock();
    mu1.unlock();
}

void func2DeadLock()
{
    mu1.lock();
    std::cout<<"func2" <<std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    mu2.lock();
    mu1.unlock();
    mu2.unlock();
}


int main()
{
    std::thread thread1(func1DeadLock);
    std::thread thread2(func2DeadLock);
    thread1.join();
    thread2.join();
}
```

First solution would be to lock mutex on correct order:

```
void func1DeadLockSolved()
{
    mu1.lock();
    std::cout<<"func1" <<std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    mu2.lock();
    mu2.unlock();
    mu1.unlock();
}

void func2DeadLockSolved()
{
    mu1.lock();
    std::cout<<"func2" <<std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    mu2.lock();
    mu1.unlock();
    mu2.unlock();
}

```

The second solution is using `std::lock` and extra parameters `std::adopt_lock` for `lock_guard`:
```
void func1DeadLockGuardGuaredSolved()
{
    std::lock(mu1,mu2);
    std::lock_guard<std::mutex> locker1(mu1,std::adopt_lock);
    std::lock_guard<std::mutex> locker2(mu2,std::adopt_lock);
    std::cout<<"func1" <<std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void func2DeadLockGuardSolved()
{
    std::lock_guard<std::mutex> locker1(mu1,std::adopt_lock);
    std::lock_guard<std::mutex> locker2(mu2,std::adopt_lock);
    std::cout<<"func2" <<std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
```

When mutex goes out of scope it will unclock the mutex. 

## Avoid Deadlock tips
 - Prefer locking single mutex.
 - Avoid locking a mutex and then calling a user provided function.
 - user `std::lock()` to lock more than one mutex
 - Lock the mutex in same order.


If you lock small part of your code many times, you program will become very complicated, if you lock piece of code, you might lose 
the advantageous of concurrent programming since your program need requeir lots of resources for switching between threads.

# <a name="unique_lock"/>Unique Lock
Follow up our logger example, we reviewed two way for locking the mutex, calling `mutex.lock()` and using `lock_guard`.
```
class LogFile
{
    std::mutex mu;
    std::ofstream file;
public:
    LogFile()
    {
        file.open("log.txt");
    }

    void sharedPrint(std::string msg, int id)
    {
        std::lock_guard<std::mutex> locker(mu);
        file<<"From "<<id <<": "<<msg<<std::endl;
    }
};
```
The third way is using `unique_lock`. If we have some operations after opening the file that doesn't need any lock we can 
explicitly only lock the part that we need and unclock it when we don't need lock.

```
class LogFile
{
std::mutex mu;
std::ofstream file;
public:
LogFile()
{
    file.open("log.txt");
}

void sharedPrint(std::string msg, int id)
{
    std::unique_lock<std::mutex> locker(mu,std::defer_lock);

    //some operation here
    locker.lock();
    file<<"From "<<id <<": "<<msg<<std::endl;
    locker.unlock();
    //some operation here
    locker.lock();
    //...
    locker.unlock();

}
};
```

The `unique_lock` is computationally more expensive than the `lock_guard` so if the performance issue should be considered when using it.

## Lazy initialization and once_flag
In our log class example, we opened the log file in the constructor but we might need to open the log file when we need it, for instance in 
`sharedPrint` function.

```
void sharedPrint(std::string msg, int id)
{
    if(!file.is_open())
    {
        file.open("log.txt");
    }
}
```
This is **Lazy Initialization** or **Initialization Upon First Use**. But this is not thread safe since `file.is_open()` is not synchronized.
By making the following changes it will became thread safe:
```
void sharedPrint(std::string msg, int id)
        {
            {
                std::unique_lock<std::mutex> locker2(mu_open_file);
                if(!file.is_open())
                {
                    file.open("log.txt");
                }
            }
            std::unique_lock<std::mutex> locker(mu,std::defer_lock);

            //some operation here
            locker.lock();
            file<<"From "<<id <<": "<<msg<<std::endl;
            locker.unlock();
            //some operation here
            locker.lock();
            //...
            locker.unlock();

        }
```
But we are repeating locking and locking file status checking everytime we want to print something, so the solution is using `once_flag`
```
std::call_once(flag,[&](){file.open("log.txt");});
```
File will be opened once only by one thread.


# <a name="condition_variable"/>Condition Variable
In the following example `function_1` produce data and `function_2` and we utilized mutex to synchronize accessing the data. The problem that rises is `function_2` is in a checking state and keep looping. If we change the `function_2` to the following we make it a bit better but we 
cant easily decide for how long we should suspend the thread in `function_2`

```
void function_2()
    {
        int data = 0;
        while ( data != 1)
        {
            std::unique_lock<std::mutex> locker(mu);
            if(!q.empty())
            {
                data=q.back();
                q.pop_back();
                locker.unlock();
                std::cout<<"t2 got a value from t1" <<data<<std::endl;
            }
            else
            {
                locker.unlock();
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    }
```

By using `condition_variable` we notify anyone thread who is waiting for the condition:

```
std::deque<int> q;
std::mutex mu;
std::condition_variable cond ;

void function_1()
{
int count = 10;
while (count > 0) {
    std::unique_lock<std::mutex> locker(mu);
    q.push_front(count);
    locker.unlock();
    cond.notify_one();  // Notify one waiting thread, if there is one.
    std::this_thread::sleep_for(std::chrono::seconds(1));
    count--;
}
}

void function_2()
{
int data = 0;
while ( data != 1) {
    std::unique_lock<std::mutex> locker(mu);
    cond.wait(locker, [](){ return !q.empty();} );  // Unlock mu and wait to be notified
        // relock mu
    data = q.back();
    q.pop_back();
    locker.unlock();
    std::cout << "t2 got a value from t1: " << data << std::endl;
}
}
```

# <a name="async_future_promise"/>Async, Future and Promise
Let say you need to pass some value from child thread to parent thread and you want to make sure the value is correctly computed:

```
void factorial(int input, int &output)
{
    int value=1;
    for(int i=1;i<=input;i++)
        value=i*value;

    output=value;
}

int main()
{
    int input=8;
    int output;

    std::thread t1(factorial,input,std::ref(output));
    t1.join();
    std::cout<< output<<std::endl;
}
```
you can use `condition_variable` and `mutex` but it would look complicated. There is an easier way, instead of creating a thread object, we can 
use some function call:

```
int factorial(int input)
{
    int value=1;
    for(int i=1;i<=input;i++)
        value=i*value;
    return value;
}

int main()
{
    int input=8;
    int output;
    std::future<int> future_thread= std::async(factorial,input);
    output=future_thread.get();
    std::cout<< output<<std::endl;
}
```
 
`future_thread.get()` function will wait for child thread to get finished and the return the value. If you call `future_thread.get()` again, your program will crash.
We can ask `std::async` to create a thread for the function call or not. If we add `std::launch::deferred` to the list of parameters, it won't
create a thread and just create a function call.

```
std::future<int> future_function= std::async(std::launch::deferred, factorial,input);

```
If you want to make sure a seperate thread will be created use `std::launch::async` in the parameters list:

```
std::async(std::launch::async, factorial,input);
```

We can also set a parameter from parent thread to child thread not in the creation time but in the future:

```
int factorialFuture(std::future<int>& f)
{
    int value=1;
    int n=f.get();
    for(int i=1;i<=n;i++)
        value=i*value;
    return value;
}


int main()
{
    int input=8;
    std::promise<int> p;
    std::future<int> inputFuture=p.get_future();
    std::future<int> future_promise_factorial= std::async(std::launch::async, factorialFuture,std::ref(inputFuture) );
    //some code here
    p.set_value(input);
    std::cout<< future_promise_factorial.get()<<std::endl;
}

```
If we can't set value for the future we will get an exception. To avoid that we can set the exception:
```
p.set_exception(std::make_exception_ptr(std::runtime_error("Value couldn't be assigned")));
``` 

and you can only `std::move()` future and promise.


If you need to send a `future` into multiple thread, since you can not copy it and calling `future.get()` will raise an exception, you can use the following:


```
int factorialSharedFuture(std::shared_future<int> f)
{
    int value=1;
    int n=f.get();
    for(int i=1;i<=n;i++)
        value=i*value;
    return value;
}

int main()
{
    std::promise<int> p;
    std::future inputFuture=p.get_future();
    std::shared_future<int> inputFutureShared=inputFuture.share();
    std::future<int> future_promise_factorial1= std::async(std::launch::async, factorialSharedFuture,inputFutureShared );
    std::future<int> future_promise_factorial2= std::async(std::launch::async, factorialSharedFuture,inputFutureShared );
    std::future<int> future_promise_factorial3= std::async(std::launch::async, factorialSharedFuture,inputFutureShared );
}
```



# <a name="packaged_task"/> Packaged Task
It can link a callable object (function, lambda expression, bind expression, or another function object) to a future so that it can be 
invoked asynchronously.



# <a name="printing_process_tree"/>Printing Process Tree

`ps -aef --forest` 


`ps f -g<PID>`




















# Memory
When you run two programs on an Operating System that offers memory protection, as Windows and UNIX/Linux
do, the two programs are executed as separate processes, which means they are given separate address spaces.
This means that when:
	program #1 modifies the address 0x800A 1234 in its memory space,
	program #2 does not see any change in the contents of its memory at address 0x800A 1234.


## Thread vs Process
    
    
The separate address space is what is missing. When a program consists of two or more threads, all the threads
share a single memory space. If one thread modifies the contents of the address 0x800A 1234,
then all the other threads immediately see a change in the contents of their address 0x800A 1234.

## Heap
Furthermore, all the threads share a single heap. If one thread allocates (via malloc or new)
all of the memory available in the heap, then attempts at additional allocations by the other
threads will fail.

## Stack
But each thread is given its own stack.
		
		
Ref:    [1](http://www.yolinux.com/TUTORIALS/LinuxTutorialPosixThreads.html), 
	[2](http://www.computersciencelab.com/MultithreadingTut1.htm), 
	[3](https://thispointer.com//c11-multithreading-part-4-data-sharing-and-race-conditions/), 
	[4](https://www.youtube.com/watch?v=s5PCh_FaMfM), 
	[5](https://www.youtube.com/watch?v=pWTtPnwialI), 
	[6](https://www.modernescpp.com/index.php/thread-safe-initialization-of-data), 
	[7](https://www.youtube.com/watch?v=qlH4-oHnBb8), 
	[8](https://www.youtube.com/watch?v=dz9Tk6KCMlQ), 
	[9](https://www.youtube.com/watch?v=xD5PB_g1rIE), 
	[10](https://en.wikipedia.org/wiki/Paging#Ferranti_Atlas), 
	[11](https://en.wikipedia.org/wiki/Virtual_address_space), 
	[12](https://en.wikipedia.org/wiki/Page_(computer_memory)), 
	[13](https://en.wikipedia.org/wiki/Page_replacement_algorithm)










