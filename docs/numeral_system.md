In C++, a numeral system, also known as a number system or base, is a mathematical notation used to represent numbers. The most common numeral system used in everyday mathematics is the decimal system, which is base 10. However, C++ allows you to work with numbers in various numeral systems, including:

1. Decimal (Base 10): This is the default numeral system used in C++. Decimal numbers use the digits 0 to 9.

```cpp
int decimalNumber = 123; // Decimal number
```

2. Binary (Base 2): Binary numbers use only two digits, 0 and 1. In C++, you can represent binary literals by prefixing them with "0b" or "0B."

```cpp
int binaryNumber = 0b1101; // Binary number (13 in decimal)
```

3. Octal (Base 8): Octal numbers use digits 0 through 7. You can represent octal literals by prefixing them with "0."

```cpp
int octalNumber = 075; // Octal number (61 in decimal)
```

4. Hexadecimal (Base 16): Hexadecimal numbers use digits 0-9 and the letters A-F (or a-f) to represent values 10-15. You can represent hexadecimal literals by prefixing them with "0x" or "0X."

```cpp
int hexNumber = 0x1A; // Hexadecimal number (26 in decimal)
```

C++ provides ways to convert between different numeral systems using functions and manipulators like `std::hex`, `std::oct`, and `std::dec`. For example:

```cpp
#include <iostream>

int main() {
    int decimalNumber = 13;
    std::cout << "Decimal: " << decimalNumber << std::endl;

    std::cout << "Binary: " << std::hex << decimalNumber << std::endl;
    std::cout << "Octal: " << std::oct << decimalNumber << std::endl;
    std::cout << "Hexadecimal: " << std::dec << decimalNumber << std::endl;

    return 0;
}
```

This program converts a decimal number to binary, octal, and hexadecimal representations using the appropriate manipulators. The `std::hex`, `std::oct`, and `std::dec` manipulators control the base in which numbers are displayed in the `std::cout` stream.
