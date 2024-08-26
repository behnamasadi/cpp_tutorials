# Unions

The purpose of the union is to save memory by using the same portion of memory for accessing different data types. All three variables `ui`, `hi, lo`, and `uc[4]` 
are pointing to the exact location in the memory, changing one of them will be seen by others.

```cpp
union mix_type 
{
    unsigned int ui; // 4 bytes
    struct 
    {
        unsigned short hi; // 2 bytes
        unsigned short lo; // 2 bytes
    } s;
    unsigned char uc[4];// 4x1 bytes
} ;
```
The above union is stored as the following in the memory:
```    
  ---------------------    
  |    |    |    |    |
  ---------------------
  mix_type
  hi         lo
  c[0] c[1] c[2] c[4] 
```


Lets creat a variable of type `mix_type`

```cpp
mix_type mix;
```
Now this:

```cpp
mix.uc[0]=1;
mix.uc[1]=1;
mix.uc[2]=1;
mix.uc[3]=1;
```
will be stored in the meomory as this: 

```
00000001000000010000000100000001
```
which can be printed with the following casting:
```cpp
std::cout<< mix.ui <<std::endl;  
std::cout<< std::bitset<8*sizeof(mix_type)>(mix.ui) <<std::endl;
std::bitset<8*sizeof(mix_type)> union_bitset;
union_bitset[0]=1;
union_bitset[8]=1;
union_bitset[16]=1;
union_bitset[24]=1;
std::cout<< union_bitset.to_ulong() <<std::endl;
std::cout<< union_bitset.to_string() <<std::endl;
std::cout<<sizeof(unsigned int)  <<std::endl;
```

The union could also be used for aliasing. For instance:

```cpp
union Vector3
{
  struct{ double x,y,z ; } ;
  double elements[3];
}
```

Iterating by index:

```cpp
for( int i = 0 ; i < 3 ; i++ )
  Vector3_object.elements[i]=1.0;
```
Accessing by name:

```cpp
Vector3_object.x=1.0;
Vector3_object.y=1.0;
Vector3_object.z=1.0;
```



### Real-World Applications of `union` in C++

1. **Memory Optimization:**
   - In systems with limited memory, like embedded systems, using a union can save memory by allowing different data types to share the same memory location.
   - Example: If you have a data structure where only one of several data members will be used at a time (e.g., a message system that could send either a text, an image, or a video), using a union would be more memory-efficient than a structure.

   ```cpp
   union Data {
       int i;
       float f;
       char str[20];
   };
   ```

   Here, `Data` can store either an integer, a floating-point number, or a string, but not all at the same time.

2. **Interfacing with Hardware:**
   - Unions are often used in low-level programming, particularly when working directly with hardware. For example, in embedded systems, you might need to access a hardware register that can be interpreted as different types.
   - Example: A hardware register might be accessed as an 8-bit integer, a bit field, or an array of smaller integers.

   ```cpp
   union Register {
       uint16_t allFlags;
       struct {
           uint8_t lowByte;
           uint8_t highByte;
       } parts;
   };
   ```

   This union allows the register to be accessed as a whole 16-bit value or as two 8-bit parts.

3. **Variant Data Types:**
   - A union can be used to implement variant types, which are common in situations where a variable might store one of several different types, like in parsers, interpreters, or dynamic type systems.
   - Example: A simple type-safe union can be used to hold an integer, a float, or a character, ensuring that only one type is active at any given time.

   ```cpp
   union Variant {
       int i;
       float f;
       char c;
   };

   struct SafeVariant {
       enum class Type { INT, FLOAT, CHAR } type;
       union {
           int i;
           float f;
           char c;
       };
   };
   ```

   Here, `SafeVariant` keeps track of which type is currently stored in the union, making it easier to handle.

4. **Type Punning:**
   - Unions are sometimes used in type punning, which allows treating a piece of memory as if it were a different type. This can be useful for tasks like serialization/deserialization or interpreting binary data in different ways.
   - Example: Converting between different representations of a number.

   ```cpp
   union FloatInt {
       float f;
       uint32_t i;
   };

   FloatInt fi;
   fi.f = 3.14f;
   std::cout << std::hex << fi.i << std::endl;  // Print the hexadecimal representation of the float.
   ```

   This code demonstrates interpreting the same memory as either a float or an integer.

### Application of `union` in a C++ Program

Here is a simple program that demonstrates using a union for memory efficiency.

```cpp
#include <iostream>
#include <cstring>

union Data {
    int i;
    float f;
    char str[20];
};

int main() {
    Data data;

    data.i = 10;
    std::cout << "data.i: " << data.i << std::endl;

    data.f = 220.5;
    std::cout << "data.f: " << data.f << std::endl;

    std::strcpy(data.str, "Hello");
    std::cout << "data.str: " << data.str << std::endl;

    std::cout << "Accessing data.i now gives: " << data.i << std::endl;

    return 0;
}
```

### Explanation:
- In this program, `Data` is a union that can store an integer, a float, or a string.
- Initially, `data.i` is assigned the value 10. However, when `data.f` is assigned the value 220.5, it overwrites the memory location, so accessing `data.i` again would give a different value.
- Finally, `data.str` is assigned the string "Hello", which again overwrites the previous value. Accessing the integer or float values after this point would give unpredictable results.
  
This example shows how unions are used to manage different data types in the same memory location, emphasizing the importance of knowing which type is currently stored in the union.

