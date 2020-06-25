#include <iostream>
#include <thread>
#include <mutex>
#include <fstream>
#include <queue>


namespace noMutex
{
    void function2()
    {
        unsigned int sleepTime=1;//milliseconds
        for(std::size_t i=0;i<100;i++)
        {
            std::cout<<"Message from function1: " <<i<<std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime) );
        }
    }

    void function1()
    {
        unsigned int sleepTime=1;//milliseconds
        for(int i=0;i>-100;i--)
        {
            std::cout<<"Message from function1: " <<i<<std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime) );
        }
    }
}

namespace mutexWithNoDeadLockProtection
{
    std::mutex mu;
    void sharedPrinter(std::string s,int id)
    {
        mu.lock();
        std::cout<<"Message from function"<<id<<": " <<s<<std::endl;
        mu.unlock();
    }

    void function2()
    {
        unsigned int sleepTime=1;//milliseconds
        for(std::size_t i=0;i<100;i++)
        {
            sharedPrinter( std::to_string(i) ,2);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime) );
        }
    }

    void function1()
    {
        unsigned int sleepTime=1;//milliseconds
        for(int i=0;i>-100;i--)
        {
            sharedPrinter( std::to_string(i) ,1);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime) );
        }
    }
}

namespace mutexWithLockGuard
{
    std::mutex mu;
    void sharedPrinter(std::string s,int id)
    {
        //here when gaurd goes out of scope, mutex get unlocked
        std::lock_guard<std::mutex> gaurd(mu);
        std::cout<<"Message from function"<<id<<": " <<s<<std::endl;
    }

    void function2()
    {
        unsigned int sleepTime=1;//milliseconds
        for(std::size_t i=0;i<100;i++)
        {
            sharedPrinter( std::to_string(i) ,2);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime) );
        }
    }

    void function1()
    {
        unsigned int sleepTime=1;//milliseconds
        for(int i=0;i>-100;i--)
        {
            sharedPrinter( std::to_string(i) ,1);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime) );
        }
    }

}

namespace mutexWithStreamProtection
{
    class LogFile
    {
        std::mutex mutex;
        //RAII (Resource acquisition is initialization)
    public:
        std::ofstream fileObj;
        LogFile(std::string logFile="Log.txt")
        {
            fileObj.open(logFile,std::ofstream::out);
        }

        void sharedPrinter(std::string s,int id)
        {
            std::lock_guard<std::mutex> guard(mutex);
            fileObj <<"Message from function"<<id<<": " <<s<<std::endl;
        }

        ~LogFile()
        {
            fileObj.close();
        }
    };

    void function1Log(LogFile &log)
    {
        unsigned int sleepTime=10;//milliseconds
        for(int i=0;i>-100;i--)
        {
            log.sharedPrinter(std::to_string(i),1);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime) );
        }
    }

    void function2Log(LogFile &log)
    {
        unsigned int sleepTime=10;//milliseconds
        for(int i=0;i<100;i++)
        {
            log.sharedPrinter(std::to_string(i),2);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime) );
        }
    }

}

class walletMutex 
{
public:
    int money;
    std::mutex mutex;
    walletMutex():money(0){}
    
    
    void increaseMoney(int amount)
    {
        mutex.lock();
        money=money+amount;
        mutex.unlock();
    }
};


int fixingRaceConditions()
{
    walletMutex walletObject;
    std::vector<std::thread> threads;
    for(int i = 0; i < 5; ++i)
    {
        threads.push_back(std::thread(&walletMutex::increaseMoney, &walletObject, 10));
    }
    
    for(int i = 0; i < threads.size() ; i++)
    {
        threads.at(i).join();
    }
    return walletObject.money;
}

void fixingRaceConditionsExample()
{
    int val = 0;
    for(int k = 0; k < 10000; k++)
    {
        if((val = fixingRaceConditions()) != 50)
        {
            std::cout << "Error at count = "<<k<<" Money in Wallet = "<<val << std::endl;
        }
    }
}


int main()
{
    {
        std::thread t1(noMutex::function1);
        std::thread t2(noMutex::function2);
        t1.join();
        t2.join();
    }

    {
        std::thread t1(mutexWithNoDeadLockProtection::function1);
        std::thread t2(mutexWithNoDeadLockProtection::function2);
        t1.join();
        t2.join();
    }

    {
        mutexWithStreamProtection::LogFile log;
        std::thread t1(mutexWithStreamProtection::function1Log,std::ref(log));
        std::thread t2(mutexWithStreamProtection::function2Log,std::ref(log));
        t1.join();
        t2.join();
    }


}
