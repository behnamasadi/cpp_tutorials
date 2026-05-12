// Memory layout demo: print the address of one variable from each segment
// and observe where each lives in the process's virtual address space.
//
//   high address
//      stack    (locals, grows down)
//      heap     (new/malloc, grows up)
//      .bss     (zero-initialized globals/statics)
//      .data    (nonzero-initialized globals/statics)
//      .rodata  (string literals, const tables)
//      .text    (code)
//   low address

#include <cstdio>

// .data : nonzero-initialized global
int        global_initialized = 1;
static int static_initialized = 1;

// .bss  : zero-initialized global
int        global_uninitialized;
static int static_uninitialized;

// .rodata : a const table the compiler can put in read-only memory
const char rodata_string[] = "behnam";

// .text : a function lives in code
void some_function() {}

void show_text_segment() {
    printf("[.text]    address of some_function     : %p\n",
           (void*)&some_function);
}

void show_rodata_segment() {
    printf("[.rodata]  address of rodata_string     : %p\n",
           (void*)rodata_string);
}

void show_data_segment() {
    printf("[.data]    address of global_initialized: %p\n",
           (void*)&global_initialized);
    printf("[.data]    address of static_initialized: %p\n",
           (void*)&static_initialized);
}

void show_bss_segment() {
    printf("[.bss]     address of global_uninit     : %p\n",
           (void*)&global_uninitialized);
    printf("[.bss]     address of static_uninit     : %p\n",
           (void*)&static_uninitialized);
}

void show_heap() {
    int* p = new int(42);
    printf("[heap]     address from new int         : %p\n", (void*)p);
    delete p;
}

void show_stack() {
    int local_a = 0;
    int local_b = 0;
    printf("[stack]    address of local_a           : %p\n", (void*)&local_a);
    printf("[stack]    address of local_b           : %p\n", (void*)&local_b);
    // On x86-64 the stack grows down, so &local_b is typically lower than &local_a.
}

int main() {
    printf("=== .text  (code) ===\n");
    show_text_segment();

    printf("\n=== .rodata (read-only data) ===\n");
    show_rodata_segment();

    printf("\n=== .data  (initialized globals/statics) ===\n");
    show_data_segment();

    printf("\n=== .bss   (zero-initialized globals/statics) ===\n");
    show_bss_segment();

    printf("\n=== heap   (new / malloc) ===\n");
    show_heap();

    printf("\n=== stack  (locals) ===\n");
    show_stack();

    printf("\nExpected ordering (low -> high addresses):\n");
    printf("  .text  <  .rodata  <  .data  <  .bss  <  heap  <<  stack\n");
    return 0;
}
