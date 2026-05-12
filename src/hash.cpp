// Hash functions and hash-based containers.
// Follows docs/hash_function_hash_table.md.

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

// ----- std::hash on built-in types ---------------------------------------
void builtinHash() {
  std::cout << "--- std::hash on primitives and strings ---\n";

  std::hash<float> float_hasher;
  std::hash<int> int_hasher;
  std::hash<std::string> str_hasher;

  std::cout << "hash(3.14f) = " << float_hasher(3.14f) << "\n";
  std::cout << "hash(42)    = " << int_hasher(42) << "\n";
  std::cout << "hash(\"hello\") = " << str_hasher("hello") << "\n";
}

// ----- User-defined type + std::hash specialization ----------------------
class student {
public:
  int id;
  std::string first_name;
  std::string last_name;

  bool operator==(const student &other) const {
    return first_name == other.first_name && last_name == other.last_name &&
           id == other.id;
  }
};

// Specialization inside std:: lets unordered_map<student, V> work directly.
namespace std {
template <> struct hash<student> {
  std::size_t operator()(const student &k) const {
    // Combine field hashes with XOR and bit shifts.
    return ((std::hash<string>()(k.first_name) ^
             (std::hash<string>()(k.last_name) << 1)) >>
            1) ^
           (std::hash<int>()(k.id) << 1);
  }
};
} // namespace std

// Alternative: a standalone hasher passed as a template argument.
struct KeyHasher {
  std::size_t operator()(const student &k) const {
    return ((std::hash<std::string>()(k.first_name) ^
             (std::hash<std::string>()(k.last_name) << 1)) >>
            1) ^
           (std::hash<int>()(k.id) << 1);
  }
};

void userDefinedHash() {
  std::cout << "--- user-defined hash for a class ---\n";

  // Uses std::hash<student> specialization.
  std::unordered_map<student, std::string> student_umap = {
      {{1, "John", "Doe"}, "example"}, {{2, "Mary", "Sue"}, "another"}};

  // Uses KeyHasher passed as the third template argument.
  std::unordered_map<student, std::string, KeyHasher> student_umap2 = {
      {{1, "John", "Doe"}, "example"}, {{2, "Mary", "Sue"}, "another"}};

  std::cout << "student_umap  size = " << student_umap.size() << "\n";
  std::cout << "student_umap2 size = " << student_umap2.size() << "\n";
}

// ----- Bucket interface --------------------------------------------------
void bucketInterface() {
  std::cout << "--- bucket_count / load_factor / max_load_factor ---\n";

  std::unordered_map<int, std::string> my_map = {
      {1, "one"}, {2, "two"}, {3, "three"}};

  std::cout << "my_map bucket_count    = " << my_map.bucket_count() << "\n";
  std::cout << "my_map load_factor     = " << my_map.load_factor() << "\n";
  std::cout << "my_map max_load_factor = " << my_map.max_load_factor() << "\n";

  std::unordered_set<int> my_set = {1, 2, 3, 4, 5};

  std::cout << "my_set bucket_count    = " << my_set.bucket_count() << "\n";
  std::cout << "my_set load_factor     = " << my_set.load_factor() << "\n";
  std::cout << "my_set max_load_factor = " << my_set.max_load_factor() << "\n";

  // Size of a specific bucket.
  std::size_t bucket_index = 0;
  std::cout << "my_map bucket_size(" << bucket_index
            << ") = " << my_map.bucket_size(bucket_index) << "\n";
}

int main() {
  builtinHash();
  std::cout << "\n";
  userDefinedHash();
  std::cout << "\n";
  bucketInterface();
}
