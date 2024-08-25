# Operators Overloading

Any of the following 38 (until C++20)40 (since C++20) operators:
```
+ - * / % ^ & |
~ ! = < > += -=
*= /= %= ^= &= |= << >> >>= <<= == != <= >= <=> (since C++20) && || ++ -- , ->*
-> ( ) [ ]
 co_await (since C++20)
```

In this example we gonna overload the following operators:
```cpp
()
-
<<
>>
= deep copy (Copy constructor, Assignment operator)
```

Our class money:

```cpp
class money {
private:
  int m_size = 10;

public:
  int m_value;
  int *data;

  money(int value = 10) : m_value(value) {
    std::cout << "Constructor money with m_value: " << m_value << std::endl;

    data = new int[m_size];

    int upperBound = 20;
    int lowerBound = 0;
    int numberOfRandomNumbers = m_size;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(lowerBound, upperBound);

    for (int n = 0; n < numberOfRandomNumbers; ++n)
      data[n] = dis(gen);
  }

  ~money() {
    std::cout << "Destructor money with value: " << m_value << std::endl;
    delete[] data;
  }

  double operator()() { return m_value; }

  friend money operator-(const money &t1, int m_value);
  friend std::ostream &operator<<(std::ostream &os, const money &t);
  friend std::istream &operator>>(std::istream &is, const money &t);

  bool operator<(const money &other) const { return m_value < other.m_value; }

  money operator-(const money &other) {
    m_value - other.m_value;
    return *this;
  }

  // Copy constructor for deep copy
  money(const money &other) : m_value(other.m_value), m_size(other.m_size) {
    data = new int[m_size];
    std::copy(other.data, other.data + m_size, data);
  }

  // Copy assignment operator for deep copy
  money &operator=(const money &other) {
    if (this != &other) {
      delete[] data;

      m_value = other.m_value;
      m_size = other.m_size;
      data = new int[m_size];
      std::copy(other.data, other.data + m_size, data);
    }
    return *this;
  }
};
```


### () operator overloading

```cpp
  double operator()() { return value; }
```
  
  
### < operator overloading

```cpp
  bool operator<(const money &other) const { return value < other.value; }

  money operator-(const money &other) {
    value - other.value;
    return *this;
  }
```

### Implementations of the < operator inside or outside of the class

inside of the class:
```cpp
struct cell {
  int index;
  float cost;
  bool operator<(const cell &otherside) { return cost < otherside.cost; }
};
```

ouside of the class:

```cpp
bool operator<(const cell &lhs, const cell &rhs) { return lhs.cost < rhs.cost; }
```


### - operator overloading

```cpp
friend money operator-(const money &t1, int value);
money operator-(const money &t1, int value) { return money(t1.value - value); }
```

###  << operator overloading

```cpp
  friend std::ostream &operator<<(std::ostream &os, const money &t);
std::ostream &operator<<(std::ostream &os, const money &t) {
  for (int i = 0; i < t.size; i++)
    os << t.data[i] << " ";
  return os;
}
```

###  >> operator overloading


```cpp
  friend std::istream &operator>>(std::istream &is, const money &t);
std::istream &operator>>(std::istream &is, const money &t) {
  is >> t.value;
  return is;
}
```

now we can use it as:

```cpp
  money money3;
  std::cout << "Enter the size: " << std::endl;

  std::cin >> money3;
  std::cout << "The operator >> gives you:" << money3 << std::endl;

  money money1(7), money2(3);
  std::cout << "The operator - gives you:" << std::endl;
  std::cout << money2 - money1 << std::endl;
  std::cout << "The operator ()  gives you:\n" << money1() << std::endl;
  std::cout << "The operator () << gives you:\n" << money1 << std::endl;
  
```




[code](../src/class/operator_overloading.cpp)
