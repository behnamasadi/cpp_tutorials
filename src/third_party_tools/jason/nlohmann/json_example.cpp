#include "json.hpp"
#include <fstream>
#include <iostream>
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
  // With this library, you could write:

  // create an empty structure (null)
  json j;

  // add a number that is stored as double (note the implicit conversion of j to
  // an object)
  j["pi"] = 3.141;

  // add a Boolean that is stored as bool
  j["happy"] = true;

  // add a string that is stored as std::string
  j["name"] = "Niels";

  // add another null object by passing nullptr
  j["nothing"] = nullptr;

  // add an object inside the object
  j["answer"]["everything"] = 42;

  // add an array that is stored as std::vector (using an initializer list)
  j["list"] = {1, 0, 2};

  // add another object (using an initializer list of pairs)
  j["object"] = {{"currency", "USD"}, {"value", 42.99}};

  // instead, you could also write (which looks very similar to the JSON above)
  json j2 = {{"pi", 3.141},
             {"happy", true},
             {"name", "Niels"},
             {"nothing", nullptr},
             {"answer", {{"everything", 42}}},
             {"list", {1, 0, 2}},
             {"object", {{"currency", "USD"}, {"value", 42.99}}}};

  std::string j1_serialized_string = j.dump();
  // std::string j2_serialized_string = j2.dump();
  std::ofstream fout(JSONlFile);
  fout << j1_serialized_string;
}

int main() {
  std::string JSONlFile = "../src/third_party_tools/data/data.json";
  createJSONFile(JSONlFile);
}
