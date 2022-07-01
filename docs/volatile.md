# Volatile

Consider the following code:

```cpp
int condition = 1;

while(condition == 1)
{
   std::cout<<"inside loop" <<std::endl;
}
```

If the the optimization is enabled, the compiler will find out that the program never changes the value of variable `condition`, so it may change the loop into something like `while(true)`. If the compiler doesn't optimize it, then it has to fetch the value of `condition` and compare it with `1`, in each iteration which obviously is a little bit slow.

However, sometimes, someone else might change the value of `condition` from outside the program (some memcopy operations, extern variables, other thread, a hardware that directly change the memory, etc) which compiler may not be aware. In such situations the compiler's optimization is undesired. 

To ensure that the compiler optimizer produce the desired result, you need to tell the compiler to avoid optimizing the while loop. That is where the `volatile` keyword is useful.



Refs: [1](https://www.geeksforgeeks.org/understanding-volatile-qualifier-c-set-1-introduction/), [2](https://www.geeksforgeeks.org/understanding-volatile-qualifier-in-c/)



