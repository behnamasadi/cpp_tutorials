#define CSV_IO_NO_THREAD
#include <csv.h>
#include <iostream>

int main(int argc, char **argv) {
  std::string csv_file_path =
      CMAKE_CURRENT_SOURCE_DIR + std::string("/src/data/data.csv");

  std::cout << "reading csv file at: " << csv_file_path << std::endl;

  io::CSVReader<2> in(csv_file_path);

  in.read_header(io::ignore_extra_column, "x", "y");
  std::string vendor;
  int x;
  double y;

  while (in.read_row(x, y)) {
    std::cout << "x: " << x << " y: " << y << std::endl;
  }
}
