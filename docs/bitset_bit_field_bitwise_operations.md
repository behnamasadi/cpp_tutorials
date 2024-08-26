# bitset
1. A bitset is an array of bool but each Boolean value is not stored separately instead bitset optimizes the space such that each bool takes `1` bit space only,
so space taken by bitset `bs` is less than that of `bool bs[N]`.  
2. `N` must be known at compile time.  
3. The size of bitset is fixed at compile time that is, it can’t be changed at runtime.


```cpp
const int M=sizeof(int)*8;
```
default constructor initializes with all bits 0 
```cpp
bitset<M> bset1; 
```
bset2 is initialized with bits of 20 
```cpp
bitset<M> bset2(20); 
```
bset3 is initialized with bits of specified binary string 
```
bitset<M> bset3(string("1100")); 
```
printing exact bits representation of bitset:
```cpp
cout << bset1 << endl; // 00000000000000000000000000000000 
cout << bset2 << endl; // 00000000000000000000000000010100 
cout << bset3 << endl; // 00000000000000000000000000001100 
```

declaring set8 with capacity of 8 bits:
```cpp
bitset<8> set8; // 00000000 
```
setting first bit (or 6th index) :
```cpp
set8[1] = 1; // 00000010 
set8[4] = set8[1]; // 00010010 
cout << set8 << endl; 
```  
count function returns number of set bits in bitset:

```cpp
int numberof1 = set8.count(); 
```
size function returns total number of bits in bitset so there difference will give us number of unset(0)  bits in bitset: 

```cpp
int numberof0 = set8.size() - numberof1; 
cout << set8 << " has " << numberof1 << " ones and "  << numberof0 << " zeros\n"; 
```

test function return 1 if bit is set else returns 0:

```cpp
cout << "bool representation of " << set8 << " : "; 
for (std::size_t i = 0; i < set8.size(); i++) 
    cout << set8.test(i) << " "; 

cout << endl; 
```
any function returns true, if atleast 1 bit is set:
```cpp
if (!set8.any()) 
    cout << "set8 has no bit set.\n"; 

if (!bset1.any()) 
    cout << "bset1 has no bit set.\n"; 
```
none function returns true, if none of the bit is set:
```cpp
if (!bset1.none()) 
    cout << "bset1 has some bit set\n"; 
```

`bset.set()` sets all bits:
```cpp
cout << set8.set() << endl; 
```
`set.set(pos, b)`makes `set[pos] = b`:
```cpp
cout << set8.set(4, 0) << endl; 
```

`set.set(pos)`makes `set[pos] = 1` i.e. default is 1:
```cpp
cout << set8.set(4) << endl; 
```
reset function makes all bits 0:
```cpp
cout << set8.reset(2) << endl; 
cout << set8.reset() << endl; 
```
flip function flips all bits i.e.  ` <-> 0` and  `0 <-> 1`:
```cpp
cout << set8.flip(2) << endl; 
cout << set8.flip() << endl; 
```
Converting decimal number to binary by using bitset:
```cpp
int num = 100; 
bitset<8> set9(num);
cout << "\nDecimal number: " << set9.to_ulong()<< "  Binary equivalent: " << set9<<endl; 
```
Refs: [1](https://www.geeksforgeeks.org/c-bitset-interesting-facts/), [2](https://www.geeksforgeeks.org/c-bitset-and-its-application/)


# bit field  
  
[code](../src/bitset_bit_field.cpp)


# bitwise operations

```
&	bitwise AND
|	bitwise inclusive OR
^	bitwise XOR (eXclusive OR)
<<	left shift 
>>	right shift
~	bitwise NOT (one's complement) (unary)
 
&=	bitwise AND assignment
|=	bitwise inclusive OR assignment
^=	bitwise exclusive OR assignment
<<=	left shift assignment
>>=	right shift assignment
```

Bitwise	Logical:

```
a & b	a && b
a | b	a || b
a ^ b	a != b
~a	!a
```


bitwise shift to calculate POW2: `[variable]<<[number of places to shift]`

```cpp
int number=2;
int power=5
int result=number<<(power-1);
std::cout <<result<<std::endl;
```

even/ odd number:

```cpp
int num=6;
int result=num&1;
std::cout<<num << (result ? " is odd":" is even")<<std::endl;
```

shifting bits:

```cpp
int a,b, result;
a=13;//1101
b=2;
result=(a>>b);//0011
std::cout <<result<<std::endl;
```
`a >>= 2` means "set a to itself shifted by two bit to the right"

```cpp
std::cout <<(a >>= 2) <<std::endl;
```


### std::bit_xor
The `std::bit_xor` function in C++20 offers several advantages over the traditional `^` operator:

**1. Generic Type Support:**

- `std::bit_xor` can operate on a wider range of integer types, including custom integral types.
- This provides more flexibility and type safety in your code.

**2. Potential Performance Improvements:**

- While the performance difference between `std::bit_xor` and `^` might not be significant in most cases, the `std::bit_xor` implementation could potentially benefit from compiler optimizations or hardware-specific instructions.

**3. Consistency with Other Bitwise Operations:**

- `std::bit_xor` is part of the `std::bit` namespace, which also includes other bitwise operations like `std::bit_and`, `std::bit_or`, `std::bit_not`, and `std::bit_count`.
- Using these functions consistently can improve code readability and maintainability.

**4. Future Enhancements:**

- As the C++ standard evolves, `std::bit_xor` might gain additional features or optimizations in the future.

**In summary:**

While the `^` operator is generally sufficient for most XOR operations, `std::bit_xor` provides a more generic, type-safe, and potentially performant alternative. If you need to work with custom integer types or prefer a more consistent approach for bitwise operations, `std::bit_xor` is a good choice. However, in most cases, the performance difference between the two methods will be negligible.



Refs: [1](https://www.geeksforgeeks.org/bitwise-algorithms/)

