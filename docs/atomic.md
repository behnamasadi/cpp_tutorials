Atomic operations in C++ are a set of operations provided by the `<atomic>` header, which allow for manipulation of data in a way that ensures atomicity. Atomicity is the property of an operation to be indivisible or uninterruptible. This means that, when one thread is performing an atomic operation, no other thread can see the operation in an incomplete state.

### Atomic Types in C++

The C++ Standard Library provides several atomic types, including:

1. `std::atomic_bool`: Atomic boolean type.
2. `std::atomic_char`, `std::atomic_uchar`: Atomic character types.
3. `std::atomic_int`, `std::atomic_uint`: Atomic integer types.
4. `std::atomic_long`, `std::atomic_ulong`: Atomic long types.
5. `std::atomic_llong`, `std::atomic_ullong`: Atomic long long types.
6. `std::atomic`: Template class for generic atomic operations.

### Examples

Here are some examples to illustrate the use of atomic operations in C++.

#### Example 1: Basic Atomic Integer

```cpp
#include <iostream>
#include <atomic>
#include <thread>

std::atomic_int counter(0);

void increment() {
    for (int i = 0; i < 10000; ++i) {
        counter++;
    }
}

int main() {
    std::thread t1(increment);
    std::thread t2(increment);

    t1.join();
    t2.join();

    std::cout << "Counter: " << counter << std::endl;
    return 0;
}
```

In this example, two threads `t1` and `t2` increment a shared `atomic_int` counter 10,000 times each. The atomicity of the counter ensures that the increments are correctly applied without any race conditions.

#### Example 2: Compare-and-Swap

```cpp
#include <iostream>
#include <atomic>

int main() {
    std::atomic<int> value(10);
    int expected = 10;

    if (value.compare_exchange_strong(expected, 20)) {
        std::cout << "Value changed successfully to " << value << std::endl;
    } else {
        std::cout << "Value was not changed. Expected: " << expected << ", but got: " << value << std::endl;
    }

    return 0;
}
```

This example demonstrates the compare-and-swap (CAS) operation. It attempts to change the value from 10 to 20 only if it is currently 10. The `compare_exchange_strong` function

The followings will print `200`, since each thread will increase the counter 100,
```cpp
int main() {
  int counter = 0;

  auto work = [&]() {
    for (int i = 0; i < 100; i++)
      counter += 1;
  };

  std::thread t1(work);
  std::thread t2(work);

  t1.join();
  t2.join();

  std::cout << counter << std::endl;
  return 0;
}
```

In the above example since the value `100` is very small, both of thread can finish the loop before have any effect on each other (without any interleave), 
however if we increase the length of the loop, threads might have impact each other, and update the value of counter while the other one has just read it, 
so we have data race problem.

```cpp
int main() {
  int counter = 0;

  auto work = [&]() {
    for (int i = 0; i < 1000000; i++)
      counter += 1;
  };

  std::thread t1(work);
  std::thread t2(work);

  t1.join();
  t2.join();

  std::cout << counter << std::endl;
  return 0;
}
```
which in different run gives us:

```
1098330
1261226
1617594
1188436
```


The solution is to make the `counter` of type `std::atomic<int>;`


```cpp
int main() {
  std::atomic<int> counter = 0;

  auto work = [&]() {
    for (int i = 0; i < 1000000; i++)
      counter += 1;
  };

  std::thread t1(work);
  std::thread t2(work);

  t1.join();
  t2.join();

  std::cout << counter << std::endl;
  return 0;
}
```

Refs: [1](https://www.youtube.com/watch?v=ZWjVa1kfE7c)
