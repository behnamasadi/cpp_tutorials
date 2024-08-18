#include <fstream>
#include <iostream>
#include <json.hpp>
#include <string>
using json = nlohmann::json;

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
}
