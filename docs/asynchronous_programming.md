# Asynchronous Calls
In C++, asynchronous programming can be achieved using various mechanisms such as threads, `std::async`, `std::future`, and `std::promise`. One common way to create asynchronous calls is through `std::async` which runs a function asynchronously (potentially in a new thread) and returns a `std::future` that will hold the result of that function call once it completes.


## std::launch::async, std::future
Here is an example demonstrating an asynchronous call in C++ using `std::async`:

```c++
#include <iostream>
#include <future>   // For std::async and std::future
#include <thread>   // For std::this_thread::sleep_for
#include <chrono>   // For std::chrono::seconds

// A function that simulates some work by sleeping for 3 seconds
void do_work() {
    std::cout << "Work started\n";
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "Work completed\n";
}

int main() {
    // Launch do_work in a separate thread
    std::future<void> work_future = std::async(std::launch::async, do_work);
    
    std::cout << "Main thread continues executing\n";
    
    // Optionally, wait for the asynchronous task to complete
    work_future.get();
    
    std::cout << "Main thread completed\n";
    
    return 0;
}
```

Explanation:
1. We first include the necessary headers: `<future>`, `<thread>`, and `<chrono>`.
2. We define a function `do_work` that simulates some work by sleeping for 3 seconds.
3. In `main`, we launch `do_work` asynchronously using `std::async` with the launch policy `std::launch::async`, which ensures that `do_work` runs in a separate thread.
4. `std::async` returns a `std::future<void>` which we store in `work_future`. This `std::future` will become ready once `do_work` completes.
5. We print a message in the main thread to show that it continues executing while `do_work` is running.
6. Optionally, we call `work_future.get()` to wait for `do_work` to complete. This is useful if we need to ensure that `do_work` has completed before proceeding.
7. Finally, we print a message to indicate that the main thread has completed.

In this example, `do_work` runs asynchronously with respect to the main thread, allowing both to execute concurrently.


When you're working with asynchronous calls and lambda functions in C++, how you pass parameters to another lambda depends on the context and what you want to achieve. If you want to ensure that the lambda has its own copy of the parameters and that these copies won't be affected by any changes in the caller or other threads, then you should capture by value.

Here's an example where `work_future` calls another lambda and passes parameters to it. Since `std::async` returns a `std::future` that is used to get the result of the asynchronous operation, we need to work around that because `std::future` can only get a result once. To chain the work, we can create another `std::async` inside the first one:

```cpp
#include <iostream>
#include <future>
#include <thread>
#include <chrono>

int main() {
    // Parameters to pass to the first lambda
    int duration = 3;
    std::string message = "Processing asynchronously";

    // Launch a lambda function asynchronously
    std::future<void> work_future = std::async(std::launch::async,
        [duration, message]() {
            std::cout << message << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(duration));
            std::cout << "First work completed after " << duration << " seconds\n";

            // Parameters for the second lambda
            int additional_duration = 2;
            std::string additional_message = "Continuing asynchronously";

            // Launch another lambda function asynchronously
            auto inner_future = std::async(std::launch::async,
                [additional_duration, additional_message]() {
                    std::cout << additional_message << std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(additional_duration));
                    std::cout << "Second work completed after " << additional_duration << " seconds\n";
                }
            );

            // Wait for the second asynchronous task to complete
            inner_future.get();
        }
    );
    
    std::cout << "Main thread continues executing\n";
    
    // Wait for the first asynchronous task (and consequently the second) to complete
    work_future.get();
    
    std::cout << "Main thread completed\n";
    
    return 0;
}
```

Explanation:
- The first lambda captures `duration` and `message` by value.
- Inside the first lambda, after some work is done, we define parameters for the second lambda (`additional_duration` and `additional_message`), which are also captured by value in the second lambda.
- The second lambda is then launched asynchronously, and it performs its own operations independently.
- `inner_future.get()` is used to wait for the completion of the second lambda.
- After the first lambda has finished its work and has waited for the second lambda, the main thread continues and waits for the first asynchronous task to complete by calling `work_future.get()`.

By capturing by value, we ensure that each lambda has its own independent copy of the parameters, which is usually the safest way to pass parameters to asynchronous calls to avoid any potential data races or undefined behavior due to accessing shared data from multiple threads.




## Parallelization with ascync


```cpp
#include <iostream>
#include <vector>
#include <numeric>
#include <future>
#include <thread>
#include <iterator>

// Function to sum a chunk of the vector
template <typename Iterator>
long long sum_chunk(Iterator begin, Iterator end) {
    return std::accumulate(begin, end, 0LL);
}

int main() {
    // Example vector
    std::vector<int> vec(1000000, 1);  // Vector of 1 million elements, each initialized to 1

    // Determine the number of chunks based on hardware concurrency
    unsigned int num_chunks = std::thread::hardware_concurrency();
    if (num_chunks == 0) num_chunks = 2; // Fallback in case hardware_concurrency returns 0

    std::vector<std::future<long long>> futures;
    size_t chunk_size = vec.size() / num_chunks;
    auto begin = vec.begin();

    // Launch async tasks for each chunk
    for (unsigned int i = 0; i < num_chunks; ++i) {
        auto end = (i == num_chunks - 1) ? vec.end() : std::next(begin, chunk_size);
        futures.push_back(std::async(std::launch::async, sum_chunk<std::vector<int>::iterator>, begin, end));
        begin = end;
    }

    // Collect the results from each chunk
    long long total_sum = 0;
    for (auto& future : futures) {
        total_sum += future.get();
    }

    std::cout << "Total sum: " << total_sum << std::endl;

    return 0;
}
```


## Parallelization with std::packaged_task

```cpp
#include <iostream>
#include <vector>
#include <numeric>
#include <thread>
#include <future>
#include <iterator>
#include <functional>

// Function to sum a chunk of the vector
template <typename Iterator>
long long sum_chunk(Iterator begin, Iterator end) {
    return std::accumulate(begin, end, 0LL);
}

int main() {
    // Example vector
    std::vector<int> vec(1000000, 1);  // Vector of 1 million elements, each initialized to 1

    // Determine the number of chunks based on hardware concurrency
    unsigned int num_chunks = std::thread::hardware_concurrency();
    if (num_chunks == 0) num_chunks = 2; // Fallback in case hardware_concurrency returns 0

    std::vector<std::future<long long>> futures;
    std::vector<std::thread> threads;
    size_t chunk_size = vec.size() / num_chunks;
    auto begin = vec.begin();

    // Launch tasks for each chunk using std::packaged_task
    for (unsigned int i = 0; i < num_chunks; ++i) {
        auto end = (i == num_chunks - 1) ? vec.end() : std::next(begin, chunk_size);
        
        std::packaged_task<long long(std::vector<int>::iterator, std::vector<int>::iterator)> task(sum_chunk<std::vector<int>::iterator>);
        futures.push_back(task.get_future());
        
        // Move the task to a new thread and execute it
        threads.emplace_back(std::move(task), begin, end);
        
        begin = end;
    }

    // Collect the results from each chunk
    long long total_sum = 0;
    for (auto& future : futures) {
        total_sum += future.get();
    }

    // Join all threads
    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << "Total sum: " << total_sum << std::endl;

    return 0;
}

```


