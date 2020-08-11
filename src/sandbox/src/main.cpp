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
void worker1()
{
    for(int i=0;i<1000;i++)
    {
        std::lock_guard<std::mutex> locker(mu);

        std::cout<<"worker1 is working, i="<<i <<std::endl;
    }
}

void worker2()
{


    for(int i=1000;i>0;i--)
    {
        std::lock_guard<std::mutex> locker(mu);
        std::cout<<"worker2 is working, i="<<i <<std::endl;
    }
}

class myclass
{
    public:
    void func(){    std::cout<<"func is working" <<std::endl;
}
    void  operator  ()()
    {
        std::cout<<"operator ()" <<std::endl;
    }

};

int myfunc(int &x)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    return x++;
}

int main()
{
    std::cout<<"main thread is working on data" <<std::endl;
    myclass myobj;
    std::thread t1(worker1);
    std::thread t2(worker2);
    t1.join();
    t2.join();

    std::conditional

    //conditional variable
    //call once notify all
}




