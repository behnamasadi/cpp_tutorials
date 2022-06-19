# Memory Layout of C Programs 
- [Memory Layout of C Programs](#memory-layout-of-c-programs)
  * [Text](#text)
  * [Data](#data)
  * [BSS](#bss)
  * [Heap](#heap)
  * [Stack](#stack)


```
 ---------------------------------
 |   command line arguments      |
 |   argv[0], argv[1] ...argv[n] |
 ---------------------------------
 |       Allocated memory        |
 |-------------------------------|<--- frame pointer      ┐
 |        Return Address         |                        |
 |          Arguments            |                        |  stack frame1
 |        Local Variables        |                        |
 |        Saved registers        |                        |
 |-------------------------------|<---- stack pointer     ┘
 |                               |
 |                               |  <-stack frame2
 |-------------------------------|
 |                               |
 |                               |  <-stack frame3
 |-------------------------------|
 |                               |
 |                               |
 |                               |
 |                               |
 |                               |
 |                               |
 |              HEAP             |
 |                               |
 |-------------------------------|            ┐
 |  global/static uninitialized  |            |
 |             data              |            |<- BSS
 |                               |            |
 |-------------------------------|            ┘  ┐
 | global/static initialized data|               |<-  Data
 |-------------------------------|               ┘
 |          Code(Text)           |
 |                               | <- Program counter point to line in this region
 |                               |
 ---------------------------------
 |          Reserved             |
 |                               |
 ---------------------------------
```



## Text

All codes in the functions, variable like `char* p1 = "behnam";` go into the code (Text) region which **read only** to prevent a program from accidentally modifying its instructions. A text segment is one of the sections of a program in the memory, which contains executable instructions.


The "size" command reports the sizes (in bytes) of the Text, Data, and BSS segments. 
Consider the following code:
```cpp
int main(void)
{
    return 0;
}
```

the size command will return:

```
text	   data	    bss	    dec	    hex	filename
1806	    544	      8	   2358	    936	heap_and_stack_memory_layout_of_C_programs
```

after adding `char* p1 = "behnam";`
```cpp
int main(void)
{
    char* p1 = "behnam";
    return 0;
}
```
the size command will return:
```
text	   data	    bss	    dec	    hex	filename
1829	    544	      8	   2381	    94d	heap_and_stack_memory_layout_of_C_programs
```


as we add more code:

```cpp
void foo()
{

}

int main(void)
{
    char* p1 = "behnam";
    return 0;
}
```
the size of the text will grow:
```
text	   data	    bss	    dec	    hex	filename
1915	    544	      8	   2467	    9a3	heap_and_stack_memory_layout_of_C_programs
```


now if we change our code, this will complies:
```cpp
p1[0] = 'C';
```
but it will cause `segmentation fault` as the variable is on the `code section` and code section is read only
(`"behnam"`  is a string literal and `p1` holds the starting address of that.)


This is allowed (Value of `p1` can be changed):
```cpp
p1 = "bar";
```
However if we define `p1` as follows:
```cpp
char* const p1 = "foo";
```
since `p1` is fixed we can not change it and it is not valid:
```cpp
p1 = "bar";
```

 


## BSS
BSS stand for Block Starting Symbol and contains statically allocated variables that are declared but have not been assigned a value yet (uninitialized data).

statically: means that the lifetime (or "extent") of the variable is the entire run of the program. This is in contrast to shorter-lived automatic variables, whose storage is stack allocated and deallocated on the call stack, and in contrast to objects, whose storage is dynamically allocated and deallocated in heap memory.

uninitialized **global variables** and **static variables** are stored in BSS section.


```cpp
static int global_static;
int global_var;

int main(void)
{
    static int local_static;
}
```

## Data

Data segment contains the global variables and static variables that are **initialized** by the programmer.

```cpp
static int global_static=1;
int global_var=1;

int main(void)
{
    static int local_static=1;
}
```

if we compare the size of empty main
```
text	   data	    bss	    dec	    hex	filename
1806	    544	      8	   2358	    936	heap_and_stack_memory_layout_of_C_programs
```
with above code we can see teh difference:
```cpp
text	   data	    bss	    dec	    hex	filename
1855	    556	      4	   2415	    96f	heap_and_stack_memory_layout_of_C_programs
```

## Heap
Dynamically allocated variables go here.

```cpp
int main(void)
{
    int *p = malloc(sizeof(int)*4);
    return 0;
}
```
## Stack

It is a dynamic data structure maintained by OS to control the way procedure calling each other and parameter they pass.
Call stack maintained for each thread. The actual implementation of a stack depends on the microprocessor architecture.
It can grow up or down in memory and can move either before or after the push/pop operations

stack is used for functions. It is on top of the memory and moves downward. each function will get its own frame.
Each instance of the function has its own frame. Data that go into frame are:
1) Arguments
2) Local variables (object, data structure)
3) Saved registers.
In high level languages stack will be managed automatically. In low level languages
it is being done with stack pointer and  frame pointer.
Stack pointer:It tell us where is the boundary between allocated and unallocated memory is.
Frame pointer: is a reference pointer allowing us to know where local variable or an argument starts.



