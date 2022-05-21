# Track Memory Allocations

If we override the new operator globally, we are asking the compiler to do not use the "new" operator that come with std library but use ours.

```cpp
void * operator new (size_t size)
{
    std::cout<<"Allocating "<<size<<" bytes" <<std::endl;
    return malloc(size);
}

void  operator delete(void * memory, size_t size)
{
    std::cout<<"Freeing "<<size<<" bytes" <<std::endl;
    free(memory);
}

void  operator delete(void * memory )
{
    std::cout<<"Freeing " <<std::endl;
    free(memory);
}
```
Now if create instance of `S`:

```cpp
std::cout<<"size of struct S is: "<<sizeof(S)<<" bytes" <<std::endl;
S* my_S=new S;
delete my_S;
```
The output is:

```
size of struct S is: 16
Allocating 16 bytes
Freeing 16 bytes
```


if we check the memory allocation for STL container such as strings:


```cpp
std::string str;
std::cout<<"allocating and freeing memory for strings" <<std::endl;
for(std::size_t i=0;i<25;i++)
{
	str =str+std::to_string(i);
	std::cout<<"size of string is: "<<str.size()<<" bytes and string is: "<< str <<std::endl;
}
```

we will observer the followings in the output:

```
allocating and freeing memory for strings
size of string is: 1 bytes and string is: 0
size of string is: 2 bytes and string is: 01
size of string is: 3 bytes and string is: 012
size of string is: 4 bytes and string is: 0123
size of string is: 5 bytes and string is: 01234
size of string is: 6 bytes and string is: 012345
size of string is: 7 bytes and string is: 0123456
size of string is: 8 bytes and string is: 01234567
size of string is: 9 bytes and string is: 012345678
size of string is: 10 bytes and string is: 0123456789
size of string is: 12 bytes and string is: 012345678910
size of string is: 14 bytes and string is: 01234567891011
Allocating 31 bytes
size of string is: 16 bytes and string is: 0123456789101112
Allocating 31 bytes
Freeing 
size of string is: 18 bytes and string is: 012345678910111213
Allocating 31 bytes
Freeing 
size of string is: 20 bytes and string is: 01234567891011121314
Allocating 31 bytes
Freeing 
size of string is: 22 bytes and string is: 0123456789101112131415
Allocating 31 bytes
Freeing 
size of string is: 24 bytes and string is: 012345678910111213141516
Allocating 31 bytes
Freeing 
size of string is: 26 bytes and string is: 01234567891011121314151617
Allocating 31 bytes
Freeing 
size of string is: 28 bytes and string is: 0123456789101112131415161718
Allocating 31 bytes
Freeing 
size of string is: 30 bytes and string is: 012345678910111213141516171819
Allocating 33 bytes
Freeing 
size of string is: 32 bytes and string is: 01234567891011121314151617181920
Allocating 35 bytes
Freeing 
size of string is: 34 bytes and string is: 0123456789101112131415161718192021
Allocating 37 bytes
Freeing 
size of string is: 36 bytes and string is: 012345678910111213141516171819202122
Allocating 39 bytes
Freeing 
size of string is: 38 bytes and string is: 01234567891011121314151617181920212223
Allocating 41 bytes
Freeing 
size of string is: 40 bytes and string is: 0123456789101112131415161718192021222324
Freeing 
```

This shows us that small size STL containers (`m_size <= 16`), would be set on stack instead of heap and after the size get bigger they would be allocated on heap, this is called **The Small String Optimization**

Refs: [1](https://www.youtube.com/watch?v=sLlGEUO_EGE)

