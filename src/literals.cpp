#include <iostream>
#include <string>

// Literals are fixed values written directly in code.
// The doc covers: integer, floating-point, char, string, boolean,
// suffix-modified literals, and user-defined literals (UDL).

void integerLiterals() {
  std::cout << "--- integer literals ---\n";
  int age = 30;
  int daysInWeek = 7;
  std::cout << "age = " << age << "\n";
  std::cout << "daysInWeek = " << daysInWeek << "\n";
}

void floatingPointLiterals() {
  std::cout << "--- floating-point literals ---\n";
  double price = 99.99;
  double productPrice = 29.95;
  std::cout << "price = " << price << "\n";
  std::cout << "productPrice = " << productPrice << "\n";
}

void charLiterals() {
  std::cout << "--- char literals ---\n";
  char grade = 'A';
  char testGrade = 'B';
  std::cout << "grade = " << grade << "\n";
  std::cout << "testGrade = " << testGrade << "\n";
}

void stringLiterals() {
  std::cout << "--- string literals ---\n";
  std::string greeting = "Hello, World!";
  std::string welcomeMessage = "Welcome to our app!";
  std::cout << "greeting = " << greeting << "\n";
  std::cout << "welcomeMessage = " << welcomeMessage << "\n";
}

void booleanLiterals() {
  std::cout << "--- boolean literals ---\n";
  bool isAdult = true;
  bool hasMilk = false;
  std::cout << "isAdult = " << isAdult << "\n";
  std::cout << "hasMilk = " << hasMilk << "\n";
}

void coffeeShopExample() {
  std::cout << "--- coffee shop example ---\n";
  int numberOfCups = 1;          // integer literal
  double pricePerCup = 2.50;     // floating-point literal
  char size = 'M';               // character literal
  std::string coffeeType = "Espresso"; // string literal
  bool hasMilk = false;          // boolean literal
  std::cout << "cups = " << numberOfCups << "\n";
  std::cout << "price = " << pricePerCup << "\n";
  std::cout << "size = " << size << "\n";
  std::cout << "type = " << coffeeType << "\n";
  std::cout << "hasMilk = " << hasMilk << "\n";
}

void suffixModifiedIntegerLiterals() {
  std::cout << "--- suffix-modified integer literals ---\n";
  int x = 42;                    // no suffix, int
  unsigned int y = 42u;          // u or U: unsigned
  long z = 42L;                  // l or L: long
  long long a = 42LL;            // ll or LL: long long
  unsigned long b = 42UL;        // combine: unsigned long
  unsigned long long c = 42ULL;  // combine: unsigned long long
  std::cout << "x = " << x << "\n";
  std::cout << "y = " << y << "\n";
  std::cout << "z = " << z << "\n";
  std::cout << "a = " << a << "\n";
  std::cout << "b = " << b << "\n";
  std::cout << "c = " << c << "\n";
}

void suffixModifiedFloatLiterals() {
  std::cout << "--- suffix-modified floating-point literals ---\n";
  float f = 42.0f;               // f or F: float
  double d = 42.0;               // no suffix, double
  long double ld = 42.0L;        // l or L: long double
  std::cout << "f = " << f << "\n";
  std::cout << "d = " << d << "\n";
  std::cout << "ld = " << ld << "\n";
}

// User-defined literal: convert miles to kilometers
long double operator"" _km(long double miles) { return miles * 1.60934; }

// User-defined literal: convert light years to kilometers
long double operator"" _ly(long double lightYears) {
  return lightYears * 9.461e+15;
}

void userDefinedLiterals() {
  std::cout << "--- user-defined literals ---\n";
  auto distance = 5.0_km;
  std::cout << "5.0_km = " << distance << " km\n";

  long long int distanceToStar = 1234567890123LL;
  auto proximaCentauriDistance = 4.24_ly;
  std::cout << "distanceToStar = " << distanceToStar << "\n";
  std::cout << "4.24_ly = " << proximaCentauriDistance << " km\n";
}

int main() {
  integerLiterals();
  floatingPointLiterals();
  charLiterals();
  stringLiterals();
  booleanLiterals();
  coffeeShopExample();
  suffixModifiedIntegerLiterals();
  suffixModifiedFloatLiterals();
  userDefinedLiterals();
  return 0;
}
