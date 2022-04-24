# Aggregate Initialization


It means the use of brace-enclosed `{}` **initializer lists** to initialize all members of an aggregate (i.e. an array or struct). This type of initialization safe guards one from over stepping the boundary and provides for initializing an array with set values.

```cpp

class intArray
{
public:
    int size;
    int * data;

    intArray(int size)
    {
        size = size;
        data = new int[size];
    }

    ~intArray()
    {
        delete[] data;
    }

    intArray(std::initializer_list<int> list)
    {
        size = list.size();
        data = new int[list.size()];

        //std::initializer_list doesn't provide a subscript operator

        int count{ 0 };
        for (auto element : list)
        {
            data[count] = element;
            ++count;
        }

    }
};
```

so far the only way to use our array is like this:
```cpp
intArray array1(4);
array1.data[0] =7 ;
array1.data[1] = 3;
array1.data[2] = 5;
array1.data[3] = 6;
```

with `initializer_list`, we can send an array directly:
```cpp
intArray array2({ 1,2,3,4,5,6,7,8 });
```

[code](../src/aggregate_initialization.cpp)


# Copy initialization
Initializes an object from another object.


```cpp
T object = other;
```

```cpp	
T object = {other};
```

when passing an argument to a function by value:
```cpp
f(other)
```


when returning from a function that returns by value:	
```cpp	
return other;
```

```cpp
T array[N] = {other-sequence};	(6)	
```


# Default Initialization
This is the initialization performed when an object is constructed with no initializer with the following syntax:

```cpp
T object;	
new T;	
```
Example:

```cpp

class Foo
{
    public: int i;
};


Foo inst1;
Foo* inst2 = new Foo;
std::cout<<inst1.i <<std::endl;
std::cout<<inst2->i <<std::endl;
```
































Consider the followings:


```cpp

class Foo
{
    public: int i;
};


Foo x;
Foo * p = new Foo;
```
Since there is no user-defined constructor, this just means that all members are default-initialized. Default-initializing a fundamental data type like int means "no initialization".


Now consider the followings:

```cpp
Foo* inst3 = new Foo();

Foo inst4 = Foo();
```
This is value initialization. **Value initialization** for fundamental types means zero-initialization, (variables are initialized to zero).















//https://www.modernescpp.com/index.php/initialization
// uniformInitialization.cpp

#include <map>
#include <vector>
#include <string>

// Initialization of a C-Array as attribute of a class
class Array{
  public:
    Array(): myData{1,2,3,4,5}{}    
  private:
    int myData[5];
};

class MyClass{			
  public: 
    int x;
    double y;
};

class MyClass2{		
  public:
    MyClass2(int fir, double sec):x{fir},y{sec} {};
  private: 
    int x;
    double y;
};

int main1(){

  // Direct Initialization of a standard container
  int intArray[]= {1,2,3,4,5};   
  std::vector<int> intArray1{1,2,3,4,5};  
  std::map<std::string,int> myMap{{"Scott",1976}, {"Dijkstra",1972}};

  // Initialization of a const heap array
  const float* pData= new const float[3]{1.1,2.2,3.3};

  Array arr;

  // Defaut Initialization of a arbitrary object   
  int i{};                // i becomes 0
  std::string s{};        // s becomes ""
  std::vector<float> v{}; // v becomes an empty vector
  double d{};             // d becomes 0.0
	
  // Initializations of an arbitrary object using public attributes	
  MyClass myClass{2011,3.14};      
  MyClass myClass1= {2011,3.14};    

  // Initializations of an arbitrary object using the constructor
  MyClass2 myClass2{2011,3.14};     
  MyClass2 myClass3= {2011,3.14};   

}
































































[code](../src/{}-operator-aggregate-copy-default-direct-value-zero.cpp)

Refs: [1](https://stackoverflow.com/questions/8860780/what-does-value-initializing-something-mean)
