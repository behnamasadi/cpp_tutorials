- [String Encoding](#string-encoding)
  * [ASCII](#ascii)
  * [ANSI](#ansi)
  * [ASCII Extended (extended ASCII" or "8-bit ASCII)](#ascii-extended--extended-ascii--or--8-bit-ascii-)
  * [Unicode](#unicode)
  * [Encodings: UTF-8 vs UTF-16 vs UTF-32](#encodings--utf-8-vs-utf-16-vs-utf-32)
- [Strings in Windows](#strings-in-windows)
  * [8-bit AnsiStrings](#8-bit-ansistrings)
  * [16-bit UnicodeStrings](#16-bit-unicodestrings)
  * [Depending on UNICODE define](#depending-on-unicode-define)
  * [Checklist for Windows String Programming](#checklist-for-windows-string-programming)
- [Literals and String Data Types](#literals-and-string-data-types)
- [C string](#c-string)
  * [Difference between char * and char[]](#difference-between-char---and-char--)
  * [size of string](#size-of-string)
  * [strdup](#strdup)
  * [strstr](#strstr)  
  * [String copying](#string-copying)
- [C++ String](#c---string)
  * [warning iso c++ forbids converting a string constant to ‘char*’ -wwrite-strings](#warning-iso-c---forbids-converting-a-string-constant-to--char----wwrite-strings)
- [String to Number Conversion](#string-to-number-conversion)
  * [int, float, double to string](#int--float--double-to-string)
- [Number to String Conversion](#number-to-string-conversion)
  * [char to a string](#char-to-a-string)
  * [float to string](#float-to-string)
  * [int to string](#int-to-string)
  * [double to string](#double-to-string)
  * [string to vector of char](#string-to-vector-of-char)
- [Spiting String By Delimiter](#spiting-string-by-delimiter)
- [string_view](#string-view)
- [Find String Case Insensitive](#find-string-case-insensitive)
- [The small string optimization](#the-small-string-optimization)




# String Encoding
## ASCII 
ASCII uses 7 bits to represent a character. By using 7 bits, we can have a maximum of 2^7 (= 128)
distinct combinations. Which means that we can represent 128 characters maximum.
The last bit (8th) is used for avoiding errors as parity bit. Most ASCII characters are printable
characters of the alphabet such as abc, ABC, 123, ?&!, etc. The others are control
characters such as carriage return, line feed, tab, etc. ASCII was meant for English only.

## ANSI
The main difference between ANSI and ASCII is the number of characters they can represent.
ASCII was the first to be developed and when its limitations were reached, ANSI was one of the
ways created to expand the number of characters that can be represented in an encoding.
In ANSI, 8 bits are used; increasing the maximum number of characters to be represented up to 256.
This is expanded even further because of how ANSI uses code pages with different character sets.
There are a number of ANSI code pages that are meant for other languages like Japanese, Chinese,
and many others. The application processing the file just needs to know which code page is in use
in order to decipher the files properly.

There are many variants of Extended ASCII characters (8-bit system) to cover regional characters
and symbols. One example is the extended ASCII characters which includes various letters needed for
writing languages of Western Europe and certain special characters. This encoding is called ISO Latin-1
or ISO 8859-1, (ISO refers to International Organization for Standardization), which is the default
character set in most browsers. The ISO 8859-1 character set includes the original ASCII character
set (values 0 to 127), plus an extended character set (codes from 160-255) which contains the characters
used in Western European countries and some commonly used special characters. Many Windows systems use
another related 8-bit encoding, and this Microsoft specific encoding is referred to as ANSI, or Windows-1252.
It is similar to ISO 8859-1 except that character codes 128-159 in ISO 8859-1 are reserved for controls
whereas ANSI uses most of them for printable characters. ANSI stands for American National Standards Institute.
The ANSI character set includes the standard ASCII character set (values 0 to 127), plus an extended
character set (values 128 to 255).

Refs: [1](https://www.excelanytime.com/excel/index.php?option=com_content&view=article&id=116:ascii-code-extended-ascii-characters-8-bit-system-and-ansi-code&catid=78&Itemid=474)

## ASCII Extended (extended ASCII" or "8-bit ASCII)
Some clever people started using the 8th bit (the bit used for parity) to encode more characters to support
their language (to support `é`, in French, for example). Just using one extra bit doubled the size of the
original ASCII table to map up to 256 characters (2^8 = 256 characters).

## Unicode
ASCII Extended solves the problem for languages that are based on the Latin alphabet but not other languages
that are completely different (Greek, Russian, Chinese) We would have needed an entirely new character set.
Unicode doesn't contain every character from every language.
[unicode-table](https://unicode-table.com/)


You need to "encode" this abstract representation. That's where an [encoding](https://en.wikipedia.org/wiki/Character_encoding) comes into play
Character encoding: is used to map every character to series of bits, number  or electrical pulse
(Morse code, ASCII, Unicode)

## Encodings: UTF-8 vs UTF-16 vs UTF-32
- `UTF-8` and `UTF-16` are variable length encoding.
- In `UTF-8`, a character may occupy a minimum of 8 bits.
- In `UTF-16`, a character length starts with 16 bits.
- `UTF-32` is a fixed length encoding of 32 bits.

Refs: [1](https://en.wikipedia.org/wiki/Comparison_of_Unicode_encodings)



# Strings in Windows

## 8-bit AnsiStrings
- `char`: 8-bit character - underlying C/C++ data type
- `CHAR`: alias of char - Windows data type
- `LPSTR`: null-terminated string of CHAR (Long Pointer)
- `LPCSTR`: constant null-terminated string of CHAR (Long Pointer)

char is supposed to hold a character, usually an 8-bit character.
`wchar_t` is supposed to hold a wide character, and then, things get tricky:
On Linux, a wchar_t is 4 bytes, while on Windows, it's 2 bytes.
neither char nor wchar_t is directly tied to unicode!.


## 16-bit UnicodeStrings
- `wchar_t`: 16-bit character - underlying C/C++ data type
- `WCHAR`: alias of `wchar_t` - Windows data type
- `LPWSTR`: null-terminated string of `WCHAR` (Long Pointer)
- `LPCWSTR`: constant null-terminated string of `WCHAR` (Long Pointer)

## Depending on UNICODE define

- `TCHAR`: alias of WCHAR if UNICODE is defined; otherwise CHAR
- `LPTSTR`: null-terminated string of TCHAR (Long Pointer)
- `LPCTSTR`: constant null-terminated string of TCHAR (Long Pointer)


| Item              | 8-bit        | 16-bit      | Varies          |
|-------------------|--------------|-------------|-----------------|
| character         | CHAR         | WCHAR       | TCHAR           |
| string            | LPSTR        | LPWSTR      | LPTSTR          |
| string (const)    | LPCSTR       | LPCWSTR     | LPCTSTR         |


LPCTSTR = L‌ong P‌ointer to a C‌onst T‌CHAR STR‌ing. 
a long pointer is the same as a pointer. There were two flavors of pointers under 16-bit windows.

```cpp
LPSTR = char*
LPCSTR = const char*
LPWSTR = wchar_t*
LPCWSTR = const wchar_t* (This type is declared in WinNT.h as follows: typedef CONST WCHAR *LPCWSTR;)
LPTSTR = char* or wchar_t* depending on _UNICODE
LPCTSTR = const char* or const wchar_t* depending on _UNICODE
```


This type is declared in WinNT.h as follows:
```cpp
#ifdef UNICODE
typedef LPCWSTR LPCTSTR; 
#else
typedef LPCSTR LPCTSTR;
#endif
```


`std::string` is a basic_string templated on a char, and `std::wstring` on a `wchar_t`.

| Type	                 | Definition                       | 
|-------------------     |--------------------------------- |
|std::string	         |        std::basic_string<char>   |
|std::wstring	         |std::basic_string<wchar_t>        |
|std::u8string (C++20)	 |std::basic_string<char8_t>        |
|std::u16string (C++11) |	std::basic_string<char16_t> |
|std::u32string (C++11) | 	std::basic_string<char32_t> |


Refs: [1](https://stackoverflow.com/questions/28917431/what-do-t-and-l-mean-in-c-and-how-can-i-pass-them)

## Checklist for Windows String Programming
1) `_T()`, and its Win32 equivalent `TEXT()`, are preprocessor macros that prepend the input value with L if
_UNICODE or UNICODE are defined, respectively.
The `_T()` macro was added when you needed to support Windows NT and later (which support Unicode) and Windows 9x/ME
(which do not). These days any code using these macros is obsolete, since all modern Windows versions are Unicode-based.
`_T("Hello")` //if defined UNICODE, change "Hello" into UNICODE; otherwise, keep it in ANSI.
The plain versions without the underscore affect the character set the Windows header files treat as default. 
So if you define UNICODE, then GetWindowText will map to GetWindowTextW instead of GetWindowTextA, for example. 
Similarly, the TEXT macro will map to L"..." instead of "...".
The versions with the underscore affect the character set the C runtime header files treat as default. 
So if you define _UNICODE, then _tcslen will map to wcslen instead of strlen, for example. Similarly, 
the _TEXT macro will map to L"..." instead of "...". UNICODE is used by Windows headers, whereas _UNICODE is used by C-runtime/MFC headers.

2) Use `_TCHAR` and `_T()` with C functions. Use `TCHAR` and `TEXT()` with the Win32 API.
CString is based on the TCHAR data type.", so use TEXT()

3) Use `LPTSTR` and `LPCTSTR` instead of `char *` and `const char *`

- `LPCSTR` is a pointer to a const string
- `LPCTSTR` is a pointer to a const TCHAR string, (TCHAR being either a wide char or char depending on whether UNICODE is defined in your project)
- `LPTSTR` is a pointer to a (non-const) TCHAR string

For C++ strings, use `std::wstring` instead of `std::string`

You don't need to use <const > in <const char *> when you define c style strings.
The reason is you don't want to increase or decrease the  length  of your string as it has fixed sized memory.
Just because it is <char *> it doesn't mean it is in heap, and we don't call delete.
it is null terminated character \0.
Char are initialized with single quotation '' If you use double quotation ""  it is char *

# Literals and String Data Types

A string literal or anonymous string is a type of literal for the representation of a string value. 
in `x = "foo"`, where `"foo"` is a string literal with value `foo`. Literal are of type `char` in C but `const char` in C++


```cpp
auto c0 = 'A'; // char
auto c1 = u8'A'; // char
auto c2 = L'A'; // wchar_t
auto c3 = u'A'; // char16_t
auto c4 = U'A'; // char32_t

// Multicharacter literals
auto m0 = 'abcd'; // int, value 0x61626364

// String literals
auto s0 = "hello"; // const char*
auto s1 = u8"hello"; // const char*, encoded as UTF-8
auto s2 = L"hello"; // const wchar_t*
auto s3 = u"hello"; // const char16_t*, encoded as UTF-16
auto s4 = U"hello"; // const char32_t*, encoded as UTF-32

const char* multiline = R"(line1
line2
line3)";
```


# C string 
Strings are actually one-dimensional array of characters terminated by a null character '\0'. 
Here the `name` is a stack variable:
```cpp
char name[10] = { 'b','e','h','n','a','m','\0' };
```
compiler output:
```cpp
char name[10] = {'b', 'e', 'h', 'n', 'a', 'm', '\0', '\0', '\0', '\0'};
```


stack variable,
```cpp
char name[] = { 'b','e','h','n','a','m','\0' };
```
compiler output:
```cpp
char name[7] = {'b', 'e', 'h', 'n', 'a', 'm', '\0'}
```

stack variable, 
```cpp
char name[] = "behnam";
```
compiler output: 
```cpp
char name[7] = "behnam";
```

it is on the **code section** of memory, this type of definition is not recommend
```cpp
char* name = "behnam";
```
and it is better to use 
```cpp
const char* name = "behnam";
```

This will complies:
```cpp
name[0] = 'C';
```
but it will cause `segmentation fault` as the variable is on the `code section` and code section is read only
(`"behnam"`  is a string literal and `name` holds the starting address of that.)


This is allowed (Value of `name` can be changed):
```cpp
name = "Margarethe";
```


this code is okay since there is a `'\0'` at the end of string:
```cpp
std::cout << "name: " << name << std::endl;
```
this is also okay:

```cpp
char name[7] = { 'b','e', 'h', 'n', 'a','m','\0' }; //or = { 'b','e', 'h', 'n', 'a','m', 0 };
std::cout << "name: " << name << std::endl;
```

but this code will print lots of strange character until it hit the null termination  character `0`
```
char name[6] = { 'b','e', 'h', 'n', 'a','m' };
```

Declares a pointer whose data cannot be changed through the pointer:
```cpp
const int *p = &someInt;
```
or 
```cpp
int const *p;
```

   
Declares a pointer who cannot be changed to point to something else:
```cpp
int * const p = &someInt;
```


to make it easy to read remove the variable type,  then read it like:

`const int *p;`  ==> `const  *p ;` ==> `*p` is which is data is fixed.  
`int const *p;`  ==> `const  *p ;` ==> `*p` is which is data is fixed.  
`int * const p` ==>  `* const p` ==> `p` is fixed which is an address.  



## Difference between char * and char[]
Consider below two statements:

```cpp
char a1[] = "Behnam";
char *p1  = "Behnam";
```

1. `a1` is an array while `p1` is a pointer
2. `a` is stored at stack, but `p1` resides in code section of memory
3. `a1++` is invalid but `p1++` is valid.
4. `sizeof(a1)` will return `7` (six chars +`'\0'`) but `sizeof(p1)` while return `8` (pointer size)
5. `a1` and `&a1` are same but `p1` and `&p1`are not same.
6. `a1[1]='n'` is okay but `p1[1]='n'` will cause segmentation fault.

	
Refs: [1](https://cs50.stackexchange.com/questions/8899/difference-between-char-and-char-in-c), [2](https://www.reddit.com/r/C_Programming/comments/jjdl7s/difference_between_char_and_char/), [3](https://stackoverflow.com/questions/1641957/is-an-array-name-a-pointer)  
		
	
## size of string

1. `sizeof(string)` tells you the size of the pointer, so it should **NOT** be used.
2.  `strlen( "my string" )` could be used for c strings.

```cpp
char my_str[100]="my string";
std::cout<<"size of string is: "<<strlen( my_str )<<" bytes and string is: "<< my_str <<std::endl;
```

3. `str.size()` also return the size of c++ strings.

## strdup

In C++, the `strdup` function (from C) is used to duplicate a string by allocating memory and copying the content of an existing string to that new memory. You would need to use `strdup` when you want to create a copy of a string that you plan to manage manually (for instance, when working with raw pointers and dynamic memory allocation).

Here is an example of when you might use `strdup`:

### Example:
```cpp
#include <iostream>
#include <cstring>  // for strdup and free

int main() {
    // Original string
    const char* original = "Hello, World!";
    
    // Duplicate the string using strdup
    char* duplicate = strdup(original);

    // Print both strings
    std::cout << "Original: " << original << std::endl;
    std::cout << "Duplicate: " << duplicate << std::endl;

    // Free the memory allocated by strdup
    free(duplicate);

    return 0;
}
```

When to use `strdup`:

- **C-style strings**: If you're working with raw C-style strings (`char*`) and need to make a copy of the string that requires its own memory management.
- **Manual memory management**: When the duplicated string will be used independently and may need to be freed later.
  
Important notes:

1. `strdup` allocates memory using `malloc`, so you must free it with `free` when you're done using the duplicated string.
2. In C++, you generally don't need to use `strdup` if you are working with `std::string`, which manages memory automatically. Instead, use `std::string`'s copy constructor or assignment operator, which is safer and more idiomatic.

C++ alternative using `std::string`:

```cpp
#include <iostream>
#include <string>

int main() {
    // Original string
    std::string original = "Hello, World!";
    
    // Duplicate the string using std::string
    std::string duplicate = original;

    // Print both strings
    std::cout << "Original: " << original << std::endl;
    std::cout << "Duplicate: " << duplicate << std::endl;

    // No need to free memory, std::string handles it automatically
    return 0;
}
```

In modern C++ code, using `std::string` is preferable to avoid manual memory management.



## strstr

A standard C-style API for searching a substring within a string can be implemented using the `strstr` function, which is part of the C standard library (<string.h>). The `strstr` function searches for the first occurrence of a substring in a string and returns a pointer to the beginning of the substring if found. Otherwise, it returns `NULL`.

```cpp
    char haystack[] = "Hello, World!";  // Now this is modifiable
    const char *needle = "World";

    // Use strstr to find the first occurrence of the needle in the haystack
    char *result = strstr(haystack, needle);

    if (result) {
        printf("Found substring: %s\n", result);
    } else {
        printf("Substring not found.\n");
    }
```
	
## String copying


# C++ String
`std::string` usually allocates memory dynamically, and must copy the C-style string literal to it at run time.

```cpp
std::string str = "initializer syntax";
std::string str("converting constructor syntax");
std::string str = string("explicit constructor syntax");
std::string str{"uniform initializer syntax"};
```

## warning iso c++ forbids converting a string constant to ‘char*’ -wwrite-strings

```cpp
char* p1 = "John";
```
The problem is that string literals `"this is a string literal"` are of type `char ` in C but `const char ` in C++.
This will compile but will cause segmentation fault in run time:

```cpp
p1[0] = 'C';
```

However this will not compile:
```cpp
const char* p1 = "John";
p1[0] = 'C';
```
to read it remove the data type char, so we would have `const * p1`, which mean the place in the memory that `p1` is pointing is const and can not be changed.


Another solution is to change the literal from `const char *` to `char *`, which remove teh warning but it is not safe, as you can 
still do `p1[0] = 'C'`

```cpp
char * p1 = (char *)"John";
```
You can also use a string object instead 

```cpp
std::string p1 = "John";
```

## C++ string comparison 

`std::string::compare()` returns an int:

- equal to zero if `str1` and `str2` are equal,
- less than zero if `str1` is less than `str2`,
- greater than zero if `str1` is greater than `str2`.

```cpp
if (!str1.compare(str2)) 
{
    // 'str1' and 'str2' are equal.
}
```

`std::string::compare()` is most useful for quick sort and binary search algorithms. Natural sorts and dichotomic searches can be implemented with only `std::less`.


you can use `str1==str2` as well and and it is more readable and it will return the same value, but the compare function provides more information how the strings differ.




Refs: [1](https://stackoverflow.com/questions/9158894/differences-between-c-string-and-compare)

# String to Number Conversion
## int, float, double to string
```cpp
std::string strNumber=std::to_string(10.3);
```
# Number to String Conversion

## char to a string

```
char c = 'A';
std::string s(1, c);
```    
or

```
char c = 'A';
std::string s;

s.push_back(c);
```
or
```
std::stringstream ss;
ss << c;
ss >> s;                // or, use `s = ss.str()`
```
## int to string
```cpp
int i =std::stoi(strNumber.c_str());//10
```
## float to string
```cpp
float f=std::atof(strNumber.c_str());//10.3
float f=std::stof(strNumber);//10.3
```

## double to string
```cpp
double d =std::stod(strNumber.c_str() );//10.3
```

## string to vector of char
```cpp
std::vector<char> charVec(str.begin(),str.end() );
```

# Spiting String By Delimiter
```cpp
std::vector<std::string> spilitedString;
std::string s = "scott>=tiger>=mushroom";
std::string delimiter = ">=";

size_t pos = 0;
std::string token;
while ((pos = s.find(delimiter)) != std::string::npos)
    {
        token = s.substr(0, pos);
        spilitedString.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
```
## std::tolower, std::toupper 


`std::tolower()` returns an integer, not a `std::string`, and you cannot directly cast the result to a `std::string`. Instead, you should convert the result of `std::tolower()` to a `char`, and then construct a `std::string` from that character.
```cpp
 unsigned char c = 'A';
  char lower_c = static_cast<char>(std::tolower(c));

  std::string a(1, lower_c); // Create a string with one character
```


To correctly use `std::tolower` on a std::string, you need to iterate over each character of the string and apply `std::tolower` to it. Since `std::tolower` works on single characters (and returns an int), you should also cast the result back to char. Here's how you can do this:


```cpp
    std::string input = "Hello, World!";
    std::string result;

    // Use std::transform to apply std::tolower to each character
    std::transform(input.begin(), input.end(), std::back_inserter(result), 
                   [](unsigned char c){ return std::tolower(c); });

    std::cout << "Original: " << input << std::endl;
    std::cout << "Lowercase: " << result << std::endl;
```

- `std::transform` is used to apply the transformation (lowercasing in this case) to each character.
- `std::tolower` is applied to each character of the string. It takes an `unsigned char` as input, so we cast each character to `unsigned char` to avoid undefined behavior with non-ASCII characters.
- The result is accumulated in the `result` string using `std::back_inserter`.

This will output:
```
Original: Hello, World!
Lowercase: hello, world!
```


## std::isalnum

The function `std::isalnum` in C++ checks whether a given character is either an alphanumeric character, i.e., a letter (A-Z, a-z) or a digit (0-9).

- digits (0123456789)
- uppercase letters (ABCDEFGHIJKLMNOPQRSTUVWXYZ)
- lowercase letters (abcdefghijklmnopqrstuvwxyz)


Return Value:
- It returns a **non-zero value** (typically `true`) if the character is alphanumeric.
- It returns **0** (typically `false`) if the character is not alphanumeric.


```cpp
  char c = 'A';
  if (std::isalnum(c)) {
    std::cout << c << " is alphanumeric." << std::endl;
  } else {
    std::cout << c << " is not alphanumeric." << std::endl;
  }
```

In this case, if `c` is 'A' (an alphanumeric character), `std::isalnum(c)` will return a non-zero value, and the output will be:

```
A is alphanumeric.
```

# Find String Case Insensitive
```cpp
auto it = std::search(
		sentence.begin(), sentence.end(),
		word.begin(), word.end(),
		[](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
	);
	return (it != sentence.end());
```

# The small string optimization

Small size STL containers, would be set on stack instead of heap and after the size get bigger they would be allocated on heap, this is called **The Small String Optimization**. complete example [here](track_memory_allocations_overriding_new_operator.md)

[source code](../src/string.cpp)
