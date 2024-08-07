# Iterator
An iterator is an object (like a pointer) that points to an element inside the container. A pointer is the most obvious type of iterator. A pointer can point to elements in an array and use the increment operator (++) to cycle through them. However, not all iterators have the same capability as pointers.


Types of iterators:

```
    Input Iterator           Output Iterator
            ^                     ^ 
             \                   /
              \                 /
               \               /
                \             /
                 \           /
                Forward Iterator
                       ^
                       |
                       |
                       |                                              

             Bidirectional Iterator
                       ^
                       |
                       |
                       |                                              

             Random Access Iterator
```

## Input Iterator
i.e. `std::find`, `std::equal`, `std::count`


## Output Iterator
i.e. `std::move`

## Forward Iterator
It permits values to be both accessed and modified.
`std::search`, `std::search_n`, `std::lower_bound`, `std::replace`

## Bidirectional Iterator
`list`, `map`, `multimap`, `set`, `std::random_shuffle` and `multiset`

```cpp
list<int>v1 = {1, 2, 3, 4, 5};
 
list<int>::iterator i1;

for (i1=v1.begin();i1!=v1.end();++i1){}
```

## Random Access Iterators
`std::nth_element`, `std::sort`,

Refs: [1](https://www.geeksforgeeks.org/introduction-iterators-c/)

## range based for-loops

Range-for is as fast as possible since it caches the end iterator, uses pre-increment and only dereferences the iterator once.
```cpp
for ( for-range-declaration : expression ) statement
```


Choose `auto const &x` when you want to work with original items and will not modify them.
```cpp
for (auto const &x : vecStudent);
```

Choose `auto &x` when you want to work with original items and may modify them.
```cpp
for (auto &x : vecStudent);
```

Choose `auto x` when you want to work with copies.
```cpp
for (auto x : vecStudent);
```

## regular for loops with iterators
`begin()` vs `cbegin()`

`begin()` returns an iterator to beginning while `cbegin()` returns a `const_iterator` to beginning.

The basic difference between these two is iterator (i.e begin()) lets you change the value of the object it is pointing to and const_iterator will not let you change the value of the object.

```cpp
for (auto a = vecStudent.cbegin(); a != vecStudent.cend(); ++a)
```

## for_each
```cpp
std::for_each(vecStudent.begin(), vecStudent.end(), [](Student &n){ n; });
```
Refs: [1](http://www.cplusplus.com/reference/iterator/), [2](https://en.cppreference.com/w/cpp/language/range-for), [3](https://en.wikipedia.org/wiki/Loop_optimization)
[source code](../src/iterator_loop.cpp)



## std::next

```cpp
  std::vector<int> v{4, 5, 6, 1, 3};

  auto it = v.begin();
  auto nx = std::next(it, 3);
  std::cout << *it << ' ' << *nx << '\n';

  std::cout << std::distance(it, nx) << '\n';

  std::advance(it, 3);
  std::cout << *it << '\n';

  it = v.end();
  nx = std::next(it, -2);
  std::cout << ' ' << *nx << '\n';
```


## Creating an iterator
Creating an iterator in C++ can be achieved in several ways depending on the type of container and the level of customization you need. Here are a few common methods to create and use iterators in C++:

### Using Standard Library Containers

For standard library containers such as `std::vector`, `std::list`, `std::map`, etc., iterators are already defined, and you can use them directly.

```cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int> vec = {1, 2, 3, 4, 5};

    // Using iterator
    for (std::vector<int>::iterator it = vec.begin(); it != vec.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    // Using const_iterator
    for (std::vector<int>::const_iterator it = vec.cbegin(); it != vec.cend(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    return 0;
}
```

### Custom Container with Custom Iterator

If you need a custom iterator for your own container class, you can define it by creating a nested class within your container class or a separate iterator class. Here's a simple example of a custom container and iterator:

```cpp
#include <iostream>

template <typename T>
class CustomContainer {
public:
    CustomContainer(size_t size) : size(size), data(new T[size]) {}

    ~CustomContainer() { delete[] data; }

    T& operator[](size_t index) { return data[index]; }
    const T& operator[](size_t index) const { return data[index]; }

    class Iterator {
    public:
        Iterator(T* ptr) : ptr(ptr) {}

        Iterator& operator++() {
            ++ptr;
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return ptr != other.ptr;
        }

        T& operator*() const {
            return *ptr;
        }

    private:
        T* ptr;
    };

    Iterator begin() { return Iterator(data); }
    Iterator end() { return Iterator(data + size); }

private:
    size_t size;
    T* data;
};

int main() {
    CustomContainer<int> container(5);
    for (size_t i = 0; i < 5; ++i) {
        container[i] = i + 1;
    }

    for (CustomContainer<int>::Iterator it = container.begin(); it != container.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    return 0;
}
```

### Explanation

1. **Standard Library Containers**:
   - Standard library containers like `std::vector` come with predefined iterators.
   - You can declare an iterator using the container's `iterator` or `const_iterator` types.
   - The `begin()` and `end()` member functions return iterators to the beginning and end of the container, respectively.

2. **Custom Container with Custom Iterator**:
   - Create a custom container class and a nested iterator class within it.
   - The iterator class should support the necessary operations (`operator++`, `operator!=`, and `operator*` in this case).
   - The container class should have `begin()` and `end()` functions returning iterators pointing to the start and end of the container.

This is a basic introduction to iterators in C++. Depending on your needs, you might need to implement additional functionalities like bidirectional iteration, random access, or const correctness.
