#include <iostream>
#include <thread>
//#include <chrono>         // std::chrono::seconds

#include <vector>
#include <algorithm>

void doWork()
{
    std::this_thread::sleep_for(std::chrono::seconds(10));
    std::cout<<"do work" <<std::endl;
    std::this_thread::get_id();
}



int main()
{
    std::thread worker(doWork);
    std::cout<<"main thread" <<std::endl;
    //worker.join();
    //std::cin.get();
}
