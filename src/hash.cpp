#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>

// user-defined hash functions:
// Example 1

class Course {
public:
  std::string m_name;
  bool m_isAdvanced;

  Course(std::string name, bool isAdvanced) {
    m_name = name;
    m_isAdvanced = isAdvanced;
  }

  bool isAdvanced() { return m_isAdvanced; }

  bool operator==(const Course &rhs) const {
    return ((rhs.m_isAdvanced == this->m_isAdvanced) &&
            (rhs.m_name == this->m_name));
  }
};

class CourseHashFunction {
public:
  std::size_t operator()(const Course &k) const {
    // We use predfined hash functions of string and bool and define our hash
    // function as XOR of the hash values.
    return (std::hash<std::string>()(k.m_name)) ^
           (std::hash<bool>()(k.m_isAdvanced));
  }
};

// Example 2

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

} // namespace std
// If you don't want to specialize template inside the std namespace (although
// it's perfectly legal in this case), you can define the hash function as a
// separate class and add it to the template argument list for the map:

struct KeyHasher {
  std::size_t operator()(const student &k) const {

    return ((std::hash<std::string>()(k.first_name) ^
             (std::hash<std::string>()(k.last_name) << 1)) >>
            1) ^
           (std::hash<int>()(k.id) << 1);
  }
};

void unordered_mapCustomClasstype() {
  std::unordered_map<student, std::string> student_umap = {
      {{1, "John", "Doe"}, "example"}, {{2, "Mary", "Sue"}, "another"}};

  std::unordered_map<student, std::string, KeyHasher> m6 = {
      {{1, "John", "Doe"}, "example"}, {{2, "Mary", "Sue"}, "another"}};
}

void sizeOfTheHashTable() {
  // Example with std::unordered_map
  std::unordered_map<int, std::string> my_map = {
      {1, "one"}, {2, "two"}, {3, "three"}};

  std::cout << "Number of buckets in my_map: " << my_map.bucket_count()
            << std::endl;
  std::cout << "Current load factor in my_map: " << my_map.load_factor()
            << std::endl;
  std::cout << "Max load factor in my_map: " << my_map.max_load_factor()
            << std::endl;

  // Example with std::unordered_set
  std::unordered_set<int> my_set = {1, 2, 3, 4, 5};

  std::cout << "Number of buckets in my_set: " << my_set.bucket_count()
            << std::endl;
  std::cout << "Current load factor in my_set: " << my_set.load_factor()
            << std::endl;
  std::cout << "Max load factor in my_set: " << my_set.max_load_factor()
            << std::endl;

  // Accessing the size of a specific bucket
  size_t bucket_index = 0;
  std::cout << "Elements in bucket " << bucket_index
            << " of my_map: " << my_map.bucket_size(bucket_index) << std::endl;
}

int main() {
  unordered_mapCustomClasstype();
  sizeOfTheHashTable();
}
