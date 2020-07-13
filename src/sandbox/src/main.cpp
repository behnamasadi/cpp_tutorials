#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include <functional>
#include <condition_variable>
#include <future>

/*
//std::defer_lock, std::try_lock, std::adopt_lock
Timed mutex
try_lock_for and try_lock_until
mutex.try_lock()
Recursive mutex
Shared mutex
The use of this mutex's share functionality should be fairly self-explanatory. A theoretically
infinite number of readers can gain read access to the mutex, while ensuring that only a
single thread can write to the resource at any time

lock_shared()
try_lock_shared()
unlock_shared()
cond.wait_for
Condition_variable_any

*/

std::mutex mu;
std::conditional cond;
int data=0;

void worker()
{
    std::cout<<"worker is working" <<std::endl;
    std::unique_lock<std::mutex> lock(mu);
}

int main()
{
    std::cout<<"main thread is working on data" <<std::endl;

    std::thread t1(worker);
}




