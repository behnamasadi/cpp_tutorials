## map
`std::map` is red black tree and **NOT** hash table. Both `std::set` and `std::map` are associative containers. The difference is that `std::sets` contain  only the key, while in `std::map` there is an associated value. 
Choosing one over the other depends mainly on what the task at hand is.  If you want to build a dictionary of all the words that appear in a text, you could use a `std::set<std::string>`, but if you also want to count  how many times each word appeared (i.e. associate a value to the key) then you would need an `std::map<std::string,int>`. If you don't need to associate that count,
 it does not make sense to have the int that is unnecessary.
  
Time complexity of map operations is `O(Log n)` while for `unordered_set`, it is `O(1)` on average.


    
### map example
item are stored sorted:
```cpp
std::map<std::string, int> items;
std::pair< std::string,int > single_item;
single_item.first="book";
single_item.second=45;

items.insert(single_item);


single_item.first="beer";
single_item.second=10;

items.insert(single_item);

single_item.first="wine";
single_item.second=14;

items.insert(single_item);
```

if you set a new value for an existing keys, the new value will be set:
```cpp
items["melon"]=3;
```
this will replace the old value:
```cpp
items["melon"]=6;
```
If you try to insert an en existing key, it wouldn't be added and it return the false:
```cpp
std::pair<std::map<int,int>::iterator, bool> resultOfInsertion;


single_item.first="melon";
single_item.second=18;
resultOfInsertion=items.insert(single_item);


single_item.first="melon";
single_item.second=20;
resultOfInsertion=items.insert(single_item);


std::cout<<resultOfInsertion.second<<std::endl;

```
How iterate the map:

```cpp
for(std::map<std::string,int>::iterator it=items.begin();it!=items.end();++it)
{
    std::cout<<it->first<< ":"<<it->second <<std::endl;
}
```


**very important**: you should use `map.count(...)` instead of the `map[key]`, to count the number of your key. When you use `map[key]`, for instance you try to see the value of items with key `"foo"` in:

```cpp
std::cout << items["foo"] << std::endl;
```

The expression `items["foo"] ` attempts to retrieve the value associated with the key `"foo"` from the `items` map. If the key exists, it returns the associated value . If the key does not exist, the map will **insert a default value** for that key (which is 0 for integers in C++) and return it.

so after the above the code, this 


```cpp
  for (std::map<std::string, int>::iterator it = items.begin();
       it != items.end(); ++it) {
    std::cout << it->first << ":" << it->second << std::endl;
  }
```
This will give in the output:

```cpp
beer:10
book:45
foo:0  <------ 
melon:6
water melon:6
wine:14
```
and 

```cpp
  std::cout << "items.size():" << items.size() << std::endl;
  std::cout << items["foo"] << std::endl;
  std::cout << "items.size():" << items.size() << std::endl;
```
which gives us:


```
items.size():5
foo:0
items.size():6
```


### What `map.count(...)` Does:

- **Existence Check**: `map.count(key)` returns the number of elements with the key `key` in the map. For a `std::map`, this will always be `0` or `1`, because keys are unique. If `count` returns `1`, it means the key exists in the map, and you can safely access its value. If it returns `0`, the key does not exist.

- **Avoids Inserting New Keys**: By using `count`, you avoid the map accidentally inserting a new key with a default value. This ensures that your map only contains valid Roman numerals and their correct integer values.


## multimap


A **multimap** in C++ is an associative container that stores elements in key-value pairs, similar to a `map`, but with one key difference: in a `multimap`, multiple elements can have the same key. This makes `multimap` suitable for situations where you need to associate multiple values with a single key.

### Characteristics of a `multimap`:
1. **Multiple Keys:** Unlike `map`, which only allows unique keys, `multimap` allows multiple elements to share the same key.
2. **Ordered:** The elements in a `multimap` are **stored in an order based on the key**, typically in ascending order.
3. **Underlying Structure:** Internally, `multimap` is usually implemented as a balanced binary search tree, which ensures that operations such as insertion, deletion, and lookup are efficient, typically O(log n).
4. **No Direct Access:** Since `multimap` allows duplicate keys, it does not support direct access through `operator[]`, unlike `map`.

