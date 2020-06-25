#include <fstream>
#include <mutex>
#include <thread>
#include <string>

class LogFile
{
    std::mutex mu;
    std::ofstream file;
public:
    LogFile()
    {
        file.open("log.txt");
    }

    void sharedPrint(std::string msg, int id)
    {
        std::lock_guard<std::mutex> locker(mu);
        file<<"From "<<id <<": "<<msg<<std::endl;
    }
};


namespace unique_lock
{
    class LogFile
    {
        std::mutex mu;
        std::ofstream file;
    public:
        LogFile()
        {
            file.open("log.txt");
        }

        void sharedPrint(std::string msg, int id)
        {
            std::unique_lock<std::mutex> locker(mu,std::defer_lock);

            //some operation here
            locker.lock();
            file<<"From "<<id <<": "<<msg<<std::endl;
            locker.unlock();
            //some operation here
            locker.lock();
            //...
            locker.unlock();

        }
    };
}


namespace lazy_initialization
{
    class LogFile
    {
        std::mutex mu;
        std::mutex mu_open_file;
        std::ofstream file;
        std::once_flag flag;
    public:
        LogFile()
        {

        }

        void sharedPrint(std::string msg, int id)
        {
            {
                std::unique_lock<std::mutex> locker2(mu_open_file);
                if(!file.is_open())
                {
                    file.open("log.txt");
                }
            }
            std::unique_lock<std::mutex> locker(mu,std::defer_lock);

            //some operation here
            locker.lock();
            file<<"From "<<id <<": "<<msg<<std::endl;
            locker.unlock();
            //some operation here
            locker.lock();
            //...
            locker.unlock();

        }
    };
}


namespace call_once
{
class LogFile
{
    std::mutex mu;
    std::mutex mu_open_file;
    std::ofstream file;
    std::once_flag flag;
public:
    LogFile()
    {

    }

    void sharedPrint(std::string msg, int id)
    {
        std::call_once(flag,[&](){file.open("log.txt");});
        std::unique_lock<std::mutex> locker(mu,std::defer_lock);

        //some operation here
        locker.lock();
        file<<"From "<<id <<": "<<msg<<std::endl;
        locker.unlock();
        //some operation here
        locker.lock();
        //...
        locker.unlock();

    }
};
}

int main()
{

}
