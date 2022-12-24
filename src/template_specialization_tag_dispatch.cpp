// https://www.youtube.com/watch?v=Vkck4EU2lOU
#include <iostream>
namespace template_specialization {
template <typename T> bool equal(T rhs, T lhs) {
  std::cout << "general template called" << std::endl;
  return rhs == lhs;
}

bool equal(float rhs, float lhs) {
  std::cout << "specialized template called" << std::endl;
  return rhs == lhs;
}

} // namespace template_specialization

#include <type_traits>
namespace tag_dispatching {

template <typename T>
bool equal(T rhs, T lhs, std::true_type type) // flot type
{
  std::cout << "true_type called" << std::endl;
  return rhs == lhs;
}

template <typename T>
bool equal(T rhs, T lhs, std::false_type type) // non float
{
  std::cout << "false_type called" << std::endl;
  return rhs == lhs;
}

template <typename T> bool equal(T lhs, T rhs) {
  return equal(lhs, rhs,
               std::conditional_t<std::is_floating_point<T>::value,
                                  std::true_type, std::false_type>{});

  // return equal( lhs, rhs ,typename
  // std::conditional<std::is_floating_point<T>::value,std::true_type,std::false_type
  // >::type{} );
}
} // namespace tag_dispatching

// SFINAE: Substitution failure is not an error
namespace SFINAE {
// you can put it in here:
/*
template <typename T, typename= std::enable_if_t<!
std::is_floating_point<T>::value >> bool equal(T lhs, T rhs   )
{
    std::cout<<"non float type called" <<std::endl;
    return rhs==lhs;
}
*/

// you can put it as function argument
template <typename T>
bool equal(T lhs, T rhs,
           std::enable_if_t<!std::is_floating_point<T>::value> * = nullptr) {
  std::cout << "SFINAE non float type called" << std::endl;
  return rhs == lhs;
}

template <typename T>
bool equal(T lhs, T rhs,
           std::enable_if_t<std::is_floating_point<T>::value> * = nullptr) {
  std::cout << "SFINAE float type called" << std::endl;
  return rhs == lhs;
}

// or you can put it as return value
template <typename T>
std::enable_if_t<std::is_floating_point<T>::value, bool> Equal(T lhs, T rhs) {
  std::cout << "SFINAE float type called" << std::endl;
  return rhs == lhs;
}

template <typename T>
std::enable_if_t<!std::is_floating_point<T>::value, bool> Equal(T lhs, T rhs) {
  std::cout << "SFINAE non float type called" << std::endl;
  return rhs == lhs;
}

} // namespace SFINAE

int main() {

  // tag_dispatching
  {
    std::cout << template_specialization::equal(1, 1) << std::endl;
    std::cout << template_specialization::equal(1.f, 1.f) << std::endl;
  }

  // tag_dispatching
  {
    std::cout << tag_dispatching::equal(1, 1) << std::endl;
    std::cout << tag_dispatching::equal(1.f, 1.f) << std::endl;
  }

  // SFINAE
  {
    std::cout << SFINAE::equal(1, 1) << std::endl;
    std::cout << SFINAE::equal(1.f, 1.f) << std::endl;

    std::cout << SFINAE::Equal(1, 1) << std::endl;
    std::cout << SFINAE::Equal(1.f, 1.f) << std::endl;
  }
}
