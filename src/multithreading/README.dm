# Process vs Thread
There are two model of concurrent programming, multi processing and multi threading.
processes communicate with each other via **inter process communication channel** such as files, PIPES, message queue, etc.

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






# Creation and Termination

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

# OverSubscrition 
If you create more thread than your hardware support it will reduce your performance since switching between thread is expensive.

```
unsigned int con_threads; 
// calculating number of concurrent threads  supported in the hardware implementation 
con_threads = std::thread::hardware_concurrency(); 
std::cout << "Number of concurrent threads supported are: " << con_threads << std::endl;

```



## Lambda expression
```
auto lambdaExpression = [](int n) {std::cout<<++n <<std::endl;};
std::thread t6(lambdaExpression,n);

```

Full Example [here](creating_and_terminating_threads.cpp) 

# Differentiating Between Threads

You can use    ```std::this_thread::get_id() ``` to get the thread ID.

# Sleeping Threads

```
std::this_thread::sleep_for(std::chrono::microseconds(numberOfMicroseconds));
usleep(numberOfMicroseconds);

```

Full example [here](sleeping_threads.cpp) 

# Joining Threads
Join will pause the current thread untill the called threads are done, imagine in your main you have 10 threads to load the GUI,...you need to wait until they all done then you can continue. if you don't put join thread, you main function might return before even your threads finish their jobs


```child_thread.join()``` will wait for the child_thread to finish it task otherwise if the main thread finish we might have some unfinished tasks in child_thread.


# Detaching Threads
If we instead of join() we call detach() the child thread will become demean process and it will run freely on its on.


Once you detattach a thread you can't call join() again so before joining check if it is joinable()



# Race Condition

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

First soluton would be using `mutex`.

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



# Thread Safe Functions
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

# DeadLock


# Printing Process Tree
# Racing Problem
# Mutex
# Guard

# Future, Promise and async
# Packaged Task















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
	[3](https://thispointer.com//c11-multithreading-part-4-data-sharing-and-race-conditions/)
	[4](https://www.youtube.com/watch?v=s5PCh_FaMfM)
	[5](https://www.youtube.com/watch?v=pWTtPnwialI)
	[6](https://www.modernescpp.com/index.php/thread-safe-initialization-of-data)





