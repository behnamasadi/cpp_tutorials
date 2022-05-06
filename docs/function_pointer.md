# Function Pointer
A function pointer is a variable that stores the address of a function that can later be called through that function pointer.

```cpp
int adder(int a, int b)
{
    return a+b;
}
```

now in your code you can have the following variable `adder_fn_ptr`:

```cpp
int (*adder_fn_ptr)(int, int);
```
binding:

```cpp
adder_fn_ptr=adder; 
```
or
```cpp 
adder_fn_ptr=&adder
```

calling:

```cpp
std::cout<<adder_fn_ptr(2,3) <<std::endl;
```
or
```cpp
std::cout<< (*adder_fn_ptr) (2,3) <<std::endl;
```

Another example

```cpp
void foreach(std::vector<int> values, void(UnaryFunc)(int))
{
    for(auto value:values)
        UnaryFunc(value);
} 
```
some unary functions:

```cpp 
void print(int i)
{
    std::cout<<i <<std::endl;
}
```

Now we can have the followings:

```cpp
std::vector<int> values={1,2,3,4};
foreach(values,print);
```    
or

```cpp        
std::vector<int> values={1,2,3,4};
foreach(values,[](int value){std::cout<<value <<std::endl;});
```
