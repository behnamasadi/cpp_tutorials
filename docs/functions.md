


# returning object from function

## Scope of return values
When you return a value, a copy is made. The scope of the local variable ends, but a copy is made.

 when a function is called, a temporary space is made for the function to put its local variables, called a frame. When the function (callee) returns its value, it puts the return value in the frame of the function that called it (caller), and then the callee frame is destroyed.

The "frame is destroyed" part is why you can't return pointers or references to local variables from functions. A pointer is effectively a memory location, so returning the memory location of a local variable (by definition: a variable within the frame) becomes incorrect once the frame is destroyed. Since the callee frame is destroyed as soon as it returns its value, any pointer or reference to a local variable is immediately incorrect.

```cpp
int *myBadAddingFunction(int a, int b)
{
    int result;

    result = a + b;
    return &result; // this is very bad and the result is undefined
}

char *myOtherBadFunction()
{
    char myString[256];

    strcpy(myString, "This is my string!");
    return myString; // also allocated on the stack, also bad
}
```

Refs: [1](https://stackoverflow.com/questions/275214/scope-and-return-values-in-c)
## Return smart pointers from functions
## Passing smart pointers to functions
  
  
