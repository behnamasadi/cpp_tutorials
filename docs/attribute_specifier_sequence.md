# Attribute specifier sequence

## nodiscard
The `[[nodiscard]]` attribute in C++ is a hint to the compiler that the return value of a function should not be ignored. If the return value of a function decorated with `[[nodiscard]]` is ignored, the compiler will generate a warning. This is useful for indicating that the function performs some computation whose result should be used, or that the function reports a status that should be checked.

Here are some situations where `[[nodiscard]]` could be beneficial:

### Error Codes

Functions that return error codes should typically not have their return values ignored, because doing so would mean that the error goes unchecked.

```cpp
[[nodiscard]] int performOperation() {
    // ... do something and return an error code
    return -1; // example error code
}

int main() {
    performOperation(); // Compiler warning: ignoring return value of ‘int performOperation()’, declared with attribute nodiscard [-Wunused-result]
    return 0;
}
```

### Resource Management

For functions that return some resource that must be released or closed, using `[[nodiscard]]` can remind the developer to store the result and manage the resource appropriately.

```cpp
[[nodiscard]] FILE* openFile(const char* fileName) {
    return fopen(fileName, "r");
}
```

### Logical Operations

Functions that perform checks or validations are good candidates for `[[nodiscard]]` to ensure that their results are actually used in a condition.

```cpp
[[nodiscard]] bool isValid(int x) {
    return x > 0;
}
```

### Utility Functions

Functions that perform computations are often expected to have their results used in further computations. Ignoring the result usually indicates a programmer error.

```cpp
[[nodiscard]] int add(int x, int y) {
    return x + y;
}
```

### Benefits

1. **Improved Code Quality**: Forces developers to handle results or errors, leading to more robust code.
2. **Self-Documenting Code**: Indicates the intent that the return value is significant.
3. **Easier Debugging**: Helps to catch instances where a return value is accidentally ignored, which can be a source of subtle bugs.

The `[[nodiscard]]` attribute is a part of modern C++ and was introduced in C++17. It's a part of the language's move toward enabling more robust and self-checking code.
