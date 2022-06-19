# Loading Classes Dynamically From Plugins

Let say you are developing an application where you want allow the user or third-party developers to extend it without recompiling your code or injecting code directly into your executable. For instance you are developing a robotic application and you want to give this freedom to the user such that the user can add his path planner so the robot moves in the way he want, or the user uses his own numerical solver.


The solution is a plugin in system. You can find find many examples of that in you daily life. Audio editing software that use audio plug-ins (Audacity plugin for example), Games that support modding, Text editors, Photo editing software (Photoshop plugin for instance) all work based on 
plugin in system.


 The host application provides services which the plug-in can register themselves with the host application. One simple approach is a specified directory that host application search for DLLs or shared objects. If any are found, they would be loaded to the program. Ok lets create one.


## Creating the Plug-in
First create two files to your project, the first one `adder.cpp` and past the following code inside of that:


```cpp
extern "C" 
{

  int add(int a,int b)
  {
	return a+b;
  }
}
```

and in your `CMakeLists.txt`:

```
add_library(add SHARED  src/add.cpp)
```

## Loading The Plug-in in Host

The second file is `loadeding_libraries.cpp` and paste the following inside:


```cpp
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>


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

    printf ("%d\n", adder_fn_ptr(2,3));
    dlclose(handle);
    return 0;
}
```

Now in your `CMakeLists.txt`:
```
add_executable(loadeding_libraries src/loadeding_libraries.cpp )
target_link_libraries(loadeding_libraries dl)
```

