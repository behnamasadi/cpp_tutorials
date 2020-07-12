#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include <functional>
#include <condition_variable>
#include <future>
std::condition_variable cond ;

class Foo {
public:
    Foo() {

    }

    void first(std::function<void()> printFirst) {

        // printFirst() outputs "first". Do not change or remove this line.
        printFirst();
    }

    void second(std::function<void()> printSecond) {

        // printSecond() outputs "second". Do not change or remove this line.
        printSecond();
    }

    void third(std::function<void()> printThird) {

        // printThird() outputs "third". Do not change or remove this line.
        printThird();
    }
};

void printFirst()
{
    std::cout<<"first";
}

void printSecond()
{
    std::cout<<"second";
}

void printThird()
{
    std::cout<<"third";
}

void worker1(int &x){std::cout<<++x<<std::endl;}

class mycalss
{
public:
    void hello(int &x)
   {std::cout<<++x<<std::endl;}
    void operator ()(int &x)
    {std::cout<<++x<<std::endl;}
};


std::mutex mu;
void sharedPrinter(int n, int id)
{
    mu.lock();
    std::cout<<"printing number "<<n <<" from thread "<<id<<std::endl;
    mu.unlock();
}

void func1()
{
    int n=100;
    for(int i=0;i<n;i++)
    {
        sharedPrinter(i, 1);
    }
}

void func2()
{
    int n=100;
    for(int i=n;i>0;i--)
    {
        sharedPrinter(i, 2);
    }
}


std::mutex mu1,mu2;


void funcdeadlock1()
{
    std::lock_guard<std::mutex>(mu1,std::adopt_lock);
    std::lock_guard<std::mutex>(mu2,std::adopt_lock);
    std::cout<<"funcdeadlock1" <<std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

}


void funcdeadlock2()
{
    std::lock_guard<std::mutex>(mu1,std::adopt_lock);
    std::lock_guard<std::mutex>(mu2,std::adopt_lock);
    std::cout<<"funcdeadlock2" <<std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

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