### Basic Operations:
- **Insertion:** You can insert elements into a `multimap` using the `insert` function.
- **Find:** You can use `find` to locate an element by key, though it will only return one of the elements with that key.
- **Equal Range:** To access all elements with a given key, you can use `equal_range`, which returns a range of iterators.

Imagine you're developing a system for managing employee records in a company. Each employee is identified by a department ID. However, multiple employees can belong to the same department. 

You need to store and manage employees in a way that allows you to quickly find all employees within a specific department. A `multimap` is a perfect choice here, as it lets you associate multiple employee names with the same department ID.

In the example above, the `multimap` stores department IDs as keys and employee names as values. When you need to retrieve all employees in a specific department, you can use `equal_range` to get the range of employees associated with that department.

This structure is ideal for scenarios where one-to-many relationships exist and where fast retrieval and ordered storage are important.

### Example Code:
```cpp

    std::multimap<int, std::string> employeeMap;

    // Inserting elements
    employeeMap.insert(std::make_pair(101, "John"));
    employeeMap.insert(std::make_pair(102, "Alice"));
    employeeMap.insert(std::make_pair(101, "Mike"));
    employeeMap.insert(std::make_pair(103, "Bob"));

    // Display all elements
    for (const auto& pair : employeeMap) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }

    // Find all employees with ID 101
    auto range = employeeMap.equal_range(101);
    std::cout << "Employees with ID 101:" << std::endl;
    for (auto it = range.first; it != range.second; ++it) {
        std::cout << it->second << std::endl;
    }
```

### Output:
```
101: John
101: Mike
102: Alice
103: Bob
Employees with ID 101:
John
Mike
```


## unordered_map example

word frequency in a string:
```cpp
std::unordered_map<std::string, int> wordFreq;
std::string str="a a b d c a d x";

// breaking input into word using string stream 
std::stringstream ss(str);  // Used for breaking words 
std::string word; // To store individual words 
while (ss >> word) 
    wordFreq[word]++; 

// now iterating over word, freq pair and printing 
// them in <, > format 
std::unordered_map<std::string, int>:: iterator p; 
for (p = wordFreq.begin(); p != wordFreq.end(); p++) 
    std::cout << "(" << p->first << ", " << p->second << ")\n";
```

## unordered_map user defined type
suppose we want to store the following class in an `unordered_map`:

```cpp
class student
{
public:
    int id;
    std::string first_name;
    std::string last_name;
    
    bool operator==(const student &other) const
    {
        return (first_name == other.first_name   && last_name == other.last_name  && id == other.id);
    }
};
```
example for user-defined hash functions:

```cpp
namespace std 
{

  template <>
  struct hash<student>
  {
    std::size_t operator()(const student& k) const
    {
    using std::size_t;
    using std::hash;
    using std::string;

    // Compute individual hash values for first,
    // second and third and combine them using XOR
    // and bit shifting:

    return ((hash<string>()(k.first_name)
            ^ (hash<string>()(k.last_name) << 1)) >> 1)
            ^ (hash<int>()(k.id) << 1);;
    }
  };

}
```
If you don't want to specialize template inside the std namespace (although it's perfectly legal in this case), you can define the hash function as a separate class and add it to the template argument list for the map:

```cpp
struct KeyHasher
{
  std::size_t operator()(const student& k) const
  {
    using std::size_t;
    using std::hash;
    using std::string;

    return ((hash<string>()(k.first_name)
             ^ (hash<string>()(k.last_name) << 1)) >> 1)
             ^ (hash<int>()(k.id) << 1);
  }
};

bool fncomp (int lhs, int rhs) {return lhs<rhs;}

struct classcomp {
	bool operator() (const int& lhs, const int& rhs) const
	{return lhs<rhs;}
};
```

