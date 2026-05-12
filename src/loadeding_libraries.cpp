// Host program: loads libadd.so at runtime and calls its add() function.
//
// Steps:
//   1. dlopen("./libadd.so", RTLD_LAZY)  -> get a handle to the shared object.
//   2. dlsym(handle, "add")              -> look up the "add" symbol.
//   3. Cast the void* to a function pointer of the right signature and call it.
//   4. dlclose(handle)                   -> release the shared object.
//
// Build note (CMake): the plug-in is a SHARED library; loadeding_libraries
// links against libdl (${CMAKE_DL_LIBS}). MODULE libraries are similar but
// cannot be linked against at link time -- they exist only to be dlopen()ed.
//
// Run from the build dir so "./libadd.so" resolves:
//     ./Debug/loadeding_libraries          # if libadd.so sits next to it
// or set LD_LIBRARY_PATH=. before running.

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  void *handle;
  int (*adder_fn_ptr)(int, int);
  char *error;

  handle = dlopen("./libadd.so", RTLD_LAZY);
  if (!handle) {
    fputs(dlerror(), stderr);
    exit(1);
  }

  adder_fn_ptr = (int (*)(int, int))dlsym(handle, "add");
  if ((error = dlerror()) != NULL) {
    fputs(error, stderr);
    exit(1);
  }

  printf("add(2, 3) = %d\n", adder_fn_ptr(2, 3));
  dlclose(handle);
  return 0;
}
