## Understanding `std::greater` and `std::less`

**`std::greater`** and **`std::less`** are function objects (functors) in C++ that define comparison operations. They are commonly used with algorithms like `sort`, `max_element`, `min_element`, and others to specify the comparison criteria.

### Simple Example

```cpp
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::vector<int> numbers = {3, 7, 4, 9, 2, 5, 8, 1, 6};

    // Using std::less (default behavior for many algorithms)
    std::sort(numbers.begin(), numbers.end());
    std::cout << "Sorted in ascending order: ";
    for (int num : numbers) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    // Using std::greater for descending order
    std::sort(numbers.begin(), numbers.end(), std::greater<int>());
    std::cout << "Sorted in descending order: ";
    for (int num : numbers) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    return 0;
}
```

### Explanation

* **`std::less<int>()`**: This is the default comparator for many algorithms. It defines the "less than" operation for integers.
* **`std::greater<int>()`**: This defines the "greater than" operation for integers.

* **`std::greater{}`**: This is equivalent to `std::greater<>()`. It's just a different syntax for creating a transparent comparator.


### Advance Example
```cpp
struct Person {

  std::string name;
  int age;
};

bool operator<(const Person &lhs, const Person &rhs) {
  return lhs.age < rhs.age;
}

bool operator>(const Person &lhs, const Person &rhs) {
  return lhs.age > rhs.age;
}

  std::vector<Person> people = {{"Alice", 30}, {"Bob", 25}, {"Charlie", 35}};

  std::sort(people.begin(), people.end(), std::less<Person>());
  std::sort(people.begin(), people.end(), std::greate<Person>());

  for (const auto &person : people) {
    std::cout << person.name << ": " << person.age << std::endl;
  }

```




