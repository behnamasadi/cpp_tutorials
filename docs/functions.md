


# returning object from function
##  returning a reference
it's okay to return a reference if the lifetime of the object won't end after the call but never retrun a reference to a local object. Also okay for accessing things where you know the lifetime is being kept open on a higher-level, e.g.:
```cpp
struct S {
    S(int i) : m_i(i) {}

    const int& get() const { return m_i; }
private:
    int m_i;
};
```
Here we know it's okay to return a reference to `m_i` because whatever is calling us manages the lifetime of the class instance, so `m_i` will live at least that long.


When you return a value, a copy is made. The scope of the local variable ends, but a copy is made.
when a function is called, a temporary space is made for the function to put its local variables, called a frame. When the function (callee) returns its value, it puts the return value in the frame of the function that called it (caller), and then the callee frame is destroyed.

The "frame is destroyed" part is why you can't return pointers or references to local variables from functions. A pointer is effectively a memory location, so returning the memory location of a local variable (by definition: a variable within the frame) becomes incorrect once the frame is destroyed. Since the callee frame is destroyed as soon as it returns its value, any pointer or reference to a local variable is immediately incorrect.


The stack-allocated `i` will go away and you are referring to nothing.
```cpp
int& getInt() 
{
    int i;
    return i;  
}
```
even if you allocate it on the heap, the caller has to free the space:
```cpp
int& getInt() 
{
    int* i = new int;
    return *i;
}
```

delete would be totally weird and evil
```cpp
int& myInt = getInt();
delete &myInt;
```


 undefined behavior, we're wrongly deleting a copy, not the original
 ```cpp
int oops = getInt(); 
delete &oops;
```


This is very bad and the result is undefined
```cpp
int *myBadAddingFunction(int a, int b)
{
    int result;

    result = a + b;
    return &result; 
}
```

Also allocated on the stack, also bad
```cpp
char *myOtherBadFunction()
{
    char myString[256];

    strcpy(myString, "This is my string!");
    return myString;
}
```

Refs: [1](https://stackoverflow.com/questions/275214/scope-and-return-values-in-c), [2](https://stackoverflow.com/questions/752658/is-the-practice-of-returning-a-c-reference-variable-evil)

## returning a const reference

```cpp
class Complx {
private:
	double real;
	double imag;
public:
	Complx() {}
	Complx(double r, double i): real(r), imag(i) {}
	Complx  operator+(const Complx & c) const 
    {
		return Complx(real + c.real, imag + c.imag);
	}
	Complx & operator=(const Complx &c;) 
    {
		real = c.real;
		imag = c.imag;
		return *this;
	}

	double size() const 
    {
		return sqrt(real*real + imag*imag);
	}
};
```

Here in the Maximum function,
```cpp
const Complx & Maximum(const Complx &c1, const Complx &c2) 
{
	
	if (c1.size() > c2.size()) 
		return c1;
	else
		return c2;
}
```
Returning an object invokes the **copy constructor** while returning a reference doesn't. Also, the reference should be to an object that exists when the calling function is execution which in this case both of them are class members so they will exist as long as the class exist.

Refs: [1](https://www.bogotobogo.com/cplusplus/object_returning.php)


# returning a reference to a non-const object


## Return smart pointers from functions
If you want to allocate something that lives beyond the scope of the function, use a smart pointer (or in general, a container):

```cpp
std::unique_ptr<int> getInt() 
{
    return std::make_unique<int>(0);
}
```
and the caller:

```cpp
std::unique_ptr<int> x = getInt();
```


Refs: [1](https://stackoverflow.com/questions/10643563/how-to-return-smart-pointers-shared-ptr-by-reference-or-by-value)
## Passing smart pointers to functions
  
  
