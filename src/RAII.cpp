#include <iostream>
#include <thread>
#include <mutex>

/*
RAII (Resource Acquisition is Initialization) tries to solve the problems like these:
Problems:
1)Problem #1
    int array=new int[size];
array goes out of scope without being deleted, memory leak

2)Problem #2
std::mutex global_mu;
void funcCalledInSeveralTreads()
{
    global_mu.lock()
}
we never unlock the mutex, so this function will stuck in deadlock


3) problem # 3
std::thread t1(func1);
t1 goes out of scope and it is joinable but std::terminte is called;


RAII is when you acquire "resources" in constructor and release them in destructor;

Whats is a resource?
1) must be acquired before use
2) in limited supply
example:
heap: you should use malloc befprehand, heap is also limited
files: you should open files before use, the number of files on your mahine is limited
socket:
mutex:

Example of RAII from standard c++:
std::vector, std::string, std::unique_ptr, std::lock_gaurd

Refs: https://medium.com/swlh/what-is-raii-e016d00269f9
*/

std::mutex  globalMutex;
void lockMutexBad(bool shouldThrow) {
  std::cout << "Locking mutex manually..." << std::endl;
  globalMutex.lock();
  std::cout << "Mutex is locked!" << std::endl;

  // Could also imagine this as an early return. If you use a plain old
  // mutex, you have to remember to manually unlock before every return...
  // and it's quite easy to forget.
  if (shouldThrow) {
    std::cout << "Throwing exception" << std::endl;
    throw 1;
  }

  globalMutex.unlock();
  std::cout << "Mutex has been unlocked manually" << std::endl;
}

void lockMutexBadExample() {
    try
    {
        //tis call will llock the mutex without unlocking that
        lockMutexBad(true);
    } catch (...)
    {
        std::cout << "Caught exception" << std::endl;
    }
    //this call will stuck in the deadlcok
    lockMutexBad(false);
}

void lockMutexGood(bool shouldThrow) {
  std::cout << "Locking mutex with scoped_lock..." << std::endl;
  std::scoped_lock lock(globalMutex);
  std::cout << "Mutex is locked!" << std::endl;

  if (shouldThrow) {
    std::cout << "Throwing exception" << std::endl;
    throw 1;
  }
}

void lockMutexGoodExample() {
    try
    {
        lockMutexGood(true);
    } catch (...)
    {
        std::cout << "Caught exception" << std::endl;
    }
    lockMutexGood(false);
}


int main()
{
    std::thread t1(lockMutexBadExample);

    t1.join();
    return 0;
}
