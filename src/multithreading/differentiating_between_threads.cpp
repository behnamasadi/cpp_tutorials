#include <thread>
#include <iostream>

void func()
{
    std::cout<<"The thread ID is:"<< std::this_thread::get_id() <<std::endl;
}

int main()
{
    std::cout<<"The thread ID is:"<< std::this_thread::get_id() <<std::endl;
    std::thread t1(func);
    std::thread t2(func);
    t1.join();
    t2.join();
}