Now you can store the object of type `student` in an `unordered_map`:
```cpp
std::unordered_map<student,std::string> student_umap
= {  { {1,"John", "Doe"}, "example"},  { {2,"Mary", "Sue"}, "another"} };


std::unordered_map<student,std::string,KeyHasher> m6 = {
{ {1,"John", "Doe"}, "example"},
{ {2,"Mary", "Sue"}, "another"}};
```

## set user defined type

first way:
```cpp
auto cmp = [](student left, student right) { return  (left.id>right.id);};
auto set_of_students = std::set<int,decltype(cmp)>( cmp );
```

second way:
```
std::set<int,classcomp> fifth;                 // class as Compare
```

third way:
```
bool(*fn_pt)(int,int) = fncomp;
std::set<int,bool(*)(int,int)> sixth (fn_pt);  // function pointer as Compare
```

 
 
## set 

### Scenario: Managing Unique Usernames in a Chat Application

Imagine you're developing a chat application where each user needs to have a unique username. As users join the chat, their usernames must be stored in such a way that:
1. **Usernames are unique**: No two users can have the same username.
2. **Usernames are sorted alphabetically**: For display purposes, the list of usernames should always be in alphabetical order.
3. **Efficient operations**: You need to efficiently check if a username already exists, add a new username, and remove a username when a user leaves.

### Why `std::set`?

- **Uniqueness**: `std::set` automatically handles the uniqueness of elements. If you try to insert a duplicate username, `std::set` will simply not add it, which saves you from manually checking for duplicates.
- **Sorting**: `std::set` keeps the elements sorted by default. This is ideal for displaying usernames in alphabetical order without requiring additional sorting operations.
- **Efficiency**: `std::set` is usually implemented as a balanced binary search tree (like a Red-Black Tree), which ensures that operations like insertion, deletion, and lookup all have logarithmic time complexity `O(log n)`.

### Implementation Example:

```cpp

    // Declare a set to store unique usernames
    std::set<std::string> usernames;

    // Adding usernames
    usernames.insert("Alice");
    usernames.insert("Bob");
    usernames.insert("Charlie");
    usernames.insert("Alice"); // This won't be added again

    // Check if a username exists
    if (usernames.find("Alice") != usernames.end()) {
        std::cout << "Alice is in the chat." << std::endl;
    }

    // Display all usernames (automatically sorted)
    std::cout << "Usernames in chat:" << std::endl;
    for (const auto& name : usernames) {
        std::cout << name << std::endl;
    }

    // Remove a username
    usernames.erase("Bob");

    // Display usernames after removal
    std::cout << "Usernames after Bob left:" << std::endl;
    for (const auto& name : usernames) {
        std::cout << name << std::endl;
    }

```

## unordered_set

The key difference here is that `std::unordered_set` provides faster average-time complexity for lookups, insertions, and deletions compared to `std::set`, but it does not maintain any particular order of elements.

### Scenario: Managing a Fast-Access Blocklist in a Network Firewall

Imagine you're developing a network firewall application that needs to manage a blocklist of IP addresses. The blocklist must be:
1. **Efficiently searchable**: The application should quickly determine if an incoming IP address is in the blocklist to block it immediately.
2. **Unique**: Each IP address in the blocklist should be unique.
3. **Order not required**: There is no need to keep the IP addresses sorted in any specific order; the primary concern is fast access.

### Why `std::unordered_set`?

- **Constant Time Complexity**: `std::unordered_set` provides average `O(1)` time complexity for lookups, insertions, and deletions because it is typically implemented using a hash table. This is crucial in a firewall scenario where speed is paramount.
- **Uniqueness**: Like `std::set`, `std::unordered_set` also ensures that each element is unique. If an IP address is already in the blocklist, adding it again will have no effect.
- **No Need for Ordering**: Since there’s no requirement to maintain any specific order of IP addresses, the unordered nature of `std::unordered_set` is ideal, avoiding the overhead associated with maintaining order.

