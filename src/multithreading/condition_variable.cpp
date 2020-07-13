#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>



namespace polling_deque
{
    std::deque<int> q;
    std::mutex mu;

    void function_1()
    {
        int count = 10;
        while (count > 0) {
            std::unique_lock<std::mutex> locker(mu);//this will create and lock the mutex
            q.push_front(count);
            locker.unlock();
            std::this_thread::sleep_for(std::chrono::seconds(1));
            count--;
        }
    }

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
                std::cout<<"t2 got a value from t1: " <<data<<std::endl;
            }
            else
            {
                locker.unlock();
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    }
}

namespace condition_variable_deque
{
    std::deque<int> q;
    std::mutex mu;
    std::condition_variable cond;
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
        while ( data != 1)
        {
            std::unique_lock<std::mutex> locker(mu);
            cond.wait(locker, [](){ return !q.empty();} );
            data = q.back();
            q.pop_back();
            locker.unlock();
            std::cout << "t2 got a value from t1: " << data << std::endl;
        }
    }
}


void worker()
{

}

int main()
{

    std::cout << "Queueing and Dequeueing with polling" << std::endl;

    {
        std::thread t1(polling_deque::function_1);
        std::thread t2(polling_deque::function_2);

        t1.join();
        t2.join();
    }

    std::cout << "Queueing and Dequeueing with condition_variable" << std::endl;
    {
        std::thread t1(condition_variable_deque::function_1);
        std::thread t2(condition_variable_deque::function_2);

        t1.join();
        t2.join();
    }

}
