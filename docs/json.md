JSON (JavaScript Object Notation) is a lightweight data-interchange format, easy for humans to read and write, and easy for machines to parse and generate. In C++, you can handle JSON using the `nlohmann::json` library, which provides a convenient API for manipulating JSON data. Below is a comprehensive explanation of JSON concepts with examples using `nlohmann::json` in C++.

### 1. **Basic JSON Structure**

A JSON object can contain:
- **Key-Value Pairs** (in a dictionary format)
- **Arrays**
- **Strings**
- **Numbers**
- **Booleans**
- **Null**

Example of a JSON:

```cpp
json j = {
    // "name": "John"             -> Key-Value Pair (string)
    {"name", "John"},
    
    // "age": 30                  -> Key-Value Pair (integer)                     
    {"age", 30},
    
    // "isStudent": false         -> Key-Value Pair (boolean)                          
    {"isStudent", false},
    
    // "courses": ["math", "science"] -> Key-Value Pair (array of strings)                 
    {"courses", {"math", "science"}},     
    
    // "address": { ... }         -> Key-Value Pair (dictionary/object)
    {"address", {                         
        {"city", "New York"},             // "city": "New York"         -> Key-Value Pair (string)
        {"zip", "10001"}                  // "zip": "10001"             -> Key-Value Pair (string)
    }}
};
```

### Explanation:
1. **"name": "John"** - This is a key-value pair where the key is `"name"` (a string), and the value is `"John"` (a string).
2. **"age": 30** - This is a key-value pair where the key is `"age"` (a string), and the value is `30` (an integer).
3. **"isStudent": false** - This is a key-value pair where the key is `"isStudent"` (a string), and the value is `false` (a boolean).
4. **"courses": ["math", "science"]** - This is a key-value pair where the key is `"courses"` (a string), and the value is an **array** containing two strings `"math"` and `"science"`.
5. **"address": {...}** - This is a key-value pair where the key is `"address"` (a string), and the value is another JSON object (dictionary) that contains two more key-value pairs:
   - **"city": "New York"** - This is a key-value pair inside the `address` object where the key is `"city"` (a string), and the value is `"New York"` (a string).
   - **"zip": "10001"** - This is a key-value pair inside the `address` object where the key is `"zip"` (a string), and the value is `"10001"` (a string).

### 2. **nlohmann::json Basics**

To use the `nlohmann::json` library in C++, you need to include the header and link the library (you can install it through a package manager or include the single header file).

#### Include the header:
```cpp
#include <nlohmann/json.hpp>
using json = nlohmann::json;
```

#### Creating a JSON Object in C++:
You can create JSON objects directly in C++ using the `nlohmann::json` library:

```cpp
json j = {
    {"name", "John"},
    {"age", 30},
    {"isStudent", false},
    {"courses", {"math", "science"}},
    {"address", {
        {"city", "New York"},
        {"zip", "10001"}
    }}
};
```

This corresponds to the JSON object shown above.

### 3. **Parsing and Serializing JSON**

- **Parsing a JSON string:**
  You can create a JSON object by parsing a string:
  
  ```cpp
  std::string jsonString = R"({"name": "John", "age": 30, "isStudent": false})";
  json j = json::parse(jsonString);
  ```

- **Serializing a JSON object to a string:**
  You can serialize a `json` object back to a string:
  
  ```cpp
  std::string serialized = j.dump();  // Default dumps with no formatting
  std::cout << serialized << std::endl;
  ```

  If you want pretty-printed output with indentation:
  
  ```cpp
  std::string prettySerialized = j.dump(4);  // 4 spaces indentation
  std::cout << prettySerialized << std::endl;
  ```

### 4. **Accessing and Modifying JSON**

- **Accessing elements:**
  You can access elements in a JSON object using either the `[]` operator or the `.at()` method.
  
  ```cpp
  std::string name = j["name"];
  int age = j["age"];
  bool isStudent = j["isStudent"];
  std::string city = j["address"]["city"];
  ```

  The `at()` method can throw an exception if the key does not exist, making it safer than `[]` in some contexts:
  
  ```cpp
  try {
      std::string zip = j.at("address").at("zip");
  } catch (json::out_of_range& e) {
      std::cerr << "Key not found: " << e.what() << std::endl;
  }
  ```

- **Modifying elements:**
  You can modify values by reassigning them:
  
  ```cpp
  j["age"] = 31;  // Modify age
  j["address"]["city"] = "Boston";  // Modify city
  ```

- **Adding new elements:**
  New elements can be added dynamically:
  
  ```cpp
  j["email"] = "john.doe@example.com";
  ```

- **Removing elements:**
  You can remove elements using the `erase` method:
  
  ```cpp
  j.erase("email");
  ```

### 5. **Iterating over JSON Objects and Arrays**

