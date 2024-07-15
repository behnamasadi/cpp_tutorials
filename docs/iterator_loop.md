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

