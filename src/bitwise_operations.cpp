// Bitwise operators and the C++17/20 functional wrappers (std::bit_xor etc.).
//
// Operator cheat sheet:
//   &    bitwise AND                 &=   AND assignment
//   |    bitwise inclusive OR        |=   OR  assignment
//   ^    bitwise XOR                 ^=   XOR assignment
//   <<   left shift                  <<=  left  shift assignment
//   >>   right shift                 >>=  right shift assignment
//   ~    bitwise NOT (one's complement, unary)
//
// Bitwise vs. logical:
//   a & b   <-> a && b
//   a | b   <-> a || b
//   a ^ b   <-> a != b
//   ~a      <-> !a
//
// Ref: https://www.geeksforgeeks.org/bitwise-algorithms/

#include <bitset>
#include <cstdint>
#include <functional>
#include <iostream>

// Power-of-two via left shift: number << (power - 1) == number * 2^(power-1).
// Useful when configuring DMA buffer sizes or scaling encoder counts where
// the result must be a power of two anyway.
void bitwiseshiftPOW2(int number = 2, int power = 5) {
  int result = number << (power - 1);
  std::cout << number << " << " << (power - 1) << " = " << result << '\n';
}

// Cheapest odd/even test: look at the least significant bit. Comes up in
// tick-tock scheduling on a fixed-rate control loop.
void evenOdd(int num = 6) {
  int result = num & 1;
  std::cout << num << (result ? " is odd" : " is even") << '\n';
}

// Right shift discards low bits. `a >>= 2` mutates `a` in place.
void shiftBits() {
  int a = 13; // 1101
  int b = 2;
  int result = (a >> b); // 0011 == 3
  std::cout << "13 >> 2 = " << result << '\n';
  std::cout << "after a >>= 2, a = " << (a >>= 2) << '\n';
}

// Robotics example: a sensor status byte read off SPI. Use bitwise ops to
// inspect, set, clear, and toggle individual flag bits.
void sensor_status_register() {
  constexpr std::uint8_t READY    = 1 << 0; // 0b00000001
  constexpr std::uint8_t DATA_RDY = 1 << 1; // 0b00000010
  constexpr std::uint8_t OVERRUN  = 1 << 2; // 0b00000100
  constexpr std::uint8_t FAULT    = 1 << 7; // 0b10000000

  std::uint8_t status = 0;
  status |= READY;                            // raise READY
  status |= DATA_RDY;                         // raise DATA_RDY
  std::cout << "status     = " << std::bitset<8>(status) << '\n';

  if (status & DATA_RDY)
    std::cout << "  -> new sample available\n";

  status &= ~DATA_RDY;                        // clear DATA_RDY after reading
  std::cout << "after read = " << std::bitset<8>(status) << '\n';

  status ^= FAULT;                            // toggle FAULT bit
  std::cout << "toggle FLT = " << std::bitset<8>(status) << '\n';

  std::uint8_t inverted = ~status;            // one's complement
  std::cout << "~status    = " << std::bitset<8>(inverted) << '\n';
}

// std::bit_xor (and bit_and / bit_or / bit_not) are function objects that
// wrap the corresponding operator. They are useful with algorithms and
// templates because they can be passed around as callables, and they work
// uniformly across integral types.
void std_bit_xor_demo() {
  std::bit_xor<std::uint8_t> xor_op;
  std::bit_and<std::uint8_t> and_op;
  std::bit_or<std::uint8_t>  or_op;

  std::uint8_t a = 0b10110010;
  std::uint8_t b = 0b01011011;

  std::cout << "a       = " << std::bitset<8>(a) << '\n';
  std::cout << "b       = " << std::bitset<8>(b) << '\n';
  std::cout << "a ^ b   = " << std::bitset<8>(xor_op(a, b)) << '\n';
  std::cout << "a & b   = " << std::bitset<8>(and_op(a, b)) << '\n';
  std::cout << "a | b   = " << std::bitset<8>(or_op(a, b))  << '\n';

  // XOR with itself clears the value -- the classic "no temporary" swap
  // ingredient and a quick parity check used in CRC routines.
  std::cout << "a ^ a   = " << std::bitset<8>(xor_op(a, a)) << '\n';
}

int main() {
  std::cout << "--- bitwiseshiftPOW2 ---\n";
  bitwiseshiftPOW2();

  std::cout << "\n--- evenOdd ---\n";
  evenOdd(8);
  evenOdd(7);

  std::cout << "\n--- shiftBits ---\n";
  shiftBits();

  std::cout << "\n--- sensor status register ---\n";
  sensor_status_register();

  std::cout << "\n--- std::bit_xor ---\n";
  std_bit_xor_demo();
  return 0;
}
