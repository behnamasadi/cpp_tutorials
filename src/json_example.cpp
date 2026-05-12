// Educational example mirroring docs/json.md, using nlohmann/json.
// Each section below corresponds to a numbered section in the doc.

#include <fstream>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

// 1 & 2. Build a JSON object from a literal (key/value pairs, array, nested object).
void section_build() {
  std::cout << "\n--- 1 & 2. build json ---\n";
  json j = {{"name", "John"},
            {"age", 30},
            {"isStudent", false},
            {"courses", {"math", "science"}},
            {"address", {{"city", "New York"}, {"zip", "10001"}}}};
  std::cout << "j: " << j << "\n";
}

// 3. Parsing a string and serializing back (compact and pretty).
void section_parse_and_dump() {
  std::cout << "\n--- 3. parse and dump ---\n";
  std::string text = R"({"name": "John", "age": 30, "isStudent": false})";
  json j = json::parse(text);

  std::cout << "compact: " << j.dump() << "\n";
  std::cout << "pretty (indent=4):\n" << j.dump(4) << "\n";
}

// 4. Access via [] and at(), modify, add, erase.
void section_access_modify() {
  std::cout << "\n--- 4. access and modify ---\n";
  json j = {{"name", "John"},
            {"age", 30},
            {"isStudent", false},
            {"address", {{"city", "New York"}, {"zip", "10001"}}}};

  std::string name = j["name"];
  int age = j["age"];
  bool is_student = j["isStudent"];
  std::string city = j["address"]["city"];
  std::cout << "name=" << name << " age=" << age
            << " isStudent=" << is_student << " city=" << city << "\n";

  // at() throws if the key is missing.
  try {
    std::string zip = j.at("address").at("zip");
    std::cout << "zip (via at): " << zip << "\n";
  } catch (json::out_of_range& e) {
    std::cout << "out_of_range: " << e.what() << "\n";
  }

  // modify
  j["age"] = 31;
  j["address"]["city"] = "Boston";

  // add
  j["email"] = "john.doe@example.com";

  // erase
  j.erase("email");

  std::cout << "after modify/add/erase: " << j.dump() << "\n";
}

// 5. Iterate over object key/value pairs and over array elements.
void section_iterate() {
  std::cout << "\n--- 5. iterate ---\n";
  json j = {{"name", "John"},
            {"age", 30},
            {"courses", {"math", "science"}}};

  std::cout << "object items:\n";
  for (auto& el : j.items()) {
    std::cout << "  " << el.key() << ": " << el.value() << "\n";
  }

  std::cout << "array elements of courses:\n";
  json courses = j["courses"];
  for (auto& c : courses) {
    std::cout << "  " << c << "\n";
  }
}

// 6. STL containers <-> JSON.
void section_stl() {
  std::cout << "\n--- 6. stl interop ---\n";
  json j = {{"courses", {"math", "science"}},
            {"address", {{"city", "New York"}, {"zip", "10001"}}}};

  std::vector<std::string> courses = j["courses"];
  std::map<std::string, std::string> address = j["address"];

  std::cout << "vector courses:";
  for (auto& c : courses) std::cout << " " << c;
  std::cout << "\n";

  std::cout << "map address:";
  for (auto& kv : address) std::cout << " " << kv.first << "=" << kv.second;
  std::cout << "\n";

  std::vector<int> numbers = {1, 2, 3, 4};
  json j_numbers = numbers;
  std::cout << "vector->json: " << j_numbers << "\n";
}

// 7. Exceptions: parse_error, type_error, out_of_range.
void section_exceptions() {
  std::cout << "\n--- 7. exceptions ---\n";
  json j = {{"name", "John"}, {"age", 30}};

  try {
    int age = j.at("age").get<int>();
    std::cout << "age=" << age << "\n";
  } catch (json::exception& e) {
    std::cout << "exception: " << e.what() << "\n";
  }

  try {
    // type_error: name is a string, not an int.
    int x = j.at("name").get<int>();
    (void)x;
  } catch (json::exception& e) {
    std::cout << "expected type_error: " << e.what() << "\n";
  }

  try {
    // parse_error: malformed input.
    json bad = json::parse("{not json}");
    (void)bad;
  } catch (json::exception& e) {
    std::cout << "expected parse_error: " << e.what() << "\n";
  }
}

// 8. JSON arrays.
void section_arrays() {
  std::cout << "\n--- 8. arrays ---\n";
  json arr = json::array();
  arr.push_back("first");
  arr.push_back(2);
  arr.push_back(true);

  std::string first = arr[0];
  int second = arr[1];
  std::cout << "arr=" << arr << " arr[0]=" << first << " arr[1]=" << second
            << "\n";
}

// 9. Type checks.
void section_type_checks() {
  std::cout << "\n--- 9. type checks ---\n";
  json j = {{"name", "John"}, {"age", 30}};

  if (j["age"].is_number()) std::cout << "age is a number\n";
  if (j["name"].is_string()) std::cout << "name is a string\n";
}

// 10. Merging with merge_patch.
void section_merge() {
  std::cout << "\n--- 10. merge_patch ---\n";
  json j1 = {{"name", "John"}, {"age", 30}};
  json j2 = {{"age", 31}, {"city", "New York"}};
  j1.merge_patch(j2);
  std::cout << "merged:\n" << j1.dump(4) << "\n";
}

// 11. Saving and loading JSON from files (uses /tmp so it is self-contained).
void section_files() {
  std::cout << "\n--- 11. file i/o ---\n";
  const std::string path = "/tmp/json_example.json";

  json out = {{"name", "John"}, {"age", 30}};

  std::ofstream ofs(path);
  ofs << out.dump(4);
  ofs.close();
  std::cout << "wrote " << path << "\n";

  std::ifstream ifs(path);
  json in;
  ifs >> in;
  std::cout << "read back: " << in.dump() << "\n";
}

int main() {
  section_build();
  section_parse_and_dump();
  section_access_modify();
  section_iterate();
  section_stl();
  section_exceptions();
  section_arrays();
  section_type_checks();
  section_merge();
  section_files();
  return 0;
}
