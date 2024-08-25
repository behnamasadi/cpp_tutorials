# Hash Functions
In C++, `std::hash` is a function object, also known as a functor, that provides a way to obtain a hash value for a given input of a specific type. When you declare something like `std::hash<float> float_hasher;` or `std::hash<std::string> str_hasher;`, you're creating an instance of `std::hash` specialized for `float` or `std::string`. These are used to generate hash values from floating-point numbers or strings, respectively.

### How Does `std::hash` Work?

1. **Hash Function**:
   - `std::hash` provides a `operator()` function that takes an object of the specified type and returns a `std::size_t`, which is an unsigned integer type used to represent sizes.
   - This `operator()` essentially converts the input (e.g., a `float` or a `std::string`) into a hash value, which is a number that ideally distributes inputs uniformly across its range.

2. **Usage Example**:
   ```cpp
   std::hash<float> float_hasher;
   std::size_t hash_value = float_hasher(3.14f);  // Hash value for the float 3.14
   
   std::hash<std::string> str_hasher;
   std::size_t hash_value_str = str_hasher("hello");  // Hash value for the string "hello"
   ```

### User-defined Hash functions

```cpp
class student {
public:
  int id;
  std::string first_name;
  std::string last_name;

  bool operator==(const student &other) const {
    return (first_name == other.first_name && last_name == other.last_name &&
            id == other.id);
  }
};
```

hash function:

```cpp
namespace std {

template <> struct hash<student> {
  std::size_t operator()(const student &k) const {

    // Compute individual hash values for first,
    // second and third and combine them using XOR
    // and bit shifting:

    return ((std::hash<string>()(k.first_name) ^
             (std::hash<string>()(k.last_name) << 1)) >>
            1) ^
           (std::hash<int>()(k.id) << 1);
    ;
  }
};

}
```
If you don't want to specialize template inside the std namespace (although it's perfectly legal in this case), you can define the hash function as a separate class and add it to the template argument list for the map:

```cpp
struct KeyHasher {
  std::size_t operator()(const student &k) const {

    return ((std::hash<std::string>()(k.first_name) ^
             (std::hash<std::string>()(k.last_name) << 1)) >>
            1) ^
           (std::hash<int>()(k.id) << 1);
  }
};
```

In your main:

```cpp
std::unordered_map<student, std::string> student_umap = {
      {{1, "John", "Doe"}, "example"}, {{2, "Mary", "Sue"}, "another"}};

  std::unordered_map<student, std::string, KeyHasher> m6 = {
      {{1, "John", "Doe"}, "example"}, {{2, "Mary", "Sue"}, "another"}};
```
    


### Where is the Hash Table?

- **No Hash Table in `std::hash`**:
   - The `std::hash` function object itself does not involve a hash table. It simply computes a hash value for a given input.
   - The responsibility of organizing and storing these hash values in a hash table belongs to containers that utilize hashing, such as `std::unordered_map`, `std::unordered_set`, etc.

- **Hash Table in Containers**:
   - When you use a container like `std::unordered_map`, it internally uses `std::hash` to compute hash values for keys and organizes these values in a hash table.
   - The hash table itself is managed by the container, not by the `std::hash` function.

### What is the Size of the Hash Table?

In C++, to get information about the size of the hash table (i.e., the number of buckets) in a `std::unordered_map` or `std::unordered_set`, you can use the `bucket_count()` member function. This function returns the current number of buckets in the hash table.

Here are the relevant functions you can use:

1. **`bucket_count()`**: Returns the number of buckets in the hash table.
2. **`load_factor()`**: Returns the current load factor, which is the average number of elements per bucket.
3. **`max_load_factor()`**: Returns the maximum load factor before the container will automatically increase the number of buckets (rehash).
4. **`bucket_size(bucket_index)`**: Returns the number of elements in the specified bucket.

### Example Code

```cpp
    std::unordered_map<int, std::string> my_map = {{1, "one"}, {2, "two"}, {3, "three"}};

    std::cout << "Number of buckets in my_map: " << my_map.bucket_count() << std::endl;
    std::cout << "Current load factor in my_map: " << my_map.load_factor() << std::endl;
    std::cout << "Max load factor in my_map: " << my_map.max_load_factor() << std::endl;

    // Example with std::unordered_set
    std::unordered_set<int> my_set = {1, 2, 3, 4, 5};

    std::cout << "Number of buckets in my_set: " << my_set.bucket_count() << std::endl;
    std::cout << "Current load factor in my_set: " << my_set.load_factor() << std::endl;
    std::cout << "Max load factor in my_set: " << my_set.max_load_factor() << std::endl;

    // Accessing the size of a specific bucket
    size_t bucket_index = 0;
    std::cout << "Elements in bucket " << bucket_index << " of my_map: " << my_map.bucket_size(bucket_index) << std::endl;
```


For the code above, you might see output similar to:

```bash
Number of buckets in my_map: 8
Current load factor in my_map: 0.375
Max load factor in my_map: 1
Number of buckets in my_set: 10
Current load factor in my_set: 0.5
Max load factor in my_set: 1
Elements in bucket 0 of my_map: 0
```


[code](../src/hash.cpp)
