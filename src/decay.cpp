// Demo: decay in C++
// Covers array-to-pointer decay, function-to-pointer decay,
// preventing array decay via reference, and std::decay_t.

#include <iostream>
#include <type_traits>

// Receives the array by value: it decays to int*.
void takesPointer(int arr[10]) {
    std::cout << "  inside takesPointer, sizeof(arr) = "
              << sizeof(arr) << " (size of a pointer)\n";
}

// Receives the array by reference: no decay, size preserved.
template <std::size_t N>
void takesArrayByRef(int (&arr)[N]) {
    std::cout << "  inside takesArrayByRef, N = " << N
              << ", sizeof(arr) = " << sizeof(arr) << "\n";
}

// A plain function used to show function-to-pointer decay.
int add(int a, int b) {
    return a + b;
}

// Receives a function: name decays to a pointer.
void callIt(int (*f)(int, int)) {
    std::cout << "  callIt result = " << f(2, 3) << "\n";
}

int main() {
    std::cout << "--- array-to-pointer decay ---\n";
    int arr[10];
    std::cout << "  in main, sizeof(arr) = " << sizeof(arr)
              << " (10 * sizeof(int))\n";
    takesPointer(arr);

    std::cout << "--- preventing array decay (pass by reference) ---\n";
    takesArrayByRef(arr);

    std::cout << "--- function-to-pointer decay ---\n";
    callIt(add);  // 'add' decays to a function pointer

    std::cout << "--- std::decay_t ---\n";
    std::cout << std::boolalpha;
    std::cout << "  decay_t<int[10]> is int*: "
              << std::is_same_v<std::decay_t<int[10]>, int*> << "\n";
    std::cout << "  decay_t<int(int,int)> is int(*)(int,int): "
              << std::is_same_v<std::decay_t<int(int, int)>, int(*)(int, int)> << "\n";
    std::cout << "  decay_t<const int&> is int: "
              << std::is_same_v<std::decay_t<const int&>, int> << "\n";

    return 0;
}