### Implementation Example:

```cpp

    // Declare an unordered_set to store unique IP addresses in the blocklist
    std::unordered_set<std::string> blocklist;

    // Adding IP addresses to the blocklist
    blocklist.insert("192.168.1.1");
    blocklist.insert("10.0.0.2");
    blocklist.insert("172.16.0.3");
    blocklist.insert("192.168.1.1"); // This won't be added again

    // Check if an IP address is in the blocklist
    std::string ip = "192.168.1.1";
    if (blocklist.find(ip) != blocklist.end()) {
        std::cout << "IP " << ip << " is blocked." << std::endl;
    } else {
        std::cout << "IP " << ip << " is allowed." << std::endl;
    }

    // Remove an IP address from the blocklist
    blocklist.erase("10.0.0.2");

    // Check the blocklist again
    ip = "10.0.0.2";
    if (blocklist.find(ip) != blocklist.end()) {
        std::cout << "IP " << ip << " is blocked." << std::endl;
    } else {
        std::cout << "IP " << ip << " is allowed." << std::endl;
    }

```

### Why Only `std::unordered_set`?

- **Speed**: In a performance-critical application like a firewall, the speed of operations is crucial. The average `O(1)` time complexity for `std::unordered_set` operations provides significant performance benefits over the `O(log n)` time complexity of `std::set`.
- **Simplicity**: There’s no need to manage ordering when it’s unnecessary for the use case. Using `std::unordered_set` avoids the overhead of maintaining sorted order, making it simpler and faster.
- **Uniqueness**: Just like `std::set`, `std::unordered_set` ensures all elements are unique without extra checks.

This is a case where `std::unordered_set` is the optimal choice due to its fast access times, which are essential in a real-time application like a firewall. Its simplicity and efficiency make it the best tool for the job when order doesn't matter.



## multiset

it is similar to a `set`, but with one key difference: it allows multiple elements with the same value. This is particularly useful when you need to store and manage collections of elements where duplicates are allowed, and you still want to maintain an ordered sequence.

### Characteristics of a `multiset`:
1. **Duplicates Allowed:** Unlike a `set`, which only allows unique elements, a `multiset` can contain multiple instances of the same element.
2. **Ordered:** The elements in a `multiset` are automatically sorted based on their values, typically in ascending order.
3. **Underlying Structure:** Internally, `multiset` is usually implemented as a balanced binary search tree (e.g., a red-black tree), which ensures that insertion, deletion, and lookup operations are efficient, typically O(log n).
4. **No Random Access:** Elements in a `multiset` cannot be accessed using an index (like in arrays or vectors), but you can iterate over them or search using iterators.

### Basic Operations:
- **Insertion:** You can insert elements into a `multiset` using the `insert` function. Since duplicates are allowed, each insertion of the same element will increase the size of the `multiset`.
- **Count:** The `count` function returns the number of occurrences of a particular element.
- **Find:** The `find` function locates an element in the `multiset`.
- **Erase:** The `erase` function removes elements from the `multiset`. You can remove all occurrences of an element or just one at a time.

Imagine you are developing a system to manage inventory in a warehouse. You need to track the number of identical items, such as multiple units of the same product. Each product is identified by a specific ID, and you want to maintain an ordered list of these product IDs, while allowing duplicates since the warehouse can have multiple units of the same product.

A `multiset` would be a good choice for this scenario. It allows you to insert product IDs into the inventory while automatically keeping them sorted. You can also easily count how many units of a particular product you have by using the `count` function, or remove items when they are shipped out.

For instance, if the warehouse receives five units of product with ID `101` and three units of product with ID `102`, you can use a `multiset` to store these IDs. If a customer orders two units of product `101`, you can remove two occurrences from the `multiset`, while the remaining occurrences still reflect the correct inventory level.

This makes `multiset` ideal for situations where managing and tracking multiple instances of identical elements is necessary, particularly when maintaining an ordered collection.


