# malloc
allocates the specified number of bytes, returning a pointer to the beginning of the block.  
`void * malloc ( size_t size );`

```cpp
#include <stdlib.h>


int * x=(int *)malloc(10 * sizeof(int));
if(x==NULL)
	exit(1);

x[0]=12;
x[1]=2;
free(x);
```
# calloc
Allocates memory for an array of num objects of size and initializes all bytes in the allocated storage to zero.
Due to the alignment requirements, the number of allocated bytes is not necessarily equal to num*size.  
`void* calloc( size_t num, size_t size );`

allocate and zero out an array of 4 int
```cpp
int *p1 = calloc(4, sizeof(int));    
```

same as above, naming the array type directly
```cpp
int *p2 = calloc(1, sizeof(int[4])); 
if(p2) {
for(int n=0; n<4; ++n) // print the array
    printf("p2[%d] == %d\n", n, p2[n]);
}

free(p1);
free(p2);
```
# realloc

expanding or contracting the existing area pointed to by pointer, if it shrinking, the contents of the area remain **unchanged** up to the lesser of the new and old sizes. If the area is expanded, the contents of the new part of the array are **undefined.**

```cpp
int * x=(int *)malloc(4 * sizeof(int));
if(x==nullptr)
	exit(1);

x[0]=12;
x[1]=2;
x[2]=4;
x[3]=6;
x=(int *)realloc(x,10 * sizeof(int));
x[4]=14;
x[5]=61;

free(x);
```
another example:
```cpp
char *str;

str = (char *) malloc(10);
strcpy(str, "Behnam");

str = (char *) realloc(str, 15);
strcat(str, "Asadi");

free(str);
```





