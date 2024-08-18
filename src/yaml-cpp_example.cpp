#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/yaml.h>

// https://github.com/jbeder/yaml-cpp/wiki/Tutorial

void loadingConfigurationFile(std::string yamlFile) {
  YAML::Node config = YAML::LoadFile(yamlFile);

  if (config["lastLogin"]) {
    std::cout << "Last logged in: " << config["lastLogin"].as<std::string>()
              << "\n";
  }

  const std::string username = config["username"].as<std::string>();
  const std::string password = config["password"].as<std::string>();

  std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
  std::time_t now_c = std::chrono::system_clock::to_time_t(now);
  std::tm now_tm = *std::localtime(&now_c);

  config["lastLogin"] = asctime(&now_tm);

  std::ofstream fout(yamlFile);
  fout << config;
}

void parsingAndNodeEditing() { // https://github.com/jbeder/yaml-cpp/wiki/Tutorial#basic-parsing-and-node-editing
}

void buildingNodes(std::string yamlFile) {
  YAML::Node node;       // starts out as null
  node["key"] = "value"; // it now is a map node
  node["seq"].push_back(
      "first element"); // node["seq"] automatically becomes a sequence
  node["seq"].push_back("second element");

  node["mirror"] = node["seq"][0]; // this creates an alias
  node["seq"][0] = "1st element";  // this also changes node["mirror"]
  node["mirror"] = "element #1";   // and this changes node["seq"][0] - they're
                                   // really the "same" node

  node["self"] = node;                // you can even create self-aliases
  node[node["mirror"]] = node["seq"]; // and strange loops :)
  std::ofstream fout(yamlFile);
  fout << node;
}

int main(int argc, char **argv) {
  std::string yamlFile = "../src/third_party_tools/data/config.yaml";
  loadingConfigurationFile(yamlFile);

  yamlFile = "../src/third_party_tools/data/data.yaml";
  buildingNodes(yamlFile);
}
