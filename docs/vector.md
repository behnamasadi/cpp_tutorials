

# vector initialization

1. Initializing by pushing/ emplacing values one by one :
    std::vector<int> vect1;
```cpp
vect1.emplace_back(10);
vect1.emplace_back(20);
vect1.emplace_back(30);
```
2. Specifying size and initializing all values. Here we create a vector of size 10 with  all values as 0.5.
```cpp
int size = 10;
double value=0.5;
std::vector<double> vect2(size, value);
```

3. Initializing like arrays
```cpp
std::vector<int> vect3{ 1, 2, 3 };
std::vector<person> vectPerson={person(10),person(12),person(3)};
```


# vector iteration


# emplace_back vs push_bask

Using `emplace_back` instead of `push_back` in a C++ `vector` when you have move constructors and move assignment operators can lead to some distinct behaviors and advantages:

1. **Direct Construction**: `emplace_back` constructs the object directly in place at the end of the `vector`. It forwards its arguments to the constructor of the element type, thereby avoiding an extra copy or move of the object. This is more efficient than `push_back`, which constructs the object and then moves or copies it into the vector.

2. **Efficiency with Move Semantics**: If you have a move constructor and move assignment operator, `emplace_back` can be more efficient. When you use `push_back`, and if the vector needs to be resized, existing elements are moved to the new storage. With move semantics, these operations are more efficient compared to deep copying, but they still entail a move operation. `emplace_back` minimizes even these move operations by constructing the element directly in its final location within the vector.

3. **No Temporary Object Creation**: With `emplace_back`, you don't create a temporary object which you then move into the vector (as you would typically do with `push_back`). This can save both time and resources, especially if the move operation is non-trivial.

4. **Perfect Forwarding**: `emplace_back` uses perfect forwarding to pass arguments to the constructor of the element type. This means you can pass arguments that are perfectly suited for the constructor of the object being emplaced, which can include lvalue references, rvalue references, or any other type of argument.

5. **Code Clarity and Intent**: By using `emplace_back`, you make it clear that your intention is to construct an object directly in the place it's going to be used. This can make the code easier to understand in terms of object lifecycle and resource management.

In summary, when you have a move constructor and move assignment operator, using `emplace_back` in a C++ `vector` can lead to more efficient object construction and resource management, as it constructs objects directly in place and fully utilizes move semantics.





since here pushing during the loop, the destination vector 
might need to resize couple of times, so we resize it once in the beginning.

```cpp
int size=10;
int value =0;
std::vector<int> src(size,value);
std::vector<int> dst;
dst.resize(src.size());
for(auto i:src)
	dst.push_back(i);
```
Refs: [1](https://www.youtube.com/watch?v=jKS9dSHkAZY), [2](http://htmlpreview.github.io/?https://github.com/HowardHinnant/papers/blob/master/insert_vs_emplace.html)

# std::copy

# erase vs remove

# front()/back() vs begin()/end()

# copying Vectors

# reinterpret vector

# vector to/from C array

# string to vector of char 
```cpp
std::string str("ali baba");
str.begin();
std::vector<char> charVec(str.begin(),str.end() );
```

# vector accessing elements
we can access vector element both by `[i]` operator and by `.at(i)` . `at(i)` is a function call while `[]` is a direct access so it is cheaper and more efficient

[source code](../src/vector.cpp)
