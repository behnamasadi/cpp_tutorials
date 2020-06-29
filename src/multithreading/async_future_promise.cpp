#include <future>
#include <iostream>
#include <functional>


int factorialSharedFuture(std::shared_future<int> f)
{
    int value=1;
    int n=f.get();
    for(int i=1;i<=n;i++)
        value=i*value;
    return value;
}

int factorialFuture(std::future<int>& f)
{
    int value=1;
    int n=f.get();
    for(int i=1;i<=n;i++)
        value=i*value;
    return value;
}

int factorial(int input)
{
    int value=1;
    for(int i=1;i<=input;i++)
        value=i*value;
    return value;
}

int main()
{
    {
        int input=8;
        int output;
        std::future<int> future_thread= std::async(factorial,input);
        output=future_thread.get();
        std::cout<< output<<std::endl;
    }

    {
        int input=8;
        int output;
        std::future<int> future_function= std::async(std::launch::async, factorial,input);
        output=future_function.get();
        std::cout<< output<<std::endl;
    }

    {
        int input=8;
        int output;
        std::promise<int> p;
        std::future<int> inputFuture=p.get_future();
        std::future<int> future_promise_factorial= std::async(std::launch::async, factorialFuture,std::ref(inputFuture) );
        //some code here
        p.set_value(input);
        //p.set_exception(std::make_exception_ptr(std::runtime_error("Value couldn't be assigned")));
        output=future_promise_factorial.get();
        std::cout<< output<<std::endl;
    }

    {
        std::promise<int> p;
        std::future inputFuture=p.get_future();
        std::shared_future<int> inputFutureShared=inputFuture.share();
        std::future<int> future_promise_factorial1= std::async(std::launch::async, factorialSharedFuture,inputFutureShared );
        std::future<int> future_promise_factorial2= std::async(std::launch::async, factorialSharedFuture,inputFutureShared );
        std::future<int> future_promise_factorial3= std::async(std::launch::async, factorialSharedFuture,inputFutureShared );
    }
}
