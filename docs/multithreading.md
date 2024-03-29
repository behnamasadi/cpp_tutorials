- [Thread](#thread)
- [Creation and Termination](#creation-and-termination)
  - [Function pointer](#function-pointer)
  - [Thread Callable Objects](#thread-callable-objects)
  - [Function object](#function-object)
  - [Lambda expression](#lambda-expression)
  - [Transferring ownership of a thread](#transferring-ownership-of-a-thread)
  - [Waiting for a thread to complete (Joining Threads)](#waiting-for-a-thread-to-complete-joining-threads)
  - [Detaching Threads](#detaching-threads)
- [Order Execution of Threads](#order-execution-of-threads)
- [Thread Oversubscription](#thread-oversubscription)
- [Getting Thread ID](#getting-thread-id)
- [Putting Threads Into Sleeping](#putting-threads-into-sleeping)
- [Thread Yield](#thread-yield)
- [Thread Synchronization](#thread-synchronization)
  - [Synchronization Primitive](#synchronization-primitive)
  - [Racing Condition](#racing-condition)
    - [Racing Condition Example: Job ID](#racing-condition-example-job-id)
    - [Racing Condition Example: Wallet Example](#racing-condition-example-wallet-example)
    - [Racing Condition Example: Common Source Example](#racing-condition-example-common-source-example)
  - [Semaphor](#semaphor)
  - [Mutex](#mutex)
    - [Dead Lock](#dead-lock)
    - [Avoiding Deadlock tips](#avoiding-deadlock-tips)
    - [scoped\_lock](#scoped_lock)
    - [unique\_lock](#unique_lock)
    - [lock\_guard vs scoped\_lock vs unique\_lock](#lock_guard-vs-scoped_lock-vs-unique_lock)
  - [Condition Variable](#condition-variable)
  - [Semaphores vs Mutexes vs Condition Variable](#semaphores-vs-mutexes-vs-condition-variable)
- [std::future](#stdfuture)
  - [std::async](#stdasync)
    - [launch policy](#launch-policy)
  - [packaged\_task](#packaged_task)
  - [promise](#promise)
  - [When to use promise over async or packaged\_task](#when-to-use-promise-over-async-or-packaged_task)
- [std::atomic](#stdatomic)
- [Threadsafe Code](#threadsafe-code)
  - [How to make an application thread safe](#how-to-make-an-application-thread-safe)

# Thread
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

std::thread is the thread class that represents a single thread in C++. To start a thread we simply need to create a new thread object and pass the executing code to be called (i.e, a callable object) into the constructor of the object. 

<b>Once the object is created, a new thread is launched</b> which will execute the code specified in callable.

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

```cpp
class myClass
{
public:
    void f(int x, char c){}
    long g(double x){return 0;}
    int operator()(int n){return 0;}
};
```

This make a copy of `myObject`

```cpp
myClass myObject;
std::thread t1(myObject,6);
```

This will launch `myObject()` in a different thread:
```cpp
std::thread t2(std::ref(myObject),myObject,6);
```

This will launch a temporarily instance of `myClass` and it will be moved to thread object.
```cpp
std::thread t3(myClass(),6);

```

This will create a copy of `myObject` and invoke `f()`
```cpp
std::thread t5(&myClass::f,myObject,7,8,'w');
```
We can also avoid copying:

```cpp
std::thread t6(&myClass::f, &myObject,7,8,'w');
```


We can move objects from parent thread to child thread but then `myObject` is no longer useable in the parent thread:
```cpp
std::thread t8(&myClass::f, std::move(myObject),8,'w');
```

## Function object
```cpp
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

```cpp
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

```cpp
t2=std::moved(t1);
```

## Lambda expression
```cpp
auto lambdaExpression = [](int n) {std::cout<<++n <<std::endl;};
std::thread t6(lambdaExpression,n);

```

Full example [here](../src/multithreading/creating_and_terminating_threads.cpp). 

## Transferring ownership of a thread


## Waiting for a thread to complete (Joining Threads)
Join will pause the current thread until the called threads are done, imagine in your main you have 10 threads to load the GUI,...you need to wait until they all done then you can continue. if you don't put join thread, you main function might return before even your threads finish their jobs


`child_thread.join()` will wait for the child_thread to finish it task otherwise if the main thread finish we might have some unfinished tasks in child_thread.


## Detaching Threads
If we instead of join() we call detach() the child thread will become <b>demean thread</b> and it will run freely on its on. detach() leaves the thread to run in the background, with no direct means of communicating with it. 


Once you detached a thread you can't call join() again so before joining check if it is joinable()
Full example [here](../src/multithreading/join_detach_threads.cpp).

# Order Execution of Threads 
It is totally up to the operating system in which order threads are scheduled. You can assume it is totally random.

# Thread Oversubscription
If you create more thread than your hardware support it will reduce your performance since switching between thread is expensive.

```cpp
unsigned int con_threads; 
// calculating number of concurrent threads  supported in the hardware implementation 
con_threads = std::thread::hardware_concurrency(); 
std::cout << "Number of concurrent threads supported are: " << con_threads << std::endl;

```

# Getting Thread ID

You can use  `std::this_thread::get_id()` to get the thread ID.
```cpp

std::cout << "Thread ID is: " << std::this_thread::get_id() << '\n';
std::cout << "Function ID is: " << __func__ << '\n';
```
Full example [here](../src/multithreading/differentiating_between_threads.cpp).

# Putting Threads Into Sleeping 

```cpp
std::this_thread::sleep_for(std::chrono::microseconds(numberOfMicroseconds));
usleep(numberOfMicroseconds);

```

Full example [here](../src/multithreading/sleeping_threads.cpp). 

# Thread Yield

You can indicate to the OS that the current thread can be rescheduled so that other threads can run instead. 
For this, one uses the `std::this_thread::yield()` function. The precise result of this function depends on the 
OS and its scheduler. Within the case of a FIFO scheduler, it's likely that the calling thread will be put at the back of the line.


# Thread Synchronization
Thread synchronization is defined as a mechanism which ensures that two or more concurrent processes or threads do not simultaneously execute some particular program segment known as a **critical section**.  A critical section is a piece of code that must not be run by multiple threads at once because the code accesses shared resources. When one thread starts executing the critical section  the other thread should wait until the first thread finishes. If proper synchronization techniques are not applied, it may cause a race condition where the values of variables may be unpredictable.

## Synchronization Primitive

Some common synchronization primitives (in order of least to most computationally expensive):
1. Mutual exclusion (mutexes)
2. Semaphores: It's  a way to limit the number of consumers for a specific resource. For instance, limit the number of simultaneous calls to a database. 
3. Condition variables

Refs: [1](https://stackoverflow.com/questions/2332765/what-is-the-difference-between-lock-mutex-and-semaphore)

## Racing Condition

A race condition occurs when two or more threads can access shared data and they try to change it at the same time. Because the thread scheduling algorithm can swap between threads at any time, you don't know the order in which the threads will attempt to access the shared data. Therefore, the result of the change in data is dependent on the thread scheduling algorithm


### Racing Condition Example: Job ID
In the following example we expect to get the same job id when entering the worker function and wehn we exit:

```cpp
int counter=0;

void worker()
{
    counter++;
    std::cout<<"job number: "<<counter<<" started" <<std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout<<"job number: "<<counter<<" finished" <<std::endl;
}

int main()
{
    std::thread t1(worker);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::thread t2(worker);
    t1.join();
    t2.join();
}
```

Expectation:
```
job number: 1 started
job number: 1 finished
job number: 2 started
job number: 2 finished
```
But we will get:
```
job number: 1 started
job number: 2 started
job number: 2 finished
job number: 2 finished

```
### Racing Condition Example: Wallet Example
The following problem happens in the case for example:


```cpp
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


| Thread 1                                    |                  Thread 2                   |
| ------------------------------------------- | :-----------------------------------------: |
| load walletObject.money into memory         |                                             |
|                                             |     load walletObject.money into memory     |
| increaseMoney() is being called             |                                             |
|                                             |       increaseMoney() is being called       |
| update walletObject.money from CPU register |                                             |
|                                             | update walletObject.money from CPU register |

let say "walletObject.money" was 10. 10 will be loaded. Both thread increase the value
to 11, while it should be increased to 11 and then 12. so the correct value is 12 while we write back into memory 11, which wrong.
                                             
### Racing Condition Example: Common Source Example
In this example both threads race for accessing shared resource `std::cout`

```cpp
void function2()
{
    unsigned int sleepTime=1;//milliseconds
    for(std::size_t i=0;i<100;i++)
    {
        std::cout<<"Message from function2: " <<i<<std::endl;
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
Message from function1: -67
Message from function2: 68Message from function1: -68

Message from function1: -69Message from function2: 69

Message from function2: 70
```
Refs: [1]([3](https://thispointer.com//c11-multithreading-part-4-data-sharing-and-race-conditions/),)


Full example [here](../src/multithreading/race_condition.cpp).

## Semaphor
Semaphor use signaling while Mutex is an object.

Full example [here](../src/multithreading/semaphor.cpp).


## Mutex
A Mutex is a lock that we set before using a shared resource and release after using it.
When the lock is set, no other thread can access the locked region of code.

```cpp
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
This will solve the problem but we might create other problems. The first problem is if for any reason mutex doesn't get unclocked, our program will get stock forever. 

Here when guard goes out of scope, mutex get unlocked automatically:

```cpp
void sharedPrinter(std::string s,int id)
{
    
    std::lock_guard<std::mutex> guard(mu);
    std::cout<<"Message from function"<<id<<": " <<s<<std::endl;
}
```
But the other problem is that `std::cout` might be still manipulated outside of code and it is not still under protection of mutex. 
Full example [here](../src/multithreading/mutex.cpp).



### Dead Lock
In the following example two function are depending on each other lock:

```cpp
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

```cpp
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
Full example [here](../src/multithreading/dead_lock.cpp).
The second solution is using `std::lock` and extra parameters `std::adopt_lock` for `lock_guard`:


### Avoiding Deadlock tips
 - Prefer locking single mutex.
 - Avoid locking a mutex and then calling a user provided function.
 - user `std::lock()` to lock more than one mutex
 - Lock the mutex in same order.


If you lock small part of your code many times, you program will become very complicated, if you lock piece of code, you might lose 
the advantageous of concurrent programming since your program need requeir lots of resources for switching between threads.
Full example [here](../src/multithreading/lock_guard.cpp).

### scoped_lock

```cpp
{
   std::scoped_lock lock{mut};  // protect this block
   ...
}
```
A `scoped_lock` is a wrapper for mutex, which handles the obtaining of a lock on the
mutex object as well as its release when the lock goes out of scope. It differs from a
 `lock_guard` in that it is a wrapper for not one, but multiple mutexes.
This can be useful when one deals with multiple mutexes in a single scope. 

When mutex goes out of scope it will unlock the mutex. 

```cpp
std::mutex mu;

void sharedPrinter(int funcid, int message) {
  std::scoped_lock<std::mutex> gaurd(mu);
  std::cout << "Message from function: " << funcid
            << " and message is: " << message << std::endl;
}

void function2() {
  unsigned int sleepTime = 1; // milliseconds
  for (std::size_t i = 0; i < 100; i++) {
    sharedPrinter(2, i);
    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
  }
}

void function1() {
  unsigned int sleepTime = 1; // milliseconds
  for (int i = 0; i > -100; i--) {
    sharedPrinter(1, i);
    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
  }
}

int main() {
  std::thread t1(function1);
  std::thread t2(function2);
  t1.join();
  t2.join();
  return 0;
}
```
Full example [here](../src/multithreading/scoped_lock.cpp). 
### unique_lock
In the following logger, we reviewed two way for locking the mutex, calling `mutex.lock()` and using `lock_guard`. 
```cpp
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
explicitly only lock the part that we need and unlock it when we don't need lock.

`unique_lock` use the `RAII` pattern. When you want to lock a mutex, you create a local variable of type `unique_lock` passing the mutex as parameter. When the `unique_lock`is constructed it will lock the mutex, and it gets destructed it will unlock the mutex. More importantly: If a exceptions is thrown, the unique_lock destructer will be called and so the mutex will be **unlocked**.

```cpp
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



Lazy initialization and `once_flag`: 

In our log class example, we opened the log file in the constructor but we might need to open the log file when we need it, for instance in 
`sharedPrint` function.

```cpp
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
```cpp
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
But we are repeating locking and locking file status checking every time we want to print something, so the solution is using `once_flag`
```cpp
std::call_once(flag,[&](){file.open("log.txt");});
```
File will be opened once only by one thread.

Whether a unique lock instance has ownership of its mutex, and whether it's locked or not,
is first determined when creating the lock, as can be seen with its constructors. For example:
```cpp
std::mutex m0, m1, m2, m3;
std::unique_lock<std::mutex> lock1(m1);
std::unique_lock<std::mutex> lock1(m1, std::defer_lock);
std::unique_lock<std::mutex> lock2(m2, std::try_lock);
std::unique_lock<std::mutex> lock3(m3, std::adopt_lock);
```
The first locks the associated mutex by calling m0.lock()
The second one constructor in the last code does not lock the assigned mutex (defers). 
The third one attempts to lock the mutex using try_lock(). 
Finally, the last constructor assumes that it already owns the provided mutex.

Full example [here](../src/multithreading/unique_lock.cpp). 

### lock_guard vs scoped_lock vs unique_lock

 `std::scoped_lock` does the same as `std::lock_guard` and more ( it is a wrapper for not one, but multiple mutexes). Simply use `std::scoped_lock` everywhere. 
 use `unique_lock` if you need to unlock within the scope of the block.
 The difference between `lock_guard` and `unique_lock`: you can lock and unlock a `unique_lock`. `std::lock_guard` will be locked only once on construction and unlocked on destruction. The `unique_lock` is computationally more expensive than the `lock_guard` so if the performance issue should be considered when using it.


## Condition Variable
Without condition variables, the programmer would need to have threads continually polling (possibly in a critical section), to check if the condition is met. A condition variable is a way to achieve the same goal without polling.
Condition variables are used for two purposes:
1. Notify other threads
2. Waiting for some condition

In the following example:




```cpp
std::mutex mu;
std::condition_variable cond;

// shared variable
int result = 0;


// I) The thread that modify the shared variable must do the followings::

void worker() {
  // 1. Acquiring a mutex
  std::scoped_lock<std::mutex> lock(mu);
  // 2. Modify the shared variable while the lock is owned, some heavy processing operations, might take time...
  std::cout << "worker is working..." << std::endl;

  std::this_thread::sleep_for(std::chrono::seconds(5));

  result = 10;
  std::cout << "worker has reached the end, result is ready..." << std::endl;

  //3. Call notify_one (or notify_all ) on the std::condition_variable
  cond.notify_one();
}

// II) Any thread/ function that is waiting for shared variable must do the followings:
void waitingForWorker() {
  // 1. Acquire a std::unique_lock<std::mutex> on the mutex used to protect the shared variable
  std::unique_lock<std::mutex> lock(mu);


  //2. Call wait, wait_for, or wait_until
  cond.wait(lock, [] {
    std::cout << "result is: " << result << std::endl;
    //if you don't return true, the waitingForWorker thread will run forever
    return true;
  });

  // or just 
 //cond.wait(lock);

    // this will be that last line of the code 
    std::cout << "end of waitingForWorker" << std::endl;

}

int main() {
  std::thread t1(worker);
  std::cout << "before calling waitingForWorker" << std::endl;
  std::thread t2(waitingForWorker);
  std::cout << "after waitingForWorker has been called " << std::endl;
  t1.join();
  t2.join();
  return 0;
}
```


An other example, we only want to withdraw after we added some money, so `withdrawMoney` is waiting for `addMoney`:


```cpp
std::condition_variable cond;
std::mutex mu;

int balance = 0;


void addMoney(int amount)
{
	std::unique_lock<std::mutex> lock(mu);
	balance = balance + amount;
	std::cout << "Amount added, new balance is: " << balance << std::endl;
	cond.notify_one();
}

void withdrawMoney(int amount) 
{
	std::unique_lock<std::mutex> lock(mu);
	cond.wait(lock, [] {if (balance == 0) return false; else return true; });
	balance = balance - amount;
	std::cout<<"Amount withdrawn, new balance is: "<< balance <<std::endl;
}



int main()
{
	int amountToAdd, amountToWithdraw;
	amountToAdd=200;
	amountToWithdraw=100;
	std::thread t_add(addMoney, amountToAdd);
	std::thread t_withdrawMoney(withdrawMoney, amountToWithdraw);

	t_add.join();
	t_withdrawMoney.join();
}
```
We don't know the order that threads will be run, but logically we only want to withdraw money only if we have added
some money first. So in this example in the line:
`cond.wait(lock, [] {if (balance == 0) return false; else return true; });`
our conditional variable `cond` will lock the mutex, then check the condition, if it is not true, it will suspend the 
current thread, put it in the waiting list, release the mutex so the other thread `t_add` can continue.

In the thread `t_add` after we added the amount, we call `cond.notify_one();`. This will awake the thread in the 
waiting list, if the condition is true, it resume the `t_withdrawMoney` from the line after `cond.wait()`   

Refs: [1](https://www.youtube.com/watch?v=eh_9zUNmTig)

In the other example the `worker_func()` should waits until `main` has pre-processed the data.
Then `worker_func()` can start working on the data. When the worker finished it job, then the main can 
do its job with the processed data. The function `wait()` from `std::condition_variable` has the following signature:  

`wait(unique_lock<mutex>& lock, Predicate pred)`:
Atomically unlocks lock, causes the current thread to block, adds it to the list of threads waiting on `*this`
until `notify_all()` or `notify_one()` is executed

`Predicate()`:
A callable object or function that takes no arguments and returns a value that can be evaluated as a bool.
This is called repeatedly until it evaluates to true.



Full example [here](../src/multithreading/condition_variable.cpp).

## Semaphores vs Mutexes vs Condition Variable

Refs: [1](https://barrgroup.com/embedded-systems/how-to/rtos-mutex-semaphore), [2](https://stackoverflow.com/questions/2350544/in-what-situation-do-you-use-a-semaphore-over-a-mutex-in-c/2350628#2350628), [3](https://stackoverflow.com/questions/4742196/advantages-of-using-condition-variables-over-mutex), [4](https://cs61.seas.harvard.edu/site/2018/Synch3/)


# std::future
`future` facilities **asynchronous** access to values set by providers:
1. `std::async`
2. `std::packaged_task`
3. `std::promise` 


## std::async 
The function template  `std::async` asynchronously runs the function `f`  and returns a `std::future` that holds the result of that function call.

### launch policy
1. `std::launch::async`:  passed function will be executed in separate thread.
2. `std::launch::deferred`: function will be called when other thread will call get() on future to access the shared state.
3. `std::launch::async | std::launch::deferred`: default behavior, with this launch policy it can run asynchronously or not depending on the load on system (we have no control over it.)


```cpp
std::future<int> future_function= std::async(std::launch::deferred, factorial,input);

```
We can ask `std::async` to create a thread for the function call or not. If we add `std::launch::deferred` to the list of parameters, it won't
create a thread and just create a function call.


If you want to make sure a separate thread will be created use `std::launch::async` in the parameters list:

```cpp
std::async(std::launch::async, factorial,input);
```


```cpp
std::string whoisLookup(std::string domain) {

  std::cout << "thread ID of whoisLookup func is: "
            << std::this_thread::get_id() << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(3));

  return "foo";
}

int main() {
  std::string domain = "www.google.com";

  std::cout << "thread ID of main calling whoisLookup func is: "
            << std::this_thread::get_id() << std::endl;

  std::cout << "calling an async call" << std::endl;
  std::future<std::string> whoisresult =
      std::async(std::launch::async, whoisLookup, domain);

  std::cout << "continue after async call" << std::endl;
  std::cout
      << "Will block till data is available in future<std::string> object."
      << std::endl;

  std::cout << "waiting..." << std::endl;
  std::future_status status;
  do {

    switch (status = whoisresult.wait_for(std::chrono::seconds(1)); status) {
    case std::future_status::deferred:
      std::cout << "deferred" << std::endl;
      break;
    case std::future_status::timeout:
      std::cout << "timeout" << std::endl;
      break;
    case std::future_status::ready:
      std::cout << "ready" << std::endl;
      break;
      break;
    }

  } while (status != std::future_status::ready);

  std::cout << "the result of future object is: " << whoisresult.get()
            << std::endl;
  std::cout << "end" << std::endl;
}
```




## packaged_task
It can link a callable object (function, lambda expression, bind expression, or another function object) to a future so that it can be 
invoked asynchronously.
```cpp
int main() {
  std::string domain = "www.google.com";

  std::cout << "thread ID of main calling whoisLookup func is: "
            << std::this_thread::get_id() << std::endl;

  std::packaged_task<std::string(std::string)> task([](std::string domain) {
    std::cout << "thread ID of lambda package task is: "
              << std::this_thread::get_id() << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(3));
    return "foo";
  });

  std::cout << "getting future object from task" << std::endl;
  std::future<std::string> whoisresult = task.get_future();

  std::cout << "calling the task" << std::endl;
  // task(domain);

//The thread starts running immediately. 
  std::thread thr(std::move(task), domain);

  std::cout << "continue after calling task" << std::endl;
  std::cout
      << "Will block till data is available in future<std::string> object."
      << std::endl;

  std::cout << "waiting..." << std::endl;
  std::future_status status;
  do {

    switch (status = whoisresult.wait_for(std::chrono::seconds(1)); status) {
    case std::future_status::deferred:
      std::cout << "deferred" << std::endl;
      break;
    case std::future_status::timeout:
      std::cout << "timeout" << std::endl;
      break;
    case std::future_status::ready:
      std::cout << "ready" << std::endl;
      break;
      break;
    }

  } while (status != std::future_status::ready);
  thr.join();
  std::cout << "the result of future object is: " << whoisresult.get()
            << std::endl;
  std::cout << "end" << std::endl;
}
```




Full example [here](../src/multithreading/packaged_task.cpp).

Refs: [1](https://stackoverflow.com/questions/11004273/what-is-stdpromise#:~:text=You%20want%20to%20pass%20something,This%20is%20the%20promise%20.), [2](https://en.cppreference.com/w/cpp/thread/packaged_task), [3](https://codereview.stackexchange.com/questions/221617/thread-pool-c-implementation), [4](https://www.youtube.com/watch?v=eWTGtp3HXiw), [5](https://vorbrodt.blog/2019/02/12/simple-thread-pool/)
 

## promise

Let say you need to pass some value from child thread to parent thread and you want to make sure the value is correctly computed:

```cpp
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

```cpp
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


We can also set a parameter from parent thread to child thread not in the creation time but in the future:

```cpp
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

```cpp
p.set_exception(std::make_exception_ptr(std::runtime_error("Value couldn't be assigned")));
```


and you can only `std::move()` future and promise.


If you need to send a `future` into multiple thread, since you can not copy it and calling `future.get()` will raise an exception, you can use the following:


```cpp
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
Full example [here](../src/multithreading/(async_future_promise.cpp).

		
Ref: [1](https://stackoverflow.com/questions/11004273/what-is-stdpromise#:~:text=You%20want%20to%20pass%20something,This%20is%20the%20promise%20.)


## When to use promise over async or packaged_task
Refs: [1](https://stackoverflow.com/questions/17729924/when-to-use-promise-over-async-or-packaged-task)



# std::atomic



# Threadsafe Code
A class is thread-safe if it behaves correctly when accessed from multiple threads, regardless of the scheduling or interleaving of the execution of those threads by the runtime environment, and with no additional synchronization or other coordination on the part of the calling code

Refs: [1](https://www.youtube.com/watch?v=pWTtPnwialI), [2](https://www.youtube.com/watch?v=s5PCh_FaMfM), 

## How to make an application thread safe

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



| Thread 1                    |          Thread 2           |
| --------------------------- | :-------------------------: |
| int a= myStack.top();   a=6 |                             |
|                             | int b= myStack.top();  b=6  |
| myStack.pop();  6 is poped  |                             |
|                             | myStack.pop();  4 is poped! |
                                            
   
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
Full example [here](thread_safe.cpp).
Refs: [1](https://stackoverflow.com/questions/5125241/how-to-make-an-application-thread-safe)