#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

/*
add.cpp

extern "C" 
{

  int add(int a,int b)
  {
	return a+b;
  }
}

in the CMakeLists.txt

ADD_LIBRARY(add SHARED  src/add.cpp)

ADD_EXECUTABLE(loadeding_libraries src/loadeding_libraries.cpp )
TARGET_LINK_LIBRARIES(loadeding_libraries dl)

SHARED libraries are linked dynamically and loaded at runtime.
MODULE libraries are plugins that are not linked into other targets but may be
loaded dynamically at runtime using dlopen.
you can link to a SHARED library with the linker,
but you cannot link to a MODULE with the linker. On some platforms.
*/

int main(int argc, char ** argv)
{
    void *handle;
    int (*adder_fn_ptr)(int, int);
    char *error;

    handle = dlopen ("./libadd.so", RTLD_LAZY);
    if (!handle)
    {
        fputs (dlerror(), stderr);
        exit(1);
    }

    adder_fn_ptr = (int (*)(int, int))  dlsym(handle, "add");
    if ((error = dlerror()) != NULL)
    {
        fputs(error, stderr);
        exit(1);
    }

//    printf ("%d\n", (*adder_fn_ptr)(2,3));
    printf ("%d\n", adder_fn_ptr(2,3));
    dlclose(handle);
    return 0;
}
