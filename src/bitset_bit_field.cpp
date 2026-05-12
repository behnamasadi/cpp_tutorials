// std::bitset and C bit fields, exercised side by side.
//
// Real-world framing: a motor-controller status register and a CAN frame flag
// byte. Both are natural fits for packed bit storage.
//
// Refs:
//   https://www.geeksforgeeks.org/c-bitset-interesting-facts/
//   https://www.geeksforgeeks.org/c-bitset-and-its-application/

#include <bitset>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

using namespace std;

// ---------------------------------------------------------------------------
// std::bitset demo
// ---------------------------------------------------------------------------
static void bitset_demo() {
  cout << "--- std::bitset demo ---\n";

  const int M = sizeof(int) * 8;

  // default constructor initializes with all bits 0
  bitset<M> bset1;

  // bset2 is initialized with bits of 20
  bitset<M> bset2(20);

  // bset3 is initialized with bits of a binary string
  bitset<M> bset3(string("1100"));

  // operator<< prints the exact bit representation
  cout << bset1 << endl; // 00000000000000000000000000000000
  cout << bset2 << endl; // 00000000000000000000000000010100
  cout << bset3 << endl; // 00000000000000000000000000001100
  cout << endl;

  // declaring set8 with capacity of 8 bits
  bitset<8> set8; // 00000000

  // setting individual bits via operator[]
  set8[1] = 1;       // 00000010
  set8[4] = set8[1]; // 00010010
  cout << set8 << endl;

  // count() returns number of set bits
  int numberof1 = set8.count();

  // size() returns the total bit capacity, so the difference gives zeros
  int numberof0 = set8.size() - numberof1;
  cout << set8 << " has " << numberof1 << " ones and " << numberof0
       << " zeros\n";

  // test(i) returns true if bit i is set
  cout << "bool representation of " << set8 << " : ";
  for (std::size_t i = 0; i < set8.size(); i++)
    cout << set8.test(i) << " ";
  cout << endl;

  // any() is true if at least one bit is set
  if (!set8.any())
    cout << "set8 has no bit set.\n";
  if (!bset1.any())
    cout << "bset1 has no bit set.\n";

  // none() is true if no bit is set
  if (!bset1.none())
    cout << "bset1 has some bit set\n";

  // set() with no argument sets every bit
  cout << set8.set() << endl;

  // set(pos, b) writes b at position pos
  cout << set8.set(4, 0) << endl;

  // set(pos) defaults b to 1
  cout << set8.set(4) << endl;

  // reset(pos) clears one bit, reset() clears all
  cout << set8.reset(2) << endl;
  cout << set8.reset() << endl;

  // flip() inverts bits (whole bitset or a single position)
  cout << set8.flip(2) << endl;
  cout << set8.flip() << endl;

  // Converting a decimal value into a bitset and back
  int num = 100;
  bitset<8> set9(num);
  cout << "\nDecimal number: " << set9.to_ulong()
       << "  Binary equivalent: " << set9 << "\n\n";
}

// ---------------------------------------------------------------------------
// Bit field demo
// ---------------------------------------------------------------------------
// Motor-controller status word laid out as a packed struct. Each field
// occupies only the bits it needs, so the whole status fits in 16 bits and
// matches the wire format the driver reads from the CAN bus.
struct MotorStatus {
  std::uint16_t enabled    : 1; // power stage on
  std::uint16_t fault      : 1; // any latched fault
  std::uint16_t overtemp   : 1; // thermal warning
  std::uint16_t mode       : 3; // 0..7, e.g. torque/velocity/position
  std::uint16_t error_code : 5; // 0..31 diagnostic code
  std::uint16_t reserved   : 5;
};

// CAN frame flag byte: bits packed into a single octet on the wire.
struct CanFlags {
  std::uint8_t rtr      : 1; // remote transmission request
  std::uint8_t extended : 1; // 29-bit identifier vs 11-bit
  std::uint8_t error    : 1;
  std::uint8_t dlc      : 4; // data length 0..15
  std::uint8_t reserved : 1;
};

static void bit_field_demo() {
  cout << "--- bit field demo ---\n";

  // The whole motor status fits in 2 bytes thanks to bit fields.
  cout << "sizeof(MotorStatus) = " << sizeof(MotorStatus) << " bytes\n";
  cout << "sizeof(CanFlags)    = " << sizeof(CanFlags) << " byte(s)\n";

  MotorStatus s{};
  s.enabled = 1;
  s.fault = 0;
  s.overtemp = 1;
  s.mode = 2;         // e.g. velocity mode
  s.error_code = 17;  // arbitrary diagnostic

  cout << "enabled=" << s.enabled << " fault=" << s.fault
       << " overtemp=" << s.overtemp << " mode=" << s.mode
       << " error_code=" << s.error_code << "\n";

  // Reinterpret the same bytes as a 16-bit word to see the wire layout.
  std::uint16_t raw;
  std::memcpy(&raw, &s, sizeof(raw));
  cout << "raw status word = 0x" << std::hex << raw << std::dec
       << "  bits = " << bitset<16>(raw) << "\n";

  CanFlags f{};
  f.extended = 1;
  f.dlc = 8;
  cout << "CAN flags: extended=" << +f.extended << " dlc=" << +f.dlc << "\n";
}

int main() {
  bitset_demo();
  bit_field_demo();
  return 0;
}
