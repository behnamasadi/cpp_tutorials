#include <thread>
#include <future>
#include <functional>
#include <mutex>
#include <iostream>

class myClass
{
public:
    void f(int x, char c){}
    long g(double x){return 0;}
    int operator()(int n){return 0;}
};


void foo(int x)
{}
int main()
{
    myClass myObject;
    std::thread t1(myObject,6);
    std::thread t2(std::ref(myObject),6);
    std::thread t3(myClass(),6);
    std::thread t4([](int n) {std::cout<<++n <<std::endl;},6 );
    std::thread t5(foo,7);
    std::thread t6(&myClass::f, myObject,8,'w');
    std::thread t7(&myClass::f, &myObject,8,'w');
    std::thread t8(&myClass::f, std::move(myObject),8,'w');


    std::async(std::launch::async,myObject,6);
    std::bind(myObject,6);
    std::once_flag flag;
    std::call_once(flag,myObject,6);


}
