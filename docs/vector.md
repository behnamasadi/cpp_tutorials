# Vector Initialization

1) Initializing by pushing/ emplacing values one by one :
    std::vector<int> vect1;
```cpp
vect1.emplace_back(10);
vect1.emplace_back(20);
vect1.emplace_back(30);
```
2) Specifying size and initializing all values. Here we create a vector of size 10 with  all values as 0.5.
```cpp
int size = 10;
double value=0.5;
std::vector<double> vect2(size, value);
```

3) Initializing like arrays
```cpp
std::vector<int> vect3{ 1, 2, 3 };
std::vector<person> vectPerson={person(10),person(12),person(3)};
```


# Vector Iteration


# emplace_back vs push_bask

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
Refs: [1](https://www.youtube.com/watch?v=jKS9dSHkAZY)

# std::copy

# erase VS remove

# front()/back() vs begin()/end()

# Copying Vectors

# Reinterpret Vector

# Vector to/from C Array

# String to Vector of Char 
```cpp
std::string str("ali baba");
str.begin();
std::vector<char> charVec(str.begin(),str.end() );
```

# Vector Accessing Elements
we can access vector element both by `[i]` operator and by `.at(i)` . `at(i)` is a function call while `[]` is a direct access so it is cheaper and more efficient

[source code](../src/vector.cpp)
