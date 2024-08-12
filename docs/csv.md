## fast-cpp-csv-parser

Below is an example of how to use the [fast-cpp-csv-parser](https://github.com/ben-strasser/fast-cpp-csv-parser) to read a CSV file, allowing for ignoring missing or extra columns, and mapping the data into appropriate data types.

First, ensure you have the `fast-cpp-csv-parser` library. You can download it from its [GitHub repository](https://github.com/ben-strasser/fast-cpp-csv-parser).

Here's the example code:

```cpp
#include <iostream>
#include <string>
#include "csv.h"

// Define a structure to hold the data
struct Position {
    double x;
    double y;
    double z;
};

int main() {
    try {
        // Define the file path
        std::string filepath = "yourfile.csv";

        // Create a CSV reader with the specified columns
        io::CSVReader<3, io::trim_chars<' '>, io::double_quote_escape<',', '\"'>> in(filepath);
        in.read_header(io::ignore_missing_column | io::ignore_extra_column, "x", "y", "z");

        // Structure to hold the data
        Position position;

        // Read the rows and map the data to the structure
        while (in.read_row(position.x, position.y, position.z)) {
            // Process the data (for example, print it)
            std::cout << "x: " << position.x << ", y: " << position.y << ", z: " << position.z << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
```





 Below is the modified code with additional exception handling for `CSVError::FailedParse` and other general exceptions:

```cpp
#include <iostream>
#include <string>
#include "csv.h"

// Define a structure to hold the data
struct Position {
    double x;
    double y;
    double z;
};

int main() {
    try {
        // Define the file path
        std::string filepath = "yourfile.csv";

        // Create a CSV reader with the specified columns
        io::CSVReader<3, io::trim_chars<' '>, io::double_quote_escape<',', '\"'>> in(filepath);
        in.read_header(io::ignore_missing_column | io::ignore_extra_column, "x", "y", "z");

        // Structure to hold the data
        Position position;

        // Read the rows and map the data to the structure
        while (in.read_row(position.x, position.y, position.z)) {
            // Process the data (for example, print it)
            std::cout << "x: " << position.x << ", y: " << position.y << ", z: " << position.z << std::endl;
        }
    } catch (const io::error::can_not_open_file &e) {
        std::cerr << "Error: Can't open file. Reason: " << e.what() << std::endl;
    } catch (const io::error::base &e) {
        std::cerr << "Error: CSV parsing error. Reason: " << e.what() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
```

### Explanation:
- We've added specific `catch` blocks for handling `io::error::can_not_open_file` and `io::error::base`, which are exceptions that can be thrown by the CSV library.
- `io::error::can_not_open_file` is thrown when the file cannot be opened, typically due to non-existence or insufficient permissions.
- `io::error::base` is a base class for other CSV parsing errors. We catch this to handle general CSV parsing errors.
- Other exceptions are caught by the catch block `catch (const std::exception &e)` to handle any unforeseen exceptions that might occur during file reading or processing.

### Notes:
- Adjust the file path (`"yourfile.csv"`) to point to the actual CSV file you want to parse.
- You may handle these exceptions differently based on your application's requirements. For example, you could log the errors or prompt the user to provide a valid file path.
- Make sure you have the necessary permissions to read the file specified.
- Compile the code using the same command provided in the previous response.




##  RapidCSV

Below is an example of how to use the RapidCSV library in C++ to handle CSV files where the user can specify the columns, and the program can handle missing or extra columns, and map data into appropriate data types.

First, make sure you have the RapidCSV library. You can download it from its [GitHub repository](https://github.com/d99kris/rapidcsv).

Here's the example code:

```cpp
#include <iostream>
#include <string>
#include <vector>
#include "rapidcsv.h"

int main()
{
    try
    {
        // Load CSV file with options to ignore missing or extra columns
        rapidcsv::Document doc("yourfile.csv", rapidcsv::LabelParams(0, -1), rapidcsv::SeparatorParams(),
                               rapidcsv::ConverterParams(), rapidcsv::LineReaderParams(), 
                               rapidcsv::IOParams(rapidcsv::io::ignore_missing_column | rapidcsv::io::ignore_extra_column));

        // Specify the columns you are interested in
        std::vector<std::string> columns = { "Column1", "Column2", "Column3" };

        // Check and handle missing columns
        for (const auto& col : columns)
        {
            if (doc.HasColumn(col))
            {
                std::cout << "Processing column: " << col << std::endl;

                // Assuming the column contains integers, otherwise change to the appropriate type
                std::vector<int> data = doc.GetColumn<int>(col);
                
                for (const auto& value : data)
                {
                    std::cout << value << " ";
                }
                std::cout << std::endl;
            }
            else
            {
                std::cout << "Column " << col << " is missing in the CSV file." << std::endl;
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
```

### Explanation:
1. **Include RapidCSV Header**: Include the necessary RapidCSV header file.
2. **Document Initialization**: Load the CSV file with `rapidcsv::Document` while specifying options to ignore missing or extra columns.
3. **Specify Columns**: Define a vector of columns that you are interested in processing.
4. **Check Columns**: Iterate through the specified columns and check if each column exists using `doc.HasColumn(col)`.
5. **Handle Data**: If the column exists, retrieve the data and process it. In this example, the data is assumed to be integers. If your data is of different types, adjust the `GetColumn` template parameter accordingly (e.g., `GetColumn<std::string>`).
6. **Error Handling**: Catch any exceptions that might occur during the CSV parsing and handling process.

### Notes:
- Ensure the file `yourfile.csv` exists in the same directory as your executable or provide the correct path.
- Adjust the column names and data types as needed for your specific use case.
- This example assumes that RapidCSV is properly included and linked in your project.

To compile the code, you may use a command similar to this if you're using g++:

```sh
g++ -std=c++17 -o your_program your_program.cpp
```

Make sure you have C++17 or later enabled due to some modern C++ features used by RapidCSV.


[code](../src/third_party_tools/csv)

