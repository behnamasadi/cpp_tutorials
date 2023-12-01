# Literals
In C++, literals are a fundamental concept. They represent fixed values in your code that do not change. There are several types of literals in C++, including integer literals, floating-point literals, character literals, and string literals. Understanding them is key to programming effectively in C++. Here's a breakdown of why we need literals and some real-world examples:

### 1. Integer Literals
- **Use**: Represent integer values.
- **Example**: `int age = 30;`
- **Real-World Application**: Storing a fixed number like the number of days in a week (`int daysInWeek = 7;`).

### 2. Floating-Point Literals
- **Use**: Represent decimal values.
- **Example**: `double price = 99.99;`
- **Real-World Application**: Setting a price for a product (`double productPrice = 29.95;`).

### 3. Character Literals
- **Use**: Represent a single character.
- **Example**: `char grade = 'A';`
- **Real-World Application**: Storing a grade received in a test (`char testGrade = 'B';`).

### 4. String Literals
- **Use**: Represent a sequence of characters.
- **Example**: `string greeting = "Hello, World!";`
- **Real-World Application**: Storing user messages or labels (`string welcomeMessage = "Welcome to our app!";`).

### 5. Boolean Literals
- **Use**: Represent true or false values.
- **Example**: `bool isAdult = true;`
- **Real-World Application**: Checking if a user is eligible for an adult membership (`bool adultMembership = (age >= 18);`).

### Why Do We Need Literals?
- **Simplicity**: They provide a straightforward way to represent constant values.
- **Readability**: Makes code easier to read and understand.
- **Efficiency**: Since they are constants, the compiler can optimize their usage.

### Real-World Example:
Imagine you're writing a program for a coffee shop. Here, you'll use various literals:

```cpp
int numberOfCups = 1; // Integer literal
double pricePerCup = 2.50; // Floating-point literal
char size = 'M'; // Character literal
string coffeeType = "Espresso"; // String literal
bool hasMilk = false; // Boolean literal
```

In this example, literals are used to set values like the number of cups, price, size of the cup, type of coffee, and whether it has milk or not. This makes the code easy to read and understand, and allows you to work with fixed values effectively.


### Suffix-Modified Literals
In C++, you can modify the type of a numeric literal by adding a suffix. This is especially useful for specifying the exact type of a large number or when you need to ensure a certain level of precision.

- **Long Integers**: You can specify a `long` or `long long` integer by adding `l` (lowercase L) or `ll` to an integer. For example, `long int li = 20l;` explicitly states that `20` is a `long int`.
- **Unsigned Integers**: Adding `u` or `U` denotes an unsigned integer. For instance, `unsigned int ui = 100u;`.
- **Long Double**: For floating-point literals, adding `l` or `L` makes it a `long double`. For example, `long double ld = 123.456L;`.

These suffixes ensure that the literal has the correct type for the intended use, which is crucial for memory management, precision, and operations where type matters.

### Integer Literals
1. **Unsigned Suffix**:
   - `u` or `U`: Indicates an unsigned integer.
     - Example: `100u` or `100U` - Unsigned integer.

2. **Long Suffix**:
   - `l` or `L`: Indicates a long integer.
     - Example: `100l` or `100L` - Long integer.

3. **Long Long Suffix**:
   - `ll` or `LL`: Indicates a long long integer.
     - Example: `100ll` or `100LL` - Long long integer.

4. **Unsigned Long**:
   - Combine `u` or `U` with `l` or `L`.
     - Example: `100ul`, `100UL`, `100lu`, or `100LU` - Unsigned long integer.

5. **Unsigned Long Long**:
   - Combine `u` or `U` with `ll` or `LL`.
     - Example: `100ull`, `100ULL`, `100llu`, or `100LLU` - Unsigned long long integer.

### Floating-Point Literals
1. **Float Suffix**:
   - `f` or `F`: Indicates a float.
     - Example: `100.0f` or `100.0F` - Float.

2. **Long Double Suffix**:
   - `l` or `L`: Indicates a long double.
     - Example: `100.0l` or `100.0L` - Long double.

### Examples of Usage
- **Integer Literal**: `int x = 42;` (no suffix, defaults to `int`)
- **Unsigned Integer**: `unsigned int y = 42u;`
- **Long Integer**: `long z = 42L;`
- **Long Long Integer**: `long long a = 42LL;`
- **Unsigned Long**: `unsigned long b = 42UL;`
- **Float**: `float c = 42.0f;`
- **Double**: `double d = 42.0;` (no suffix, defaults to `double`)
- **Long Double**: `long double e = 42.0L;`

These suffixes help to explicitly specify the type and size of a literal, which is important for precision, memory allocation, and ensuring correct operations in contexts where the type matters.


### User-Defined Literals (UDL)
User-defined literals allow you to extend the language by defining your own literals. This is powerful for creating domain-specific languages or for making certain kinds of code more readable and expressive.

- **Syntax**: A UDL is defined by a literal operator with the syntax `operator "" _suffix()`. The `_suffix` is a user-defined sequence of characters that follows the literal value.
- **Example**: 
  ```cpp
  long double operator "" _km(long double miles) {
      return miles * 1.60934;
  }

  auto distance = 5.0_km; // User-defined literal
  ```
  In this example, the `_km` suffix is used to convert miles to kilometers.

- **Use Cases**: They are particularly useful in applications where specific units of measure, encoding styles, or domain-specific concepts are frequently used.

### Example of Suffix-Modified and User-Defined Literals
Consider an application for astronomy. You might need to represent large distances in light years or parse specific formats of star data:

```cpp
long long int distanceToStar = 1234567890123LL; // Long long int literal

long double operator "" _ly(long double lightYears) {
    return lightYears * 9.461e+15; // Convert light years to kilometers
}

auto proximaCentauriDistance = 4.24_ly; // User-defined literal
```

In this example, `distanceToStar` uses a long long int literal for a very large number, while `proximaCentauriDistance` uses a UDL to represent the distance to Proxima Centauri in kilometers, calculated from light years. This makes the code more readable and directly expresses domain-specific concepts.

Refs: [1](https://www.dummies.com/article/technology/programming-web-design/cplusplus/creating-c-literals-with-prefixes-and-suffixes-147634/),
[2](https://www.programiz.com/cpp-programming/variables-literals#:~:text=C%2B%2B%20Literals,2.5%20%2C%20'c'%20etc.), 
[3](https://stackoverflow.com/questions/38626341/advantages-of-using-user-defined-literal-for-strings-instead-of-string-literal)


[code](../src/literals.cpp)
