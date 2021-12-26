#include <thread>
#include <iostream>

int main()
{
    unsigned int numberOfMicroseconds=1000000;

    using namespace std::chrono_literals;
    std::cout<<"First way: Sleeping thread for: "<<numberOfMicroseconds<< " Micro Seconds"  <<std::endl;
    std::this_thread::sleep_for(1000000us);

    std::cout<<"Second way: Sleeping thread for: "<<numberOfMicroseconds<< " Micro Seconds"  <<std::endl;
    std::this_thread::sleep_for(std::chrono::microseconds(numberOfMicroseconds));

    std::cout<<"Third way: Sleeping thread for: "<<numberOfMicroseconds<< " Micro Seconds"  <<std::endl;
    std::this_thread::sleep_for(std::chrono::microseconds(numberOfMicroseconds));


    std::cout<<"Thread Finished" <<std::endl;

}
