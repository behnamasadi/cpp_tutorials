#include <cstdio>
#include <fstream>
#include <iostream>
#include <system_error>

void open_file(const std::string &filename, std::error_code &ec) {
  std::ifstream file(filename);
  if (!file) {
    ec = std::make_error_code(std::errc::no_such_file_or_directory);
    return;
  }
  // Process file...
  ec.clear(); // No error
}

void delete_file(const std::string &filename, std::error_code &ec) {
  if (std::remove(filename.c_str()) != 0) {
    ec = std::error_code(errno, std::system_category());
  } else {
    ec.clear(); // No error
  }
}

int main() {

  {
    std::error_code ec;
    open_file("non_existent_file.txt", ec);

    if (ec) {
      std::cout << "Error opening file: " << ec.message()
                << " (Error code: " << ec.value() << ")\n";
    } else {
      std::cout << "File opened successfully.\n";
    }
  }

  {
    std::error_code ec;
    delete_file("non_existent_file.txt", ec);

    if (ec) {
      std::cout << "Error deleting file: " << ec.message()
                << " (Error code: " << ec.value() << ")\n";
    } else {
      std::cout << "File deleted successfully.\n";
    }
  }

  return 0;
}