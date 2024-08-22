### YAML Example File

Let's assume we have a YAML file named `example.yaml` with the following content:

```yaml
name: John Doe
age: 30
address:
  street: 123 Maple Street
  city: Springfield
  state: IL
phones:
  - type: home
    number: 123-456-7890
  - type: work
    number: 987-654-3210
```

### C++ Code Using `yaml-cpp`

Here's a simple C++ program that uses `yaml-cpp` to parse the above YAML file and print the contents:

```cpp
#include <iostream>
#include <fstream>
#include <yaml-cpp/yaml.h>

int main() {
    try {
        // Load the YAML file
        YAML::Node config = YAML::LoadFile("example.yaml");

        // Access simple elements
        std::string name = config["name"].as<std::string>();
        int age = config["age"].as<int>();

        // Access nested elements
        std::string street = config["address"]["street"].as<std::string>();
        std::string city = config["address"]["city"].as<std::string>();
        std::string state = config["address"]["state"].as<std::string>();

        // Access sequence (array) elements
        const YAML::Node& phones = config["phones"];
        for (std::size_t i = 0; i < phones.size(); i++) {
            std::string type = phones[i]["type"].as<std::string>();
            std::string number = phones[i]["number"].as<std::string>();
            std::cout << "Phone (" << type << "): " << number << std::endl;
        }

        // Print the parsed data
        std::cout << "Name: " << name << std::endl;
        std::cout << "Age: " << age << std::endl;
        std::cout << "Address: " << street << ", " << city << ", " << state << std::endl;
    } catch (const YAML::Exception& e) {
        std::cerr << "Error parsing YAML: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
```



When you run the compiled program, it will print the contents of the YAML file:

```
Phone (home): 123-456-7890
Phone (work): 987-654-3210
Name: John Doe
Age: 30
Address: 123 Maple Street, Springfield, IL
```


[code](../src/yaml-cpp_example.cpp)
