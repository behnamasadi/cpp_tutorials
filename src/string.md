# ASCII 
ASCII uses 7 bits to represent a character. By using 7 bits, we can have a maximum of 2^7 (= 128)
distinct combinations. Which means that we can represent 128 characters maximum.
The last bit (8th) is used for avoiding errors as parity bit. Most ASCII characters are printable
characters of the alphabet such as abc, ABC, 123, ?&!, etc. The others are control
characters such as carriage return, line feed, tab, etc. ASCII was meant for English only.

# ANSI
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

# ASCII Extended (extended ASCII" or "8-bit ASCII)
Some clever people started using the 8th bit (the bit used for parity) to encode more characters to support
their language (to support `é`, in French, for example). Just using one extra bit doubled the size of the
original ASCII table to map up to 256 characters (2^8 = 256 characters).

# Unicode
ASCII Extended solves the problem for languages that are based on the Latin alphabet but not other languages
that are completely different (Greek, Russian, Chinese) We would have needed an entirely new character set.
Unicode doesn't contain every character from every language.
[unicode-table](https://unicode-table.com/)


You need to "encode" this abstract representation. That's where an [encoding](https://en.wikipedia.org/wiki/Character_encoding) comes into play
Character encoding: is used to map every character to series of bits, number  or electrical pulse
(Morse code, ASCII, Unicode)

# Encodings: UTF-8 vs UTF-16 vs UTF-32
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
the _TEXT macro will map to L"..." instead of "...".

UNICODE is used by Windows headers, whereas _UNICODE is used by C-runtime/MFC headers.

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


# String Data Types
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

# String to Number Conversion
```cpp
std::string strNumber=std::to_string(10.3);
float f=std::atof(strNumber.c_str());//10.3
int i =std::stoi(strNumber.c_str());//10
double d =std::stod(strNumber.c_str() );//10.3
```
# C Style String

# string_view

# Short String Optimization

# Spiting String By Delimiter

# Find String Case Insensitive

[source code](string.cpp)