**std::unordered_set::count**
 Because unordered_set containers do not allow for duplicate values, this means that the function actually returns 1 if an element with that value exists in the container, and zero otherwise.

### Example Code:
```cpp
std::multiset<std::string> items_ID;

  // Inserting elements
  items_ID.insert("ID5");
  items_ID.insert("ID3");
  items_ID.insert("ID7");
  items_ID.insert("ID3"); // Duplicate
  items_ID.insert("ID5"); // Duplicate

  // Display all elements
  for (const auto &item_ID : items_ID) {
    std::cout << item_ID << " ";
  }
  std::cout << std::endl;

  // Count occurrences of ID3
  std::cout << "Count of ID3: " << items_ID.count("ID3") << std::endl;

  // Remove one occurrence of 5
  items_ID.erase(items_ID.find("ID5"));

  // Display all elements after erasure
  for (const auto &item_ID : items_ID) {
    std::cout << item_ID << " ";
  }
  std::cout << std::endl;
```

### Output:
```
ID3 ID3 ID5 ID5 ID7 
Count of ID3: 2
ID3 ID3 ID5 ID7 
```


## unordered_set user defined type
let say we want to store the following class in an `unordered_set`:
```cpp
class Course
{
public:
    std::string m_name;
    bool m_isAdvanced;

    Course (std::string name, bool isAdvanced)
    {
        m_name=name;
        m_isAdvanced=isAdvanced;
    }

    bool isAdvanced()
    {
        return m_isAdvanced;
    }

    bool operator ==(const Course& rhs) const
    {
        return ((rhs.m_isAdvanced==this->m_isAdvanced )&&(rhs.m_name==this->m_name));
    }
};
```

The `unordered_set` internally implements a hash table to store elements. By default we can store only
per-defined type as int, string, float etc.
Some comparison function need to be designed. Since unordered_set also store implements hash table
to store elements we should also have to implement hash function to perform hashing related work. here we define the hash function for our class:

```cpp
class CourseHashFunction
{
public:
    std::size_t operator ()(const Course& k) const
    {
        // We use predfined hash functions of string and bool and define our hash function as XOR of the hash values.
        return (std::hash<std::string>()(k.m_name)) ^ (std::hash<bool>()(k.m_isAdvanced)) ;
    }
};
```

Now we can store object of type `Course`:

```cpp
std::unordered_set<Course,CourseHashFunction> courses;

Course c1=Course("Smalltalk Programming", false);
Course c2=Course("Appreciating Single Malts", true);

courses.insert(c1);
courses.insert(c2);

courses.find(c2)->m_name;
```

## Real-world Examples and Applications of std::unordered_map and std::unordered_set


`std::unordered_map` and `std::unordered_set` are powerful containers in C++ that are particularly useful in situations where you need fast lookups, insertions, and deletions. Here are some real-world examples where these containers shine:

### 1. **Caching (Memoization) in Dynamic Programming**
   - **Scenario**: You're implementing a dynamic programming solution, such as solving the Fibonacci sequence, and want to avoid recalculating results for the same inputs.
   - **Use Case**: A `std::unordered_map` can be used to store previously computed values (e.g., `fib(n)`) so that when the function is called with the same argument again, the result can be retrieved in constant time.
   - **Example**: Calculating Fibonacci numbers using memoization.
     ```cpp
     std::unordered_map<int, long long> fib_cache;

     long long fib(int n) {
         if (n <= 1) return n;
         if (fib_cache.find(n) != fib_cache.end()) {
             return fib_cache[n];
         }
         long long result = fib(n - 1) + fib(n - 2);
         fib_cache[n] = result;
         return result;
     }
     ```

