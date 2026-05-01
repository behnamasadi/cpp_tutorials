# Strings in C and C++

- [String encoding](#string-encoding)
  - [ASCII](#ascii)
  - [ANSI](#ansi)
  - [Extended ASCII (8-bit ASCII)](#extended-ascii-8-bit-ascii)
  - [Unicode](#unicode)
  - [Encodings: UTF-8 vs UTF-16 vs UTF-32](#encodings-utf-8-vs-utf-16-vs-utf-32)
- [Strings in Windows](#strings-in-windows)
  - [8-bit AnsiStrings](#8-bit-ansistrings)
  - [16-bit UnicodeStrings](#16-bit-unicodestrings)
  - [TCHAR (depends on UNICODE)](#tchar-depends-on-unicode)
  - [Checklist for Windows string programming](#checklist-for-windows-string-programming)
- [Literals and string data types](#literals-and-string-data-types)
- [C strings](#c-strings)
  - [char\* vs char array](#char-vs-char-array)
  - [Size of a string](#size-of-a-string)
  - [strdup](#strdup)
  - [strstr — substring search](#strstr--substring-search)
- [std::string](#stdstring)
  - [The -Wwrite-strings warning](#the--wwrite-strings-warning)
  - [String comparison](#string-comparison)
- [String / number conversions](#string--number-conversions)
  - [String → number](#string--number)
  - [Number → string](#number--string)
  - [char → string](#char--string)
  - [string → vector\<char\>](#string--vectorchar)
- [Splitting a string by a delimiter](#splitting-a-string-by-a-delimiter)
- [Character classification: tolower, toupper, isalnum](#character-classification-tolower-toupper-isalnum)
- [std::string\_view](#stdstring_view)
- [Case-insensitive find](#case-insensitive-find)
- [Small-string optimization (SSO)](#small-string-optimization-sso)

---

# String encoding

## ASCII

ASCII uses 7 bits to represent a character, giving `2^7 = 128` distinct codes. The 8th bit was historically used as a parity bit. Most ASCII codes are printable (`abc`, `ABC`, `123`, `?&!`); the rest are control characters (carriage return, line feed, tab). ASCII was designed for English only.

## ANSI

ANSI extends ASCII to 8 bits (256 codes) and uses **code pages** to assign different meanings to the upper 128 codes for different languages — Japanese, Chinese, Western European, etc. The application reading the file has to know which code page is in use to interpret the bytes correctly.

There are many 8-bit "extended ASCII" variants. **ISO 8859-1** (Latin-1) is one common one — original ASCII (0–127) plus Western European letters in 160–255, the default for most early browsers. Windows uses a closely related encoding called **Windows-1252**, often loosely labeled "ANSI" — it differs from ISO 8859-1 by using 128–159 for printable characters instead of controls.

Refs: [excelanytime.com](https://www.excelanytime.com/excel/index.php?option=com_content&view=article&id=116:ascii-code-extended-ascii-characters-8-bit-system-and-ansi-code&catid=78&Itemid=474)

## Extended ASCII (8-bit ASCII)

Once people started using the 8th bit (originally parity) for additional characters, the table doubled to 256 entries (`2^8 = 256`). This was enough for languages based on the Latin alphabet (e.g. French `é`) but not for Greek, Cyrillic, or CJK scripts.

## Unicode

Unicode assigns a unique **code point** (an abstract integer) to every character in essentially every script — currently over 140k assigned. A code point is *not* a byte sequence; you still need an **encoding** to map code points to bytes. See the [unicode table](https://unicode-table.com/) for the full list.

A character encoding maps each character (or code point) to a sequence of bits or bytes — Morse code, ASCII, UTF-8, UTF-16, etc. are all encodings. See [Wikipedia: character encoding](https://en.wikipedia.org/wiki/Character_encoding).

## Encodings: UTF-8 vs UTF-16 vs UTF-32

| Encoding | Code unit | Per code point | Notes |
| --- | --- | --- | --- |
| UTF-8  | 8 bits  | 1–4 bytes | ASCII-compatible, dominant on the web |
| UTF-16 | 16 bits | 2 or 4 bytes (surrogates) | Used by Windows APIs, Java, JavaScript |
| UTF-32 | 32 bits | always 4 bytes | Fixed-width, simple but space-hungry |

UTF-8 and UTF-16 are **variable-length**; UTF-32 is fixed-length. Refs: [Comparison of Unicode encodings](https://en.wikipedia.org/wiki/Comparison_of_Unicode_encodings).

# Strings in Windows

## 8-bit AnsiStrings

- `char` — 8-bit character, the C/C++ data type
- `CHAR` — Windows alias for `char`
- `LPSTR` — null-terminated string of `CHAR` (Long Pointer to STRing)
- `LPCSTR` — null-terminated **const** string of `CHAR`

`char` is supposed to hold a one-byte character; `wchar_t` holds a "wide character," but its size is implementation-defined: **2 bytes on Windows, 4 bytes on Linux**. Neither `char` nor `wchar_t` is directly tied to Unicode by the language — Windows just *uses* UTF-16 in its 16-bit APIs.

## 16-bit UnicodeStrings

- `wchar_t` — 16-bit character on Windows
- `WCHAR` — alias for `wchar_t`
- `LPWSTR` — null-terminated string of `WCHAR`
- `LPCWSTR` — null-terminated **const** string of `WCHAR`

## TCHAR (depends on UNICODE)

- `TCHAR` — `WCHAR` if `UNICODE` is defined, else `CHAR`
- `LPTSTR` — null-terminated string of `TCHAR`
- `LPCTSTR` — null-terminated **const** string of `TCHAR`

| Item            | 8-bit   | 16-bit   | Varies   |
|-----------------|---------|----------|----------|
| character       | CHAR    | WCHAR    | TCHAR    |
| string          | LPSTR   | LPWSTR   | LPTSTR   |
| string (const)  | LPCSTR  | LPCWSTR  | LPCTSTR  |

`LPCTSTR` = **L**ong **P**ointer to a **C**onst **T**CHAR **STR**ing. The "long" prefix is a 16-bit Windows leftover; today it's just a pointer.

```cpp
LPSTR    = char*
LPCSTR   = const char*
LPWSTR   = wchar_t*
LPCWSTR  = const wchar_t*
LPTSTR   = char* or wchar_t*           // depends on _UNICODE
LPCTSTR  = const char* or const wchar_t*
```

The `LPCTSTR` typedef from `WinNT.h`:

```cpp
#ifdef UNICODE
typedef LPCWSTR LPCTSTR;
#else
typedef LPCSTR  LPCTSTR;
#endif
```

`std::string` is `std::basic_string<char>`; `std::wstring` is `std::basic_string<wchar_t>`. There are also fixed-width Unicode variants:

| Type                    | Underlying                       |
|-------------------------|----------------------------------|
| `std::string`           | `std::basic_string<char>`        |
| `std::wstring`          | `std::basic_string<wchar_t>`     |
| `std::u8string`  (C++20)| `std::basic_string<char8_t>`     |
| `std::u16string` (C++11)| `std::basic_string<char16_t>`    |
| `std::u32string` (C++11)| `std::basic_string<char32_t>`    |

Refs: [SO: T and L meaning in C++](https://stackoverflow.com/questions/28917431/what-do-t-and-l-mean-in-c-and-how-can-i-pass-them).

## Checklist for Windows string programming

1. **`_T()` / `TEXT()`** are macros that prepend `L` to a string literal if `_UNICODE` / `UNICODE` is defined. They date from when you had to support Windows 9x (no Unicode) and Windows NT (Unicode) from one codebase. All modern Windows is Unicode-based, so new code rarely needs them.
   - `UNICODE` controls Win32 headers: defining it makes `GetWindowText` resolve to `GetWindowTextW` and `TEXT(...)` produce `L"..."`.
   - `_UNICODE` controls the C runtime headers: defining it makes `_tcslen` resolve to `wcslen` and `_TEXT(...)` produce `L"..."`.
2. Use `_TCHAR` / `_T()` with C-runtime functions; use `TCHAR` / `TEXT()` with the Win32 API. `CString` (MFC) is `TCHAR`-based, so use `TEXT()`.
3. Prefer `LPTSTR` and `LPCTSTR` over `char*` and `const char*` in Windows code.
   - `LPCSTR` — pointer to a const `char` string
   - `LPCTSTR` — pointer to a const `TCHAR` string (wide or narrow depending on `UNICODE`)
   - `LPTSTR` — pointer to a non-const `TCHAR` string
4. For C++ strings, prefer `std::wstring` over `std::string` when interoperating with the Win32 W APIs.

# Literals and string data types

A **string literal** is the source-code form of a string value: in `x = "foo"`, `"foo"` is the literal. In C the type is `char[N]`; in C++ it's `const char[N]` (the array decays to `const char*`).

```cpp
auto c0 = 'A';   // char
auto c1 = u8'A'; // char (C++17), char8_t (C++20+)
auto c2 = L'A';  // wchar_t
auto c3 = u'A';  // char16_t
auto c4 = U'A';  // char32_t

// Multicharacter literal — int, value is implementation-defined.
auto m0 = 'abcd';

// String literals
auto s0 = "hello";   // const char[6]      → decays to const char*
auto s1 = u8"hello"; // const char[6] (C++17), const char8_t[6] (C++20+); UTF-8
auto s2 = L"hello";  // const wchar_t[6]
auto s3 = u"hello";  // const char16_t[6]; UTF-16
auto s4 = U"hello";  // const char32_t[6]; UTF-32

const char* multiline = R"(line1
line2
line3)";
```

# C strings

A C string is a one-dimensional array of characters terminated by `'\0'`. The trailing null is what lets `printf`, `std::cout`, `strlen`, etc. find the end.

Stack-allocated, fixed length 10 with explicit `'\0'`:

```cpp
char name[10] = { 'b','e','h','n','a','m','\0' };
// Compiler zero-fills the rest:
// char name[10] = {'b','e','h','n','a','m','\0','\0','\0','\0'};
```

Stack-allocated, size deduced:

```cpp
char name[] = { 'b','e','h','n','a','m','\0' };
// equivalent to:
// char name[7] = {'b','e','h','n','a','m','\0'};
```

Same thing using a string literal — the `'\0'` is implicit:

```cpp
char name[] = "behnam";
// equivalent to:
// char name[7] = "behnam";
```

The `char*`-to-literal form puts the data in **read-only memory** (typically the `.rodata` section, *not* the code/text section). This form is discouraged in modern code:

```cpp
char* name = "behnam"; // deprecated in C++; warns or errors with -Wwrite-strings
```

Better:

```cpp
const char* name = "behnam";
```

This compiles but **segfaults at runtime** — `name` points into read-only memory:

```cpp
name[0] = 'C'; // crash
```

This is fine — you're changing what `name` points at, not the literal:

```cpp
name = "Margarethe";
```

Printing works because the literal is null-terminated:

```cpp
std::cout << "name: " << name << '\n';
```

Without a terminator, printing reads past the end until it hits a stray zero — undefined behavior:

```cpp
char name[6] = { 'b','e','h','n','a','m' }; // no '\0' — DON'T print this
```

## Aside: `const` placement on pointers

Reading right-to-left and ignoring the type makes this easy:

```cpp
const int* p = &x;   // *p is const  → pointed-to int can't be modified
int const* p = &x;   // same thing
int* const p = &x;   // p is const   → pointer can't be reseated
const int* const p;  // both
```

## char\* vs char array

```cpp
char  a1[] = "Behnam";
char* p1   = "Behnam"; // const char* in modern C++
```

| Aspect | `a1` (array) | `p1` (pointer) |
| --- | --- | --- |
| Storage  | stack          | points into `.rodata` |
| `a1++` / `p1++` | invalid | valid |
| `sizeof` | 7 (6 chars + `'\0'`) | size of a pointer (8 on 64-bit) |
| `&a1` vs `a1` | same address | different (address of pointer vs value) |
| Assignment | `a1[1]='n'` OK | `p1[1]='n'` segfaults |

Refs: [cs50](https://cs50.stackexchange.com/questions/8899/difference-between-char-and-char-in-c), [r/C\_Programming](https://www.reddit.com/r/C_Programming/comments/jjdl7s/difference_between_char_and_char/), [SO: array name as pointer](https://stackoverflow.com/questions/1641957/is-an-array-name-a-pointer).

## Size of a string

1. `sizeof(s)` on a `std::string` gives the size of the string *object* (typically 24–32 bytes), **not** the number of characters. Don't use it for string length.
2. `strlen(p)` works on null-terminated C strings — counts bytes up to (but not including) `'\0'`.
3. `s.size()` (or `s.length()`) gives the character count of a `std::string`.

```cpp
char my_str[100] = "my string";
std::cout << "length: " << strlen(my_str)
          << ", contents: " << my_str << '\n';

std::string s = "my string";
std::cout << "length: " << s.size() << '\n';
```

## strdup

`strdup` (from C, declared in `<cstring>`) allocates new memory with `malloc` and copies a null-terminated string into it. You must `free` the result.

```cpp
#include <cstring>
#include <cstdlib>

const char* original = "Hello, World!";
char* copy = std::strdup(original);
// ... use copy ...
std::free(copy);
```

In C++, prefer `std::string` — copies and frees automatically:

```cpp
std::string original = "Hello, World!";
std::string copy = original;
```

## strstr — substring search

`strstr(haystack, needle)` returns a pointer to the first occurrence of `needle` in `haystack`, or `nullptr` if not found.

```cpp
#include <cstring>
#include <cstdio>

char haystack[] = "Hello, World!";
const char* needle = "World";

if (char* hit = std::strstr(haystack, needle)) {
    std::printf("found at offset %ld: %s\n", hit - haystack, hit);
} else {
    std::printf("not found\n");
}
```

The C++ equivalent is `std::string::find`, which returns `std::string::npos` on miss.

# std::string

`std::string` allocates its buffer on the heap (with the small-string optimization handling short strings on the stack — see [SSO](#small-string-optimization-sso)) and copies the literal into it at construction.

```cpp
std::string s1 = "initializer syntax";
std::string s2("converting constructor");
std::string s3 = std::string("explicit constructor");
std::string s4{"uniform initialization"};
```

## The -Wwrite-strings warning

```cpp
char* p = "John"; // warning: ISO C++ forbids converting a string constant to 'char*'
```

String literals are `char[]` in C but `const char[]` in C++. Assigning to a non-const pointer is allowed for backward-compatibility but warns under `-Wwrite-strings`. The literal still lives in read-only memory, so:

```cpp
p[0] = 'C'; // compiles, segfaults at runtime
```

The fix is to make the pointer `const`:

```cpp
const char* p = "John";
p[0] = 'C'; // now a compile error — caught at compile time, as it should be
```

A C-style cast silences the warning but doesn't fix the bug — the memory is still read-only:

```cpp
char* p = (char*)"John"; // compiles, still segfaults on p[0] = 'C'
```

The right fix in C++ is to use `std::string`:

```cpp
std::string p = "John"; // mutable, owns its memory
p[0] = 'C';             // fine
```

## String comparison

`std::string::compare` returns an `int`:

- `0` if equal
- `< 0` if `*this` is lexicographically less
- `> 0` if `*this` is greater

```cpp
if (str1.compare(str2) == 0) { /* equal */ }
```

For plain equality, just use `==` — it's clearer and equivalent:

```cpp
if (str1 == str2) { /* equal */ }
```

`compare` is useful when you need the three-way result (sort, binary search, custom orderings). For just-greater/just-less checks, `<`, `<=`, `>`, `>=` operators work too.

Refs: [SO: == vs compare](https://stackoverflow.com/questions/9158894/differences-between-c-string-and-compare).

# String / number conversions

## String → number

```cpp
std::string s = "10.3";

int    i = std::stoi(s);  // 10
long   l = std::stol(s);  // 10
float  f = std::stof(s);  // 10.3f
double d = std::stod(s);  // 10.3
```

Note: `std::stoi`, `std::stof`, `std::stod` take a `std::string` (or `std::wstring`), **not** a `const char*`. For C strings, use the C-style `std::atoi`, `std::atof` (return `int` and `double` respectively), or pass `s.c_str()` only if you wrap it back into a `std::string`.

`std::stoi` throws `std::invalid_argument` if no conversion is possible and `std::out_of_range` if the value doesn't fit. C-style `atoi` returns `0` silently on failure.

## Number → string

```cpp
std::string a = std::to_string(42);     // "42"
std::string b = std::to_string(10.3);   // "10.300000" (C locale, fixed precision)
```

For controlled formatting use `std::format` (C++20) or a `std::ostringstream`:

```cpp
#include <format>   // C++20
auto s = std::format("{:.2f}", 10.3);   // "10.30"
```

```cpp
std::ostringstream oss;
oss << std::fixed << std::setprecision(2) << 10.3;
std::string s = oss.str(); // "10.30"
```

## char → string

```cpp
char c = 'A';

std::string s1(1, c);     // construct: 1 copy of c
std::string s2;
s2.push_back(c);          // append
std::string s3 = std::string{c};

std::stringstream ss;
ss << c;
std::string s4 = ss.str();
```

## string → vector\<char\>

```cpp
std::string s = "hello";
std::vector<char> v(s.begin(), s.end());
```

# Splitting a string by a delimiter

Manual loop using `find` / `substr`:

```cpp
std::string s = "scott>=tiger>=mushroom";
const std::string delim = ">=";

std::vector<std::string> tokens;
size_t pos = 0;
while ((pos = s.find(delim)) != std::string::npos) {
    tokens.push_back(s.substr(0, pos));
    s.erase(0, pos + delim.size());
}
tokens.push_back(s); // final piece after the last delimiter
```

C++20 alternative using ranges:

```cpp
#include <ranges>
#include <string_view>

std::string s = "scott>=tiger>=mushroom";
for (auto part : std::views::split(s, std::string_view{">="})) {
    std::string_view sv{part.begin(), part.end()};
    // use sv
}
```

# Character classification: tolower, toupper, isalnum

`<cctype>` functions take an `int` whose value must be representable as `unsigned char` (or `EOF`). Passing a plain `char` directly is **undefined behavior** for non-ASCII bytes on platforms where `char` is signed — always cast to `unsigned char` first.

```cpp
unsigned char c = 'A';
char lower = static_cast<char>(std::tolower(c));
std::string s(1, lower);
```

To lowercase an entire string, transform each character:

```cpp
#include <algorithm>
#include <cctype>

std::string in = "Hello, World!";
std::string out;
out.reserve(in.size());
std::transform(in.begin(), in.end(), std::back_inserter(out),
               [](unsigned char ch){ return std::tolower(ch); });
// out == "hello, world!"
```

`std::isalnum(c)` returns non-zero if `c` is a letter or digit (`A-Z`, `a-z`, `0-9` in the C locale):

```cpp
unsigned char c = 'A';
if (std::isalnum(c)) { /* letter or digit */ }
```

Other useful predicates: `isalpha`, `isdigit`, `isspace`, `ispunct`, `isupper`, `islower`.

# std::string\_view

`std::string_view` (C++17, `<string_view>`) is a non-owning reference to a contiguous character sequence: a pointer + a length, no allocation, no null-termination guarantee. Use it for read-only string parameters to accept any of `const char*`, `std::string`, `std::string_view`, or substrings without copying.

```cpp
#include <string_view>

void log(std::string_view msg) {       // accepts std::string, const char*, literal, etc.
    std::cout << msg << '\n';
}

std::string_view sv = "hello world";
auto first = sv.substr(0, 5);          // "hello" — no allocation, just a new view
```

**Lifetime caveat**: a `string_view` doesn't own its data. Outliving the underlying buffer (e.g. taking a view into a temporary `std::string`) is a use-after-free.

```cpp
std::string_view bad = std::string{"oops"}; // dangles after the temporary dies
```

# Case-insensitive find

```cpp
#include <algorithm>
#include <cctype>

auto it = std::search(
    sentence.begin(), sentence.end(),
    word.begin(),     word.end(),
    [](unsigned char a, unsigned char b){ return std::toupper(a) == std::toupper(b); });

bool found = (it != sentence.end());
```

# Small-string optimization (SSO)

Most `std::string` implementations store short strings (typically up to 15–22 characters, depending on the standard library) **inline inside the string object** rather than allocating a heap buffer. Once the content grows past that threshold, the implementation switches to a heap allocation. This is the **small-string optimization (SSO)** and it dramatically reduces allocations for the common case of short strings.

You can observe the switch by overriding `operator new` — see the worked example in [track\_memory\_allocations\_overriding\_new\_operator.md](track_memory_allocations_overriding_new_operator.md).

[source code](../src/string.cpp)
