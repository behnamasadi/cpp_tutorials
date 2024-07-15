# std::packaged_task

`std::packaged_task` is a class template that wraps a callable (like a function or lambda) so that it can be invoked **asynchronously**. It allows you to manage the callable and its future result.


1. **Define a callable**: This could be a function, a lambda, or a function object.

2. **Create a `std::packaged_task` object**: Pass the callable to the `std::packaged_task`.

3. **Get the future**: Use the `get_future()` method to get a `std::future` object that will hold the result of the callable.

4. **Invoke the task**: You can either call the `operator()` of `std::packaged_task` directly or pass it to a thread.

6. **Retrieve the result**: Use the `std::future` object to get the result.

Here's the complete example:

The callable:
```cpp
int adder(int a, int b) {
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  return a + b;
}
```
simple call:

```cpp
void task_simple() {
  std::packaged_task<int(int, int)> adder_task(adder);
  std::future<int> result = adder_task.get_future();
  adder_task(2, 3);
  std::cout << "packaged_task called" << std::endl;
  std::cout << result.get() << std::endl;
}
```

asynchronous tasks
```cpp
void task_thread() {
  std::packaged_task<int(int, int)> adder_task(adder);
  std::future<int> result = adder_task.get_future();
  std::thread t1(std::move(adder_task), 2, 3);
  std::cout << "packaged_task called" << std::endl;
  t1.join();
  std::cout << result.get() << std::endl;
}
```


main:
```cpp
int main() {
  task_simple();
  task_thread();
}
```


## Advantages of packaged_task

`std::packaged_task` provides several advantages when dealing with asynchronous tasks in C++. Here are some of the key benefits:

### 1. **Separation of Task and Execution**:
- `std::packaged_task` allows you to decouple the creation of a task from its execution. You can define a task and decide when and how to execute it later, which gives you more control over task management.

### 2. **Future Result Management**:
- By using `std::packaged_task`, you can obtain a `std::future` object that allows you to retrieve the result of the task once it's completed. This provides a straightforward way to handle the results of asynchronous operations.

### 3. **Exception Handling**:
- If the task throws an exception, it is automatically stored in the associated `std::future` object. When you call `future.get()`, the exception is rethrown, allowing for centralized and consistent error handling.

### 4. **Integration with Standard Library Concurrency Mechanisms**:
- `std::packaged_task` integrates well with other concurrency tools provided by the C++ standard library, such as `std::thread`, `std::async`, and `std::promise`. This allows for flexible and powerful concurrency management.

### 5. **Ease of Use with Lambdas and Callable Objects**:
- You can easily wrap lambdas or callable objects in a `std::packaged_task`, making it versatile and easy to use with different kinds of callables.

### 6. **Flexibility in Task Execution**:
- `std::packaged_task` allows you to execute the task synchronously or asynchronously, providing flexibility in how tasks are run. You can call the task directly or run it in a separate thread.

### Example of Advantages:

```cpp
#include <iostream>
#include <future>
#include <thread>
#include <exception>

// A simple function that may throw an exception
int divide(int a, int b) {
    if (b == 0) {
        throw std::runtime_error("Division by zero");
    }
    return a / b;
}

int main() {
    try {
        // Create a packaged_task wrapping the divide function
        std::packaged_task<int(int, int)> task(divide);

        // Get the future object to retrieve the result later
        std::future<int> result = task.get_future();

        // Execute the task in a separate thread
        std::thread t(std::move(task), 10, 0);  // This will throw an exception

        // Wait for the task to complete and get the result
        t.join();
        
        // Try to get the result, this will rethrow the stored exception
        std::cout << "Result: " << result.get() << std::endl;
    } catch (const std::exception &e) {
        // Handle the exception
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }

    return 0;
}
```

### Explanation of Example:
- **Exception Handling**: In this example, if the `divide` function throws an exception (e.g., division by zero), the exception is stored in the `std::future` object. When `result.get()` is called, the exception is rethrown, allowing for centralized exception handling.
- **Separation of Task and Execution**: The task is created and packaged separately, and its execution is controlled later by passing it to a thread.
- **Future Result Management**: The result (or exception) of the task is managed through a `std::future` object.

By leveraging these advantages, `std::packaged_task` helps to create robust and manageable asynchronous programs in C++.



## When to use packaged_task and when should you avoid it

### When to Use `std::packaged_task`

`std::packaged_task` is a useful tool in certain scenarios where you need to manage asynchronous tasks and their results. Here are some situations where it is appropriate to use `std::packaged_task`:

1. **Custom Thread Management**:
   - When you need precise control over task execution and threading, `std::packaged_task` allows you to wrap a callable and run it in a thread of your choosing.

2. **Deferred Task Execution**:
   - If you want to define a task but defer its execution until a later time or event, `std::packaged_task` lets you encapsulate the task and execute it when needed.

3. **Combining with Other Asynchronous Primitives**:
   - When you need to integrate with other concurrency mechanisms like `std::promise`, `std::future`, or custom thread pools, `std::packaged_task` can be a versatile tool.

4. **Handling Exceptions**:
   - If you need robust exception handling for asynchronous tasks, `std::packaged_task` automatically captures exceptions thrown by the callable and stores them in the associated `std::future`.

### Example Use Case

Here's an example where `std::packaged_task` is useful:

```cpp
#include <iostream>
#include <future>
#include <thread>

void example() {
    auto async_task = [](int a, int b) {
        if (b == 0) throw std::runtime_error("Division by zero");
        return a / b;
    };

    std::packaged_task<int(int, int)> task(async_task);
    std::future<int> result = task.get_future();

    std::thread t(std::move(task), 10, 0);

    try {
        t.join();
        std::cout << "Result: " << result.get() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }
}
```

### When to Avoid `std::packaged_task`

There are scenarios where `std::packaged_task` might not be the best choice:

1. **Simple Asynchronous Calls**:
   - For simple asynchronous tasks where you don't need fine-grained control over the task or threading, `std::async` is often a more straightforward choice.

2. **Thread Pool Management**:
   - If you are managing a thread pool or need to handle a large number of tasks efficiently, using a task scheduler or thread pool library (like `std::async`, `Boost.Asio`, or a custom thread pool) might be more appropriate.

3. **Complex Lifetime Management**:
   - `std::packaged_task` is not copyable, which can complicate situations where tasks need to be stored or passed around. If you need more flexible lifetime management,