### 2. **Counting Word Frequencies in Text Processing**
   - **Scenario**: In text processing or natural language processing (NLP), you often need to count the frequency of words in a large corpus of text.
   - **Use Case**: A `std::unordered_map<std::string, int>` can efficiently store words as keys and their frequencies as values. The fast lookups provided by the hash table are crucial when processing large amounts of text.
   - **Example**: Counting the frequency of each word in a document.
     ```cpp
     std::unordered_map<std::string, int> word_count;

     void count_words(const std::string& text) {
         std::istringstream stream(text);
         std::string word;
         while (stream >> word) {
             ++word_count[word];
         }
     }
     ```

### 3. **Tracking Unique Visitors on a Website**
   - **Scenario**: A website wants to track the number of unique visitors in a day by storing their IP addresses.
   - **Use Case**: A `std::unordered_set<std::string>` is ideal for this scenario, where each IP address is stored only once, ensuring uniqueness. The fast insertions and lookups help maintain performance even with a large number of visitors.
   - **Example**: Tracking unique visitor IP addresses.
     ```cpp
     std::unordered_set<std::string> unique_ips;

     void log_visit(const std::string& ip_address) {
         unique_ips.insert(ip_address);
     }

     size_t unique_visitors() {
         return unique_ips.size();
     }
     ```

### 4. **Building an Inverted Index for a Search Engine**
   - **Scenario**: In a search engine, you need to build an inverted index that maps each word to the list of documents that contain that word.
   - **Use Case**: A `std::unordered_map<std::string, std::unordered_set<int>>` can be used, where the key is a word, and the value is a set of document IDs. The `std::unordered_set` ensures that each document ID is unique for a given word.
   - **Example**: Building an inverted index.
     ```cpp
     std::unordered_map<std::string, std::unordered_set<int>> inverted_index;

     void index_document(int doc_id, const std::string& content) {
         std::istringstream stream(content);
         std::string word;
         while (stream >> word) {
             inverted_index[word].insert(doc_id);
         }
     }
     ```

### 5. **Routing in Network Applications**
   - **Scenario**: In a peer-to-peer network application, you need to manage a dynamic list of active connections (identified by IP and port) to route data efficiently.
   - **Use Case**: A `std::unordered_set<std::pair<std::string, int>, CustomHash>` can track active connections. Using a custom hash function to hash the combination of IP address and port ensures that connections are unique and can be efficiently managed.
   - **Example**: Managing active network connections.
     ```cpp
     struct Connection {
         std::string ip;
         int port;
         bool operator==(const Connection& other) const {
             return ip == other.ip && port == other.port;
         }
     };

     struct ConnectionHash {
         std::size_t operator()(const Connection& conn) const {
             return std::hash<std::string>()(conn.ip) ^ std::hash<int>()(conn.port);
         }
     };

     std::unordered_set<Connection, ConnectionHash> active_connections;

     void add_connection(const std::string& ip, int port) {
         active_connections.insert({ip, port});
     }
     ```

### 6. **Deduplication in Large Datasets**
   - **Scenario**: You have a large dataset with potential duplicate records, and you want to remove these duplicates efficiently.
   - **Use Case**: A `std::unordered_set` is ideal for deduplication, as it only allows unique elements. You can insert records into the set, and duplicates will be automatically discarded.
   - **Example**: Deduplicating a list of user IDs.
     ```cpp
     std::unordered_set<int> unique_user_ids;

     void add_user(int user_id) {
         unique_user_ids.insert(user_id);
     }
     ```

### Why `std::unordered_map` and `std::unordered_set`?

- **Performance**: Both containers provide average-case constant time complexity (`O(1)`) for insertions, lookups, and deletions, which is often crucial in performance-sensitive applications.
- **Ease of Use**: They provide a simple and effective way to manage key-value pairs or unique collections without worrying about the underlying implementation details.
- **Flexibility**: They can be used in a variety of real-world scenarios that require fast access to data, uniqueness enforcement, or efficient key-based retrieval.

These examples demonstrate how `std::unordered_map` and `std::unordered_set` can be applied to solve practical problems efficiently, leveraging their strengths in situations where speed and unique data management are critical.




## Big O `std::unordered_map`, `std::unordered_set`, `std::map`, `std::set`, `std::multimap`, and `std::multiset`

