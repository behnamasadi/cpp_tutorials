#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
using json = nlohmann::json;

void jsonTypes() {
  /**/
  json j = {{"name", "John"},
            {"age", 30},
            {"isStudent", false},
            {"courses", {"math", "science"}},
            {"address", {{"city", "New York"}, {"zip", "10001"}}}};

  std::string serialized = j.dump(4);

  std::cout << "serialized json:" << serialized << std::endl;

  // Accessing and Modifying JSON
  std::string name = j["name"];
  int age = j["age"];
  bool isStudent = j["isStudent"];
  std::string city = j["address"]["city"];

  for (const auto &elem : j.items())
    std::cout << "key:" << elem.key() << " value:" << elem.value() << std::endl;
}

void createJSONFile(std::string JSONlFile) {
  /*
      Assume you want to create the JSON object

      {
        "pi": 3.141,
        "happy": true,
        "name": "Niels",
        "nothing": null,
        "answer": {
          "everything": 42
        },
        "list": [1, 0, 2],
        "object": {
          "currency": "USD",
          "value": 42.99
        }
      }
  */
}

int main() {

  std::string json_file_path =
      CMAKE_CURRENT_SOURCE_DIR + std::string("/src/data/data.csv");

  std::cout << "reading json file at: " << json_file_path << std::endl;

  createJSONFile(json_file_path);

  jsonTypes();
}
