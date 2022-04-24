# Explicit Constructor
The compiler is allowed to make one implicit conversion to resolve the parameters to a function. Suppose, you have a class `String`:


```cpp
class String 
{
public:
// allocate n bytes to the String object
    String(int n);
   
// initializes object with char *p    
    String(const char *p);
};
```
You can write:

```cpp
String mystring = 'a';
```

This is actually possible and character `'a'` will be implicitly converted to int (ascii code for 'a' is 97) and then the `String(int)` constructor will be called.But, perhaps this is not what you might have intended. `explicit` keyword will disable the above conversion.


```cpp
class String 
{
public:
 //allocate n bytes
    explicit String (int n);
// initialize sobject with string p    
    String(const char *p); 
};

//This is actually possible because Entity has a constructor that can get only an integer or a string
Implicit::String s1='a';

//explicit keyword will disable the above conversion, so we should do it by casting
Explicit::String s2=(const char*)'a';
```    



[code](../src/class/explicit_constructor.cpp)