a comparison of the time complexity (Big O) for various operations in `std::unordered_map`, `std::unordered_set`, `std::map`, `std::set`, `std::multimap`, and `std::multiset`:

| Operation         | `std::unordered_map` | `std::unordered_set` | `std::map` | `std::set` | `std::multimap` | `std::multiset` |
|-------------------|----------------------|----------------------|------------|------------|-----------------|-----------------|
| **Insertion**     | O(1) on average, O(n) in worst case | O(1) on average, O(n) in worst case | O(log n) | O(log n) | O(log n) | O(log n) |
| **Access (Find)** | O(1) on average, O(n) in worst case | O(1) on average, O(n) in worst case | O(log n) | O(log n) | O(log n) | O(log n) |
| **Deletion**      | O(1) on average, O(n) in worst case | O(1) on average, O(n) in worst case | O(log n) | O(log n) | O(log n) | O(log n) |
| **Space**         | O(n) | O(n) | O(n) | O(n) | O(n) | O(n) |

### Notes:
1. **`std::unordered_map` and `std::unordered_set`:**
   - The average time complexity for insertion, access, and deletion is O(1) due to hash table implementation.
   - However, in the worst case (e.g., when many elements hash to the same bucket), the time complexity can degrade to O(n).

2. **`std::map`, `std::set`, `std::multimap`, and `std::multiset`:**
   - These are implemented as self-balancing binary search trees (usually red-black trees), so the time complexity for insertion, access, and deletion is O(log n).

3. **`std::multimap` and `std::multiset`:**
   - These containers allow multiple elements with the same key (multimap for key-value pairs, multiset for values only).
   - The time complexity for operations remains the same as `std::map` and `std::set`.




## std::tie, std::pair, std::tuple

In C++, `std::tie`, `std::pair`, and `std::tuple` are useful features provided by the Standard Library that allow you to handle multiple values together in a convenient and structured way. Here's a breakdown of each and how they can be used in real-world applications:

### 1. `std::pair`
A `std::pair` is a simple container that holds two values, which may be of different types. It is often used when you want to return two values from a function or associate two related values together.

#### **Example: Using `std::pair` in a real-world application**

Imagine you are building a function that returns the minimum and maximum values from an array of integers. You can use `std::pair` to return both values together:

```cpp
#include <iostream>
#include <vector>
#include <utility> // for std::pair

std::pair<int, int> findMinMax(const std::vector<int>& numbers) {
    int min = numbers[0];
    int max = numbers[0];

    for (int number : numbers) {
        if (number < min) min = number;
        if (number > max) max = number;
    }

    return std::make_pair(min, max); // Returning a pair of min and max
}

int main() {
    std::vector<int> numbers = {3, 5, 1, 9, 2, 8, -1, 6};
    std::pair<int, int> minMax = findMinMax(numbers);

    std::cout << "Min: " << minMax.first << ", Max: " << minMax.second << std::endl;

    return 0;
}
```

### 2. `std::tuple`
A `std::tuple` is like an extension of `std::pair` that can hold more than two values of potentially different types. It is useful when you need to return multiple values from a function or group related values together.

#### **Example: Using `std::tuple` in a real-world application**

Suppose you're writing a program to process student records, and you want a function that returns a student's name, age, and GPA. You can use `std::tuple` to return all three values together.

```cpp
#include <iostream>
#include <tuple>
#include <string>

std::tuple<std::string, int, double> getStudentInfo() {
    std::string name = "John Doe";
    int age = 20;
    double gpa = 3.75;

    return std::make_tuple(name, age, gpa); // Returning a tuple of name, age, and GPA
}

int main() {
    auto studentInfo = getStudentInfo();

    std::string name;
    int age;
    double gpa;

    // Unpacking the tuple using std::tie
    std::tie(name, age, gpa) = studentInfo;

    std::cout << "Name: " << name << ", Age: " << age << ", GPA: " << gpa << std::endl;

    return 0;
}
```