- **Iterating over JSON objects:**
  You can iterate over key-value pairs in a JSON object using a range-based for loop:
  
  ```cpp
  for (auto& el : j.items()) {
      std::cout << el.key() << ": " << el.value() << std::endl;
  }
  ```

- **Iterating over JSON arrays:**
  If you have a JSON array, you can iterate over the elements:
  
  ```cpp
  json courses = j["courses"];
  for (auto& course : courses) {
      std::cout << course << std::endl;
  }
  ```

### 6. **JSON and STL Compatibility**

`nlohmann::json` is compatible with standard C++ containers like `std::vector`, `std::map`, and `std::unordered_map`.

- **JSON to STL containers:**
  
  ```cpp
  std::vector<std::string> courses = j["courses"];
  std::map<std::string, std::string> address = j["address"];
  ```

- **STL containers to JSON:**
  
  ```cpp
  std::vector<int> numbers = {1, 2, 3, 4};
  json jNumbers = numbers;
  
  std::map<std::string, std::string> address = {{"city", "New York"}, {"zip", "10001"}};
  json jAddress = address;
  ```

### 7. **Handling Exceptions**

The library provides exception handling for various scenarios:
- **parsing_error**: Thrown when parsing fails.
- **type_error**: Thrown when a type mismatch occurs (e.g., accessing a string as an integer).
- **out_of_range**: Thrown when accessing a key or array index that doesn't exist.

Example of handling an exception:
```cpp
try {
    int age = j.at("age").get<int>();
} catch (json::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
}
```

### 8. **Working with JSON Arrays**

You can also represent JSON arrays directly:

```cpp
json arr = json::array();
arr.push_back("first");
arr.push_back(2);
arr.push_back(true);

// Accessing elements
std::string firstElement = arr[0];
int secondElement = arr[1];
```

### 9. **Checking Types**

You can check the type of a JSON element before accessing it:

```cpp
if (j["age"].is_number()) {
    std::cout << "Age is a number" << std::endl;
}
if (j["name"].is_string()) {
    std::cout << "Name is a string" << std::endl;
}
```

### 10. **Merging and Combining JSON Objects**

You can merge two JSON objects using the `merge_patch` function:

```cpp
json j1 = {{"name", "John"}, {"age", 30}};
json j2 = {{"age", 31}, {"city", "New York"}};

j1.merge_patch(j2);
std::cout << j1.dump(4) << std::endl;
```

### 11. **Saving and Loading JSON from Files**

- **Saving JSON to a file:**

```cpp
std::ofstream file("output.json");
file << j.dump(4);  // Write formatted JSON to file
file.close();
```

- **Loading JSON from a file:**

```cpp
std::ifstream file("input.json");
json j;
file >> j;  // Read JSON from file
```

## JSON Schema
JSON Schema is a powerful tool for validating the structure and content of JSON data. It provides a way to define the expected format of JSON documents, including the types of values, required properties, and other constraints. This ensures that the JSON data adheres to a specified schema, which is useful for data validation, documentation, and interoperability between systems.

Here is a simple example to illustrate how JSON Schema works:

### Example JSON Schema

```json
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "title": "Person",
  "type": "object",
  "properties": {
    "name": {
      "type": "string"
    },
    "age": {
      "type": "integer",
      "minimum": 0
    },
    "email": {
      "type": "string",
      "format": "email"
    }
  },
  "required": ["name", "age"]
}
```

### Explanation

- **`$schema`**: Specifies the version of the JSON Schema standard being used.
- **`title`**: A title for the schema.
- **`type`**: Specifies that the root of the JSON data should be an object.
- **`properties`**: Defines the properties of the object and their respective types and constraints:
  - **`name`**: A string property.
  - **`age`**: An integer property with a minimum value of 0.
  - **`email`**: A string property that must be a valid email format.
- **`required`**: Specifies that the `name` and `age` properties are required.

### Example JSON Data

Here is an example of a valid JSON document according to the above schema:

```json
{
  "name": "John Doe",
  "age": 30,
  "email": "john.doe@example.com"
}
```

### Invalid JSON Data Examples

Here are some examples of JSON documents that would be considered invalid according to the schema:

1. Missing required property:
   ```json
   {
     "name": "John Doe",
     "email": "john.doe@example.com"
   }
   ```
   - This is invalid because the `age` property is missing.

2. Incorrect type:
   ```json
   {
     "name": "John Doe",
     "age": "thirty",
     "email": "john.doe@example.com"
   }
   ```
   - This is invalid because the `age` property should be an integer, not a string.

3. Invalid email format:
   ```json
   {
     "name": "John Doe",
     "age": 30,
     "email": "john.doe"
   }
   ```
   - This is invalid because the `email` property is not in a valid email format.

JSON Schema provides a robust framework to ensure that JSON data is well-formed and adheres to specific rules, making it an essential tool for data validation and API development.



[code](../src/json_example.cpp)  
