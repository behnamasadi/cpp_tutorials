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
