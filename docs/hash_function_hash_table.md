# Hash Functions
In C++, a hash refers to a function or algorithm that takes an input (or "key") and produces a fixed-size string of characters, which is typically a hexadecimal number or a sequence of bytes. Hash functions are commonly used in data structures like hash tables and for various other purposes, such as data encryption, password storage, and digital signatures.

Here's a basic explanation of how to use a hash function in C++ with an example using the `std::hash` function from the C++ Standard Library:

```cpp
#include <iostream>
#include <functional>

int main() {
    // Create a hash function object using std::hash
    std::hash<std::string> hasher;

    // Input data (a string)
    std::string input = "Hello, World!";

    // Calculate the hash value of the input
    size_t hashValue = hasher(input);

    // Display the hash value
    std::cout << "Hash value of '" << input << "': " << hashValue << std::endl;

    return 0;
}
```

In this example, we include the `<iostream>` and `<functional>` headers, which are necessary for input/output and using the `std::hash` function, respectively.

Here's how the code works:

1. We create a hash function object called `hasher` using the `std::hash` template. In this case, we specify that we want to hash `std::string` objects.

2. We define a `std::string` variable called `input` with the string "Hello, World!" as our input data.

3. We calculate the hash value of the input string by invoking the `hasher` function with the `input` as its argument. This will return a `size_t` value representing the hash code of the input string.

4. Finally, we display the hash value using `std::cout`.

It's important to note that the `std::hash` function is suitable for basic use cases, but it's not suitable for cryptographic purposes or when you need a hash function with specific properties like collision resistance. For cryptographic purposes, you should use cryptographic hash functions like SHA-256 or SHA-3, which are available in C++ through various libraries, such as OpenSSL or the C++ Standard Library's `<cryptopp>` library.

# Hash Data Structure (Hash Table)
A hash data structure, often referred to as a hash table or hash map, is a data structure that uses a hash function to map keys to values. It allows for efficient retrieval and storage of values based on their associated keys. Hash tables are commonly used in computer science and programming for tasks like implementing dictionaries, caches, and database indexing.

Here's a basic explanation of a hash table in C++ with an example using the `std::unordered_map` container from the C++ Standard Library:

```cpp
#include <iostream>
#include <unordered_map>

int main() {
    // Create an unordered_map (hash table) to store key-value pairs
    std::unordered_map<std::string, int> hashMap;

    // Insert key-value pairs into the hash table
    hashMap["apple"] = 3;
    hashMap["banana"] = 2;
    hashMap["cherry"] = 5;

    // Access values by their keys
    std::cout << "Number of apples: " << hashMap["apple"] << std::endl;
    std::cout << "Number of cherries: " << hashMap["cherry"] << std::endl;

    return 0;
}
```

In this example, we use the `std::unordered_map` container, which is an implementation of a hash table. Here's how the code works:

1. We include the necessary header `<iostream>` for input/output and `<unordered_map>` for using the `std::unordered_map` container.

2. We create an `std::unordered_map` named `hashMap` that associates `std::string` keys with `int` values.

3. We insert key-value pairs into the hash table using the `[]` operator. For example, we associate the key "apple" with the value 3.

4. We access values from the hash table using their corresponding keys. In this case, we retrieve the number of apples and cherries from the hash table and display them using `std::cout`.

So, to clarify, the example provided above demonstrates the use of a hash data structure (hash table) in C++. It uses a hash function internally to efficiently store and retrieve values based on their keys.



[code](../src/hash.cpp)
