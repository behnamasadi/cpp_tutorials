#include <assert.h>
#include <type_traits>

/*
//http://blog.aaronballman.com/2011/11/a-simple-introduction-to-type-traits/

let say you want to write a piece of code that swap the bytes of integer type
int, long int, unsigned int, etc. One simpleimplemention wouldbe the following:
*/

template <typename T> T byte_swap(T value) {
  unsigned char *bytes = reinterpret_cast<unsigned char *>(&value);
  for (std::size_t i = 0; i < sizeof(T); i += 2) {
    // Take the value on the left and switch it
    // with the value on the right
    unsigned char v = bytes[i];
    bytes[i] = bytes[i + 1];
    bytes[i + 1] = v;
  }
  return value;
}
/*
But here we don't check the type so we don't know if user has passed a double of
integer, so one solution is template specialization. However, what happens if
the caller passes in a float? Or an unsigned char? Or a pointer? You can quickly
see how such a simple function would balloon out into many specializations, all
to protect the caller.
*/

template <> double byte_swap(double value) {
  assert(false && "Illegal to swap doubles");
  return value;
}

template <> char byte_swap(char value) {
  assert(false && "Illegal to swap chars");
  return value;
}
/*
A type trait is a way for you to get information about the types passed in as
template arguments, at compile time, so you can make more intelligent decisions.
The basics behind a type trait are:

1)You use a templated structure, usually named with the type trait you are
after. Eg) is_integer, is_pointer, is_void 2)The structure contains a static
const bool named value which defaults to a sensible state 3)You make
specializations of the structure representing the traits you want to expose, and
have those set their bool value to a sensible state You use a type trait by
querying its value, like: my_type_trait<T>::value
*/

template <typename T> struct is_swapable { static const bool value = false; };

template <> struct is_swapable<unsigned short> {
  static const bool value = true;
};

template <> struct is_swapable<short> { static const bool value = true; };

template <> struct is_swapable<unsigned long> {
  static const bool value = true;
};

template <> struct is_swapable<long> { static const bool value = true; };

template <> struct is_swapable<unsigned long long> {
  static const bool value = true;
};

template <> struct is_swapable<long long> { static const bool value = true; };

/*
Another example to check if type is pointer:

static_assert(is_pointer<int >::value == false, "");
static_assert(is_pointer<int*>::value == true,  "");
*/
template <typename T> struct is_pointer { static constexpr bool value{false}; };

template <typename T> struct is_pointer<T *> {
  static constexpr bool value{true};
};

/*
In C++11, there is now a standard STL header called type_traits, and it contains
traits for almost everything you can think of. There are traits to tell you the
basic datatypes, whether something is a pointer, whether something is an array,
whether something is const, etc. There are even traits to remove traits, like
turn a pointer type into a value type, or a const type into a non-const type.
Most of the time you are able to make use of these built-in type traits for your
own code. Also, as part of C++11 is the new static_assert functionality, which
uses compile-time constant expressions as a way to generate errors. So, for
instance, our byte_swap assert code could look like this in C++11:

static_assert( std::is_integral< T >::value && sizeof( T ) >= 2, "Cannot swap
values of this type" );
*/

int main() {

  static_assert(is_pointer<int>::value == false, "");
  static_assert(is_pointer<int *>::value == true, "");

  typedef int T;
  static_assert(std::is_integral<T>::value && sizeof(T) >= 2,
                "Cannot swap values of this type");
}
