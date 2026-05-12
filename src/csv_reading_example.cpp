// Demo of fast-cpp-csv-parser, matching docs/csv.md.
//
// We write a tiny CSV to /tmp at runtime, then read it back twice:
//   1. With a plain try/catch (first snippet in the doc).
//   2. With specific io::error::* catch blocks (second snippet in the doc).

#define CSV_IO_NO_THREAD
#include <csv.h>

#include <fstream>
#include <iostream>
#include <string>

// Define a structure to hold the data
struct Position {
  double x;
  double y;
  double z;
};

static const std::string filepath = "/tmp/positions.csv";

void write_sample_csv() {
  std::ofstream out(filepath);
  out << "x,y,z\n";
  out << "1.0,2.0,3.0\n";
  out << "4.5,5.5,6.5\n";
  out << "7.0,8.0,9.0\n";
}

void demo_basic_try_catch() {
  std::cout << "--- demo 1: basic try/catch ---\n";
  try {
    io::CSVReader<3, io::trim_chars<' '>, io::double_quote_escape<',', '\"'>>
        in(filepath);
    in.read_header(io::ignore_missing_column | io::ignore_extra_column, "x",
                   "y", "z");

    Position position;
    while (in.read_row(position.x, position.y, position.z)) {
      std::cout << "x: " << position.x << ", y: " << position.y
                << ", z: " << position.z << "\n";
    }
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
}

void demo_specific_exceptions() {
  std::cout << "--- demo 2: specific exception types ---\n";
  try {
    io::CSVReader<3, io::trim_chars<' '>, io::double_quote_escape<',', '\"'>>
        in(filepath);
    in.read_header(io::ignore_missing_column | io::ignore_extra_column, "x",
                   "y", "z");

    Position position;
    while (in.read_row(position.x, position.y, position.z)) {
      std::cout << "x: " << position.x << ", y: " << position.y
                << ", z: " << position.z << "\n";
    }
  } catch (const io::error::can_not_open_file &e) {
    std::cerr << "Error: Can't open file. Reason: " << e.what() << "\n";
  } catch (const io::error::base &e) {
    std::cerr << "Error: CSV parsing error. Reason: " << e.what() << "\n";
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
}

int main() {
  write_sample_csv();
  demo_basic_try_catch();
  demo_specific_exceptions();
  return 0;
}
