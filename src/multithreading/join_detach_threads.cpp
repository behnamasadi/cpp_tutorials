#include <algorithm>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

//#include <unistd.h>// for usleep function
#include <chrono> //for  sleep_for function
#include <condition_variable>
#include <fstream>
#include <functional>
#include <mutex>
#include <queue>

////////////////////////////////////Join, Detach
///Threads//////////////////////////////

void print(int a, int &b) { std::cout << a << " " << b << std::endl; }

void joiningThreads() {
  /*
  join will pause the current thread unitill the called threads are done,
  imagine in your main you have 10 threads to load the GUI,...you need to wait
  until they all done then you can continue. if you don't put join thread, you
  main function might return before even your threads finish their jobs
  */
  int a = 4;
  int b = 2;
  std::vector<std::thread> threadList;
  for (int i = 0; i < 10; i++) {
    threadList.push_back(std::thread(print, a, std::ref(b)));
  }
  // Now wait for all the worker thread to finish i.e.
  // Call join() function on each of the std::thread object
  std::cout << "wait for all the worker thread to finish" << std::endl;
  std::for_each(threadList.begin(), threadList.end(),
                std::mem_fn(&std::thread::join));
  std::cout << "Exiting from Main Thread" << std::endl;
  return;
}

void createFile(std::string filename, std::string data) {

  std::ofstream fileObj(filename, std::ofstream::out);
  fileObj << data;
  fileObj.close();
}

void threadFileWriter() {
  unsigned int milliseconds = 1000;
  for (std::size_t i = 0; i < 10; i++) {
    createFile(std::to_string(i) + ".txt", std::to_string(i));
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
  }
}

void detachingThreads() {
  /*
  After calling detach(), std::thread object is no longer associated with the
  actual thread of execution.

  In the following example we detach the thread from main, main will terminate,
  but our thread is working in the background and every second create 1 file for
  10 seconds.
  */
  std::thread threadObj(threadFileWriter);

  std::cout << std::this_thread::get_id() << std::endl;
  threadObj.detach();
}

void joinableDetachable() {
  /*
  When a join() function is called on an thread object (thread object=main
  function for instance), then when the join() returns then thread object (main
  function) has no associated thread with it. In case again join() function is
  called on such object then it will cause the program to Terminate. Similarly
  calling detach() makes the std::thread object not linked with any thread
  function
  */
  int a, b;
  a = 3;
  b = 5;
  std::thread threadObj(print, a, std::ref(b));

  if (threadObj.joinable()) {
    std::cout << "Detaching Thread " << std::endl;
    threadObj.detach();
  }
  if (threadObj.joinable()) {
    std::cout << "Detaching Thread " << std::endl;
    threadObj.detach();
  }
}

void processTree() {
  // pstree -p PID
}

int main() {}
