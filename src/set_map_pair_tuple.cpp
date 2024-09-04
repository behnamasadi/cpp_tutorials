#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

void tupleExample() {
  std::tuple<int, double, int, std::string> mytuple =
      std::make_tuple(10, 12.4, 3, "this is a tuple");
  // get() is used to access the tuple values and modify them, it accepts the
  // index and tuple name as arguments

  std::cout << std::get<0>(mytuple) << std::endl;
  std::cout << std::get<1>(mytuple) << std::endl;
  std::cout << std::get<2>(mytuple) << std::endl;
  std::cout << std::tuple_size<decltype(mytuple)>::value << std::endl;

  // tie() : unpack the tuple values into seperate variables. There are two
  // variants of tie()

  int i;
  double d;
  int n;
  std::string s;
  std::tie(i, d, n, s) = mytuple;
}

void pairExample() {
  // std::make_pair()
  std::pair<std::string, int> item1, item2;
  item1.first = "wieght";
  item1.second = 12;

  item2 = std::make_pair("size", 12);
}

void map_Example() {
  // item are stored sorted
  std::map<std::string, int> items;
  std::pair<std::string, int> single_item;
  single_item.first = "book";
  single_item.second = 45;

  items.insert(single_item);

  single_item.first = "beer";
  single_item.second = 10;

  items.insert(single_item);

  single_item.first = "wine";
  single_item.second = 14;

  items.insert(single_item);

  // keys should be unique,
  items["melon"] = 3;
  // this will replace the old value
  items["melon"] = 6;
  // this even won't be added
  single_item.first = "melon";
  single_item.second = 18;
  items.insert(single_item);

  items["water melon"] = 6;

  std::cout << "items.size():" << items.size() << std::endl;
  std::cout << items["foo"] << std::endl;
  std::cout << "items.size():" << items.size() << std::endl;

  for (std::map<std::string, int>::iterator it = items.begin();
       it != items.end(); ++it) {
    std::cout << it->first << ":" << it->second << std::endl;
  }
}

void checkExsitanceOfKeyInMap() {
  // using std::map::count()
  // finding an item based on the key for an item in map

  std::map<std::string, int> wordMap = {{"a", 0}, {"b", 1}, {"c", 2}};

  if (wordMap.count("a") > 0) {
    std::cout << "'a' Found" << std::endl;
  }

  // using std::map::find
  std::map<std::string, int> items;
  std::string searchingKey = "melon";
  std::cout << searchingKey
            << (items.find(searchingKey) != items.end() ? " found"
                                                        : " not found")
            << std::endl;

  // when key doesn't exist:
  if (items.count("mumbo jumbo") == 0) {
    std::cout << "not found" << std::endl;
  }
}

void unordered_mapExample() {

  std::cout << "item are sorted in std::map \nThe order that we inserted the "
               "items is: wine, beer, book"
            << std::endl;
  std::map<std::string, int> items;
  std::pair<std::string, int> single_item;

  single_item.first = "wine";
  single_item.second = 14;
  items.insert(single_item);

  single_item.first = "beer";
  single_item.second = 10;
  items.insert(single_item);

  single_item.first = "book";
  single_item.second = 45;
  items.insert(single_item);

  std::cout << "The order that we iterate items in map is:" << std::endl;

  for (std::map<std::string, int>::iterator it = items.begin();
       it != items.end(); ++it) {
    std::cout << it->first << ":" << it->second << std::endl;
  }
}

void wordFrequencyInString() {
  std::unordered_map<std::string, int> wordFreq;
  std::string str = "a a b d c a d x";

  // breaking input into word using string stream
  std::stringstream ss(str); // Used for breaking words
  std::string word;          // To store individual words
  while (ss >> word)
    wordFreq[word]++;

  // now iterating over word, freq pair and printing
  // them in <, > format
  std::unordered_map<std::string, int>::iterator p;
  for (p = wordFreq.begin(); p != wordFreq.end(); p++)
    std::cout << "(" << p->first << ", " << p->second << ")\n";
}
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

// example for user-defined hash functions:

