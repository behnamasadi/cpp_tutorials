#include <bitset>
#include <iostream>

// A numeral system (base) is how we write a number.
// The doc covers: decimal (base 10), binary (base 2),
// octal (base 8), and hexadecimal (base 16), plus the
// std::dec / std::oct / std::hex stream manipulators.

void decimalLiteral() {
  std::cout << "--- decimal (base 10) ---\n";
  int decimalNumber = 123;
  std::cout << "decimalNumber = " << decimalNumber << "\n";
}

void binaryLiteral() {
  std::cout << "--- binary (base 2, prefix 0b) ---\n";
  int binaryNumber = 0b1101;
  std::cout << "binaryNumber (printed as decimal) = " << binaryNumber << "\n";
}

void octalLiteral() {
  std::cout << "--- octal (base 8, prefix 0) ---\n";
  int octalNumber = 075;
  std::cout << "octalNumber (printed as decimal) = " << octalNumber << "\n";
}

void hexLiteral() {
  std::cout << "--- hexadecimal (base 16, prefix 0x) ---\n";
  int hexNumber = 0x1A;
  std::cout << "hexNumber (printed as decimal) = " << hexNumber << "\n";
}

void streamManipulators() {
  std::cout << "--- std::dec / std::oct / std::hex ---\n";
  int number = 13;
  std::cout << "decimal:     " << std::dec << number << "\n";
  std::cout << "octal:       " << std::oct << number << "\n";
  std::cout << "hexadecimal: " << std::hex << number << "\n";
  std::cout << std::dec; // reset back to decimal
}

void binaryPrinting() {
  std::cout << "--- printing in binary with std::bitset ---\n";
  int number = 13;
  std::cout << "binary: " << std::bitset<8>(number) << "\n";
}

int main() {
  decimalLiteral();
  binaryLiteral();
  octalLiteral();
  hexLiteral();
  streamManipulators();
  binaryPrinting();
  return 0;
}
