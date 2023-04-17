

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