#include "arguments_parser.hpp"
#include <iostream>
#include <string.h>
#include <string>

enum MY_APP { SUCCESS = 0, NOINPUT_FILE, USAGE_ERORR, NO_OUTPUT_FILE };

int main(int argc, char **argv) {
  ArgumentsParser input(argc, argv);
  if (input.argExists("-h") || input.argExists("--help") || argc == 1) {
    std::cerr << "usage error: please specify input and output file, -i "
                 "<input-mesh>  -o <output-mesh>"
              << std::endl;
    return MY_APP::USAGE_ERORR;
  }
  std::string inputFilename = input.getArg("-i");
  if (inputFilename.empty()) {
    std::cerr << "no intput file is given, please specify the input file, -i "
                 "<input-mesh>"
              << std::endl;
    return MY_APP::NOINPUT_FILE;
  }

  std::string outputFilename = input.getArg("-o");
  if (outputFilename.empty()) {
    std::cerr << "no output file is given, please specify the output file, -o "
                 "<output-mesh>"
              << std::endl;
    return MY_APP::NO_OUTPUT_FILE;
  }
}
