#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include <condition_variable>
#include <future>
#include <deque>

/*
https://www.educba.com/multithreading-interview-questions-c-plus-plus/
https://www.geeksforgeeks.org/tag/cpp-multithreading/
https://www.wisdomjobs.com/e-university/c-multithreading-developer-interview-questions.html
multi thread interview questions c++
https://career.guru99.com/top-40-multithreading-interview-questions-and-answers/
https://www.indeed.com/career-advice/interviewing/multithreading-interview-questions
https://quizlet.com/24524062/c-multithreading-practice-interview-questions-flash-cards/
https://www.acodersjourney.com/top-20-cplusplus-multithreading-mistakes/
https://www.modernescpp.com/index.php/c-core-guidelines-taking-care-of-your-child-thread
c++ multi threading interview questions
https://www.geeksforgeeks.org/c-plus-plus/?ref=shm
c++ multithreading exercises
https://www.bogotobogo.com/cplusplus/quiz_multithreading.php
https://www.linuxtopia.org/online_books/programming_books/c++_practical_programming/c++_practical_programming_289.html
https://leetcode.com/problemset/concurrency/
c++ concurrency exercises
https://softwareengineering.stackexchange.com/questions/32763/how-would-you-practice-concurrency-and-multi-threading
https://www.reddit.com/r/cpp/comments/gcrp6f/concurrency_coding_exercise_ideas/
https://cs61.seas.harvard.edu/site/2018/SynchEx/
detect deadlock automatically

https://thispointer.com/different-ways-to-iterate-over-a-set-in-c/


//std::defer_lock, std::try_lock, std::adopt_lock
Timed mutex
try_lock_for and try_lock_until
mutex.try_lock()
Recursive mutex
Shared mutex
The use of this mutex's share functionality should be fairly self-explanatory. A theoretically
infinite number of readers can gain read access to the mutex, while ensuring that only a
single thread can write to the resource at any time

lock_shared()
try_lock_shared()
unlock_shared()
cond.wait_for
Condition_variable_any


https://livebook.manning.com/book/c-plus-plus-concurrency-in-action/chapter-2/4
*/


#include <time.h>
std::mutex mu;
int worker(int &input)
{
    std::this_thread::sleep_for(std::chrono::seconds(4));
    return ++input;
}


int main ()
{
    auto now=std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cout<<std::string(ctime(&now))<<std::endl;


    time_t _tm =time(NULL );

    struct tm * curtime = localtime ( &_tm );
    std::cout<<asctime(curtime);

}


//std::promise
//std::future()
//std::async



