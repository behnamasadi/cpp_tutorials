// The plug-in: built as a SHARED library (libadd.so) and loaded at runtime by
// loadeding_libraries via dlopen()/dlsym().
//
// We wrap add() in extern "C" so the symbol name is plain "add" (no C++ name
// mangling). dlsym() needs that exact symbol string to find the function.

extern "C" {

int add(int a, int b) { return a + b; }
}
