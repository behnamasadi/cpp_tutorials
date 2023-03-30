- [Thread](#thread)
- [Creation and Termination](#creation-and-termination)
  * [Function pointer](#function-pointer)
  * [Thread Callable Objects](#thread-callable-objects)
  * [Function object](#function-object)
  * [Lambda expression](#lambda-expression)
- [Transferring ownership of a thread](#transferring-ownership-of-a-thread)
- [Threads order of execution](#threads-order-of-execution)
  * [Waiting for a thread to complete (Joining Threads)](#waiting-for-a-thread-to-complete--joining-threads-)
  * [Detaching Threads](#detaching-threads)
- [Thread Oversubscription](#thread-oversubscription)
- [Differentiating Between Threads](#differentiating-between-threads)
- [Sleeping Threads](#sleeping-threads)
- [Yield](#yield)
- [Thread Synchronization](#thread-synchronization)
- [Racing Condition](#racing-condition)
  * [Racing Condition Example: Job ID](#racing-condition-example--job-id)
  * [Racing Condition Example: Wallet Example](#racing-condition-example--wallet-example)
  * [Racing Condition Example: Common Source Example](#racing-condition-example--common-source-example)
- [Semaphor](#semaphor)
- [Mutex](#mutex)
- [Thread Safe Functions](#thread-safe-functions)
- [Dead Lock](#dead-lock)
- [Lock Guard](#lock-guard)
  * [Avoiding Deadlock tips](#avoiding-deadlock-tips)
- [Unique Lock](#unique-lock)
  * [Lazy initialization and once_flag](#lazy-initialization-and-once-flag)
- [Scoped Lock](#scoped-lock)
- [Condition Variable](#condition-variable)
- [Async, Future and Promise](#async--future-and-promise)
- [Packaged Task](#packaged-task)



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

Full example [here](creating_and_terminating_threads.cpp). 

# Transferring ownership of a thread

# Threads order of execution
It is totally up to the operating system in which order threads are scheduled. You can assume it is totally random.

## Waiting for a thread to complete (Joining Threads)
Join will pause the current thread until the called threads are done, imagine in your main you have 10 threads to load the GUI,...you need to wait until they all done then you can continue. if you don't put join thread, you main function might return before even your threads finish their jobs


```child_thread.join()``` will wait for the child_thread to finish it task otherwise if the main thread finish we might have some unfinished tasks in child_thread.


## Detaching Threads
If we instead of join() we call detach() the child thread will become <b>demean thread</b> and it will run freely on its on. detach() leaves the thread to run in the background, with no direct means of communicating with it. 


Once you detattach a thread you can't call join() again so before joining check if it is joinable()
Full example [here](join_detach_threads.cpp).



# Thread Oversubscription
If you create more thread than your hardware support it will reduce your performance since switching between thread is expensive.

```
unsigned int con_threads; 
// calculating number of concurrent threads  supported in the hardware implementation 
con_threads = std::thread::hardware_concurrency(); 
std::cout << "Number of concurrent threads supported are: " << con_threads << std::endl;

```

# Differentiating Between Threads

You can use    ```std::this_thread::get_id() ``` to get the thread ID.
Full example [here](differentiating_between_threads.cpp).

# Sleeping Threads

```
std::this_thread::sleep_for(std::chrono::microseconds(numberOfMicroseconds));
usleep(numberOfMicroseconds);

```

Full example [here](sleeping_threads.cpp). 

# Yield

You can indicate to the OS that the current thread can be rescheduled so that other threads can run instead. 
For this, one uses the `std::this_thread::yield()` function. The precise result of this function depends on the 
OS and its scheduler. Within the case of a FIFO scheduler, it's likely that the calling thread will be put at the back of the line.


# Thread Synchronization
Thread synchronization is defined as a mechanism which ensures that two or more concurrent processes or threads do not simultaneously execute some particular program segment known as a critical section. Processes’ access to critical section is controlled by using synchronization techniques. When one thread starts executing the critical section  the other thread should wait until the first thread finishes. If proper synchronization techniques are not applied, it may cause a race condition where the values of variables may be unpredictable and vary depending on the timings of context switches of the processes or threads.

# Racing Condition

When two or more threads perform a set of operations in parallel, that access the same memory location.
Also, one or more thread out of them modifies the data in that memory location, then this can lead to an
unexpected results some times.

## Racing Condition Example: Job ID
In the following example we expect to get the same job id when entering the worker function and wehn we exit:

```
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
## Racing Condition Example: Wallet Example
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
                                             
## Racing Condition Example: Common Source Example
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

Full example [here](race_condition.cpp).
Soluton would be using `mutex` and `emaphor`.

# Semaphor
Semaphor use signaling while Mutex is an object.

Full example [here](semaphor.cpp).


# Mutex
A Mutex is a lock that we set before using a shared resource and release after using it.
When the lock is set, no other thread can access the locked region of code.

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
This will solve the problem but we might create other problems. The first problem is if for any reason mutex doesn't get unclocked, our program will get stock forever. 

Here when gaurd goes out of scope, mutex get unlocked automatically:
```
void sharedPrinter(std::string s,int id)
{
    
    std::lock_guard<std::mutex> gaurd(mu);
    std::cout<<"Message from function"<<id<<": " <<s<<std::endl;
}
```
But the other problem is that `std::cout` might be still manipulated outside of code and it is not still under protection of mutex. 
Full example [here](../src/multithreading/mutex.cpp).


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
| int a= myStack.top();   a=6                |                                           |
|                                            |int b= myStack.top();  b=6                 |
| myStack.pop();  6 is poped                 |                                           |
|                                            |myStack.pop();  4 is poped!                |
                                            
   
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
# Dead Lock
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
Full example [here](dead_lock.cpp).
The second solution is using `std::lock` and extra parameters `std::adopt_lock` for `lock_guard`:

# Lock Guard
A lock guard is a wrapper for mutex, which handles the obtaining of a lock on the
mutex object as well as its release when the lock guard goes out of scope. This is a helpful
mechanism to ensure that one does not forget to release a mutex lock, and to help reduce
clutter in one's code when one has to release the same mutex in multiple locations.


```
int counter = 0;
std::mutex counter_mutex;
void worker()
{
    std::lock_guard<std::mutex> lock(counter_mutex);
    if (counter == 1) 
    { 
        counter += 10; 
    }
    else if (counter >= 10) 
    { 
        counter += 15; 
    }
    else if (counter >= 50) 
    { 
        return; 
    }
    else 
    { 
        ++counter; 
    }
    std::cout << std::this_thread::get_id() << ": " << counter << '\n';
}
int main()
{
    std::cout << __func__ << ": " << counter << '\n';
    std::thread t1(worker);
    std::thread t2(worker);
    t1.join();
    t2.join();
    std::cout << __func__ << ": " << counter << '\n';
}
```

We can use `lock_guard` to solve our dead lock prolem:

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

## Avoiding Deadlock tips
 - Prefer locking single mutex.
 - Avoid locking a mutex and then calling a user provided function.
 - user `std::lock()` to lock more than one mutex
 - Lock the mutex in same order.


If you lock small part of your code many times, you program will become very complicated, if you lock piece of code, you might lose 
the advantageous of concurrent programming since your program need requeir lots of resources for switching between threads.
Full example [here](lock_guard.cpp).
# Unique Lock
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

`unique_lock` use the `RAII` pattern. When you want to lock a mutex, you create a local variable of type `unique_lock` passing the mutex as parameter. When the `unique_lock`is constructed it will lock the mutex, and it gets destructed it will unlock the mutex. More importantly: If a exceptions is thrown, the unique_lock destructer will be called and so the mutex will be <b>unlocked</b>.

`unique_lock` and `scoped_lock` work kind of like `unique_pointer` and `scoped_pointer`



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

The difference between `lock_guard` and `unique_lock`: you can lock and unlock a `unique_lock`. `std::lock_guard` will be locked only once on construction and unlocked on destruction.
The `unique_lock` is computationally more expensive than the `lock_guard` so if the performance issue should be considered when using it.

Since, C++17, one should use `scoped_lock` instead of `lock_guard`.

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

Whether a unique lock instance has ownership of its mutex, and whether it's locked or not,
is first determined when creating the lock, as can be seen with its constructors. For example:
```
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

Full example [here](unique_lock.cpp). 

# Scoped Lock
It differs from a `lock_guard` in that it is a wrapper for not one, but multiple mutexes.

This can be useful when one deals with multiple mutexes in a single scope. One reason to
use a scoped lock is to avoid accidentally introducing deadlocks and other unpleasant
complications with, for example, one mutex being locked by the scoped lock, another lock
still being waited upon, and another thread instance having the exactly opposite situation.
One property of a scoped lock is that it tries to avoid such a situation, theoretically making
this type of lock deadlock-safe.
Full example [here](scoped_lock.cpp). 

# Condition Variable
Condition variables are used for two purposes:
1. Notify other threads
2. Waiting for some condition

In the following exmaple:

```
std::condition_variable cond;
std::mutex mu;

int balance = 0;

void withdrawMoney(int amount) 
{
	std::unique_lock<std::mutex> lock(mu);
	cond.wait(lock, [] {if (balance == 0) return false; else return true; });
	balance = balance - amount;
	std::cout<<"Amount withdrawn, new balance is: "<< balance <<std::endl;
}

void addMoney(int amount)
{
	std::unique_lock<std::mutex> lock(mu);
	balance = balance + amount;
	std::cout << "Amount added, new balance is: " << balance << std::endl;
	cond.notify_one();
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
Then `worker_func()` can start working on the data. When the worker finsihed it job, then the main can 
do its job with the processed data. The function `wait()` from `std::condition_variable` has the following signature:  

`wait(unique_lock<mutex>& lock, Predicate pred)`:
Atomically unlocks lock, causes the current thread to block, adds it to the list of threads waiting on `*this`
until `notify_all()` or `notify_one()` is executed

`Predicate()`:
A callable object or function that takes no arguments and returns a value that can be evaluated as a bool.
This is called repeatedly until it evaluates to true.


```cpp
std::condition_variable cond;
std::mutex mu;

bool main_thread_is_ready=false;
bool data_has_been_processed = false;

int worker_func()
{
    std::unique_lock<std::mutex> worker_lock(mu);
    cond.wait(worker_lock,[]{return main_thread_is_ready;});
    std::cout << "Worker thread is processing data"<<std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    data_has_been_processed=true;
    std::cout << "Worker thread has processed the data"<<std::endl;
    worker_lock.unlock();
    cond.notify_one();
}

int main ()
{
    std::thread worker_thread(worker_func);
    {
        std::cout<<"pre-preparing data in main thread" <<std::endl;
        std::lock_guard<std::mutex>main_thread_lock(mu);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout<<"main thread is ready" <<std::endl;
        main_thread_is_ready=true;
    }
    cond.notify_one();
    std::unique_lock<std::mutex> main_lock(mu);
    cond.wait(main_lock,[]{return data_has_been_processed;});
    std::cout << "Back in main()"  <<std::endl;
    worker_thread.join();
}
```

Full example [here](condition_variable.cpp).
# Async, Future and Promise
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
We can ask `std::async` to create a thread for the function call or not. If we add `std::launch::deferred` to the list of parameters, it won't
create a thread and just create a function call.

```cpp
std::future<int> future_function= std::async(std::launch::deferred, factorial,input);

```
If you want to make sure a seperate thread will be created use `std::launch::async` in the parameters list:

```cpp
std::async(std::launch::async, factorial,input);
```

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
Full example [here](async_future_promise.cpp).


# Packaged Task
It can link a callable object (function, lambda expression, bind expression, or another function object) to a future so that it can be 
invoked asynchronously.
Full example [here](packaged_task.cpp).



Refs: [1](https://codereview.stackexchange.com/questions/221617/thread-pool-c-implementation),
[2](https://www.youtube.com/watch?v=eWTGtp3HXiw), [3](https://vorbrodt.blog/2019/02/12/simple-thread-pool/)
 


		
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
	[13](https://en.wikipedia.org/wiki/Page_replacement_algorithm),
	[14](https://www.geeksforgeeks.org/multilevel-feedback-queue-scheduling-mlfq-cpu-scheduling/),
	[15](https://www.geeksforgeeks.org/cpu-scheduling-in-operating-systems/),
	[16](https://stackoverflow.com/questions/11770571/how-do-mutexes-really-work)

