#include <iostream>
#include <stdio.h> 
#include <errno.h> 
#include <string.h>  
/*
A lot of C function calls return a -1 or NULL in case of an error, so quick test on these return values are easily done with for instance an ‘if statement’.


Global Variable errno: 
    When a function is called in C, a variable named as errno is automatically assigned a code (value) which can be used to identify the type of error that has been encountered. Its a global variable indicating the error occurred during any function call and defined in the header file errno.h.


Below is a list of few different errno values and its corresponding meaning:
errno value       Error
1             Operation not permitted
2             No such file or directory
3             No such process
4             Interrupted system call
5             I/O error
6             No such device or address
7             Argument list too long
8             Exec format error
9             Bad file number
10            No child processes
11            Try again
12            Out of memory
13            Permission denied


*/

int main()
{
    FILE *fp; 
  

    fp = fopen("file.txt ", "r"); 
  

    printf("Value of errno: %d\n ", errno); 
    
/*
    strerror(): returns a pointer to the textual representation of the current errno value.
*/
    printf("The error message is : %s\n", strerror(errno)); 
    
/*
    perror: It displays the string you pass to it, followed by a colon, a space, and then the textual representation of the current errno value.
*/
    perror("Message from perror");
}
