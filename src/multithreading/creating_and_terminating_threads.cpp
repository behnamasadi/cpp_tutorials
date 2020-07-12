#include <thread>
#include <iostream>
#include <string>
#include <future>
#include <functional>
#include <mutex>
#include <iostream>

// A callable object
void functionPointer(int n)
{
    std::cout<<++n <<std::endl;
}

void functionPointerByRef(std::string &msg)
{
    msg="New value!";
}

// A callable object
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
    int n=12;

    // function pointer
    std::thread t1(functionPointer,n );
    std::thread t2{functionPointer,n };


    // function object
    std::thread t3(callableObject1(),n);


    // This will not compile and cause most vexing parse
    // std::thread t4(callableObject2());

    // first solution
    std::thread t4((callableObject2()));

    // second solution
    callableObject2 threadObj ;
    std::thread t5(threadObj);

    //lambda expression
    auto lambdaExpression = [](int n) {std::cout<<++n <<std::endl;};
    std::thread t6(lambdaExpression,n);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();


    std::string msg="Old Value";
    std::thread  t7(functionPointerByRef, std::ref(msg));
    t7.join();
    std::cout<< msg<<std::endl;

    std::thread::hardware_concurrency();


    unsigned int con_threads;
    // calculating number of concurrent threads  supported in the hardware implementation
    con_threads = std::thread::hardware_concurrency();
    std::cout << "Number of concurrent threads supported are: " << con_threads << std::endl;


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

}