### 3. `std::tie`
`std::tie` is used to unpack values from a `std::pair` or `std::tuple` into individual variables. This is especially handy when you don't want to deal with the tuple's index-based access.

#### **Example: Using `std::tie` to unpack values**

In the example above, `std::tie` is used to unpack the tuple into individual variables (`name`, `age`, and `gpa`). This makes the code more readable and convenient.

Another example could be unpacking the return value of a function that returns a `std::pair`:

```cpp
#include <iostream>
#include <vector>
#include <tuple>
#include <string>

std::tuple<std::string, int, double> analyzeText(const std::string& text) {
    int wordCount = 0;
    int charCount = text.length();
    double averageWordLength = charCount;

    // Simple analysis: let's say each word is roughly 5 characters
    wordCount = charCount / 5;
    averageWordLength = charCount / static_cast<double>(wordCount);

    return std::make_tuple(text, wordCount, averageWordLength);
}

int main() {
    std::string text = "This is a simple example sentence.";
    std::string analyzedText;
    int wordCount;
    double avgWordLength;

    // Unpacking tuple returned from analyzeText
    std::tie(analyzedText, wordCount, avgWordLength) = analyzeText(text);

    std::cout << "Text: " << analyzedText << "\nWord Count: " << wordCount
              << "\nAverage Word Length: " << avgWordLength << std::endl;

    return 0;
}
```

### Summary

- **`std::pair`**: Useful for storing two related values, often used for returning two values from a function.
- **`std::tuple`**: Extends `std::pair` by allowing more than two values of different types to be stored together.
- **`std::tie`**: Simplifies unpacking `std::pair` or `std::tuple` into individual variables, improving code readability.



 
## tie
The work of `tie()` is to unpack the tuple values into seperate variables. There are two variants of `tie()`, with and without “ignore” , 
the "ignore" ignores a particular tuple element and stops it from getting unpacked.


```cpp
int i_val; 
char ch_val; 
float f_val;    

// Initializing tuple 
std::tuple <int,char,float> tup1(20,'g',17.5);


std::tie(i_val,ch_val,f_val) = tup1;

std::tie(i_val,std::ignore,f_val) = tup1;
std::cout<<i_val<<" " <<ch_val<<" " <<f_val  <<std::endl;
```



## tuple
A tuple is an object that can hold a number of elements. The elements can be of different data types.


```cpp
std::tuple<int, double, int, std::string > mytuple = std::make_tuple(10, 12.4, 3, "this is a tuple");
```
`get()` is used to access the tuple values and modify them, it accepts the index and tuple name as arguments 

```cpp
std::cout<<std::get<0>(mytuple) <<std::endl;
std::cout << std::get<1>(mytuple) << std::endl;
std::cout << std::get<2>(mytuple) << std::endl;
std::cout << std::tuple_size<decltype(mytuple)>::value << std::endl;
```

`tie()` : unpack the tuple values into seperate variables. There are two variants of tie()

```cpp
int i; 
double d; 
int n; 
std::string s;
std::tie(i, d, n, s) = mytuple;
```


## pair

```cpp
//std::make_pair()
std::pair<std::string, int> item1,item2;
item1.first="wieght";
item1.second=12;

item2=std::make_pair("size",12);
```

checking existence of key in a map using `std::map::count()`:
```cpp
std::map<std::string, int> wordMap = { {"a",0}, {"b",1}, {"c",2} };
if (wordMap.count("a") > 0)
{
    std::cout << "'a' Found" << std::endl;
}
```


checking the existence of key in a map using `std::map::find`:

```
std::map<std::string, int> items;
std::string searchingKey="melon";
std::cout<<searchingKey <<(items.find(searchingKey)!=items.end()? " found" :" not found") <<std::endl;
```

when key doesn't exist:
```cpp
  if (items.count("mumbo jumbo") == 0) {
    std::cout << "not found" << std::endl;
  }
```



