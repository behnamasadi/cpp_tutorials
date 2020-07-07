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


int main()
{
    /**/
    Foo foo;
    std::future<void>  threadA=std::async(&Foo::first,foo,printFirst);
    threadA.get();
    std::thread  threadB(&Foo::second,foo,printSecond);
    std::thread  threadC(&Foo::third,foo,printThird);


//    threadA.join();
//    threadB.join();
//    threadC.join();

}