namespace std {

template <> struct hash<student> {
  std::size_t operator()(const student &k) const {
    using std::hash;
    using std::size_t;
    using std::string;

    // Compute individual hash values for first,
    // second and third and combine them using XOR
    // and bit shifting:

    return ((hash<string>()(k.first_name) ^
             (hash<string>()(k.last_name) << 1)) >>
            1) ^
           (hash<int>()(k.id) << 1);
    ;
  }
};

} // namespace std

// If you don't want to specialize template inside the std namespace (although
// it's perfectly legal in this case), you can define the hash function as a
// separate class and add it to the template argument list for the map:

struct KeyHasher {
  std::size_t operator()(const student &k) const {
    using std::hash;
    using std::size_t;
    using std::string;

    return ((hash<string>()(k.first_name) ^
             (hash<string>()(k.last_name) << 1)) >>
            1) ^
           (hash<int>()(k.id) << 1);
  }
};

bool fncomp(int lhs, int rhs) { return lhs < rhs; }

struct classcomp {
  bool operator()(const int &lhs, const int &rhs) const { return lhs < rhs; }
};

void unordered_mapCustomClasstype() {
  std::unordered_map<student, std::string> student_umap = {
      {{1, "John", "Doe"}, "example"}, {{2, "Mary", "Sue"}, "another"}};

  std::unordered_map<student, std::string, KeyHasher> m6 = {
      {{1, "John", "Doe"}, "example"}, {{2, "Mary", "Sue"}, "another"}};
}

void multimapExample() {

  std::multimap<int, std::string> employeeMap;

  // Inserting elements
  employeeMap.insert(std::make_pair(101, "John"));
  employeeMap.insert(std::make_pair(102, "Alice"));
  employeeMap.insert(std::make_pair(101, "Mike"));
  employeeMap.insert(std::make_pair(103, "Bob"));

  // Display all elements
  for (const auto &pair : employeeMap) {
    std::cout << pair.first << ": " << pair.second << std::endl;
  }

  // Find all employees with ID 101
  auto range = employeeMap.equal_range(101);
  std::cout << "Employees with ID 101:" << std::endl;
  for (auto it = range.first; it != range.second; ++it) {
    std::cout << it->second << std::endl;
  }

  return;
}

void setExample() {
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
  for (const auto &name : usernames) {
    std::cout << name << std::endl;
  }

  // Remove a username
  usernames.erase("Bob");

  // Display usernames after removal
  std::cout << "Usernames after Bob left:" << std::endl;
  for (const auto &name : usernames) {
    std::cout << name << std::endl;
  }
  return;
}

void setFromUserDefinedTypeExample() {
  // first way
  auto cmp = [](student left, student right) { return (left.id > right.id); };
  auto set_of_students = std::set<int, decltype(cmp)>(cmp);

  // second way
  std::set<int, classcomp> fifth; // class as Compare

  // third way
  bool (*fn_pt)(int, int) = fncomp;
  std::set<int, bool (*)(int, int)> sixth(fn_pt); // function pointer as Compare
}

void unordered_setExample() {
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

  return;
}

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

void unordered_setUserDefinedClassExample() {
  /*
  The unordered_set internally implements a hash table to store elements. By
  default we can store only pre definded type as int, string, float etc.

  Some comparison function need to be designed. Since unordered_set also
  store implements hash table to store elements we should also have to
  implement hash function to perform hashing related work.
  */

  std::unordered_set<Course, CourseHashFunction> courses;

  Course c1 = Course("Smalltalk Programming", false);
  Course c2 = Course("Appreciating Single Malts", true);

  courses.insert(c1);
  courses.insert(c2);

  courses.find(c2)->m_name;
}

void tieExample() {
  int i_val;
  char ch_val;
  float f_val;

  // Initializing tuple
  std::tuple<int, char, float> tup1(20, 'g', 17.5);

  std::tie(i_val, ch_val, f_val) = tup1;

  std::tie(i_val, std::ignore, f_val) = tup1;
  std::cout << i_val << " " << ch_val << " " << f_val << std::endl;
}

template <typename T> typename T::iterator min_map_element(T &m) {
  return std::min_element(
      m.begin(), m.end(),
      [](typename T::value_type &l, typename T::value_type &r) -> bool {
        return l.second < r.second;
      });
}

void multiset_Example() {

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
  return;
}

int main() {
  // unordered_mapExample();
  // multiset_Example();

  map_Example();
  return 0;
}
