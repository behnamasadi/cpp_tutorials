#include <thread>
#include <iostream>
#include <string>


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


}
