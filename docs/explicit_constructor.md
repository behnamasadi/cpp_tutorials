# Explicit Constructor
The compiler is allowed to make one implicit conversion to resolve the parameters to a function. Suppose, you have a class `String`:


```cpp
class String 
{
public:
// allocate n bytes to the String object
    String(int n)
    {
        std::cout<<"allocate "<<n<<" bytes to the String object" <<std::endl;
    }

// initializes object with char *p
    String(const char *p)
    {
        std::cout<<"initializes object with char *p" <<std::endl;
    }
};
```
You can write:

```cpp
String mystring = 'a';
```

This is actually possible and character `'a'` will be implicitly converted to int (ascii code for 'a' is 97) and then the `String(int)` constructor will be called:

```
allocate 97 bytes to the String object
```

But, perhaps this is not what you might have intended. `explicit` keyword will specify that a constructor or conversion function is explicit, which means, it cannot be used for implicit conversions and copy-initialization and the conversion will be disabled.


```cpp
class String 
{
public:
 //allocate n bytes
    explicit String (int n);
// initialize sobject with string p    
    String(const char *p); 
};
```

explicit keyword will disable the above conversion (allocate 97 bytes instead of 'a'), so we should do a casting first:

```cpp
Explicit::String s2=(const char*)'a';
```    



[code](../src/class/explicit_constructor.cpp)
