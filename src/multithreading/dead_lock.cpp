#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include <unistd.h>// for usleep function

#include <chrono>//for  sleep_for function
#include<mutex>
#include <condition_variable>
#include <fstream>
#include <queue>
#include <functional>
/////////////////////////////////////Dead Lock///////////////////////////////

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

void deadLockExample()
{
    std::thread thread1(func1DeadLock);
    std::thread thread2(func2DeadLock);
    thread1.join();
    thread2.join();
}

//First Solution
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

void deadLockSolvedExample()
{
    std::thread thread1(func1DeadLockSolved);
    std::thread thread2(func2DeadLockSolved);
    thread1.join();
    thread2.join();
}


//Second Solution
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

void deadLockGuardSolvedExample()
{
    std::thread thread1(func1DeadLockGuardGuaredSolved);
    std::thread thread2(func2DeadLockGuardSolved);
    thread1.join();
    thread2.join();
}

int main()
{
}
