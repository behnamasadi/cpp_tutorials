/*
 * Timers.cpp
 *
 *  Created on: Oct 31, 2012
 *      Author: behnam
 */


#include <iostream>
#include <time.h>


#include <ctime>
#include <chrono>
#include <iomanip>
#include <thread>


template<typename Clock, typename Duration>
std::ostream &operator<<(std::ostream &stream,  const std::chrono::time_point<Clock, Duration> &time_point)
{
    const time_t time = Clock::to_time_t(time_point);
    struct tm tm;
    localtime_r(&time, &tm);
    return stream << std::put_time(&tm, "%c"); // Print standard date&time
 
}


void timeLapseFirstExample()
{
    auto start = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(std::chrono::microseconds(1000));  
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    std::cout<<"time lapse in microseconds:" <<microseconds <<std::endl;
}


void timeLapseSecondExample()
{
    /*  get start time  */
    time_t  start_time = clock();
/*code to measure*/
	for(int i=0;i<1000000;i++)
	{
        std::this_thread::sleep_for(std::chrono::microseconds(1000));
	}
    float time1 = (float) (clock() - start_time) / CLOCKS_PER_SEC;
    printf("time for code was %f seconds\n", time1);
}


int main()
{
//     timeLapseFirstExample();
    timeLapseSecondExample();
    return 0;
}

