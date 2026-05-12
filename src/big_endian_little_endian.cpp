// Companion source for docs/big-endian_little-endian.md
//
// Demonstrates byte-order techniques referenced in the doc:
//   1. Runtime endian detection (the classic pointer-cast trick).
//   2. Compile-time endian detection via std::endian (C++20).
//   3. Printing raw memory bytes vs. logical big-endian bytes.
//   4. Manual byte swapping for 16/32/64-bit words.
//   5. std::byteswap (C++23) when available, else a manual fallback.
//   6. Robotics-flavored examples: CAN frame ID + payload, network
//      protocol header on the wire, and an embedded sensor word.
//
// Build target: big_endian_little_endian (C++20).

#include <array>
#include <bit>          // std::endian, std::bit_cast
#include <cstdint>
#include <cstring>      // std::memcpy
#include <iomanip>
#include <iostream>
#include <span>
#include <string_view>

#include <version>      // feature-test macros (__cpp_lib_byteswap)

// --- Helpers -----------------------------------------------------------------

// Print bytes of any trivially-copyable value as stored in memory.
template <class T>
void print_memory_bytes(const T& value, std::string_view label) {
    const auto* p = reinterpret_cast<const unsigned char*>(&value);
    std::cout << label << " (in-memory): ";
    std::cout << std::hex << std::setfill('0');
    for (std::size_t i = 0; i < sizeof(T); ++i) {
        std::cout << std::setw(2) << static_cast<int>(p[i]) << ' ';
    }
    std::cout << std::dec << std::setfill(' ') << '\n';
}

// Print bytes from MSB to LSB (logical big-endian view) regardless of host.
template <class T>
void print_logical_be(const T& value, std::string_view label) {
    const auto* p = reinterpret_cast<const unsigned char*>(&value);
    std::cout << label << " (logical BE): ";
    std::cout << std::hex << std::setfill('0');
    if constexpr (std::endian::native == std::endian::little) {
        for (std::size_t i = sizeof(T); i-- > 0;) {
            std::cout << std::setw(2) << static_cast<int>(p[i]) << ' ';
        }
    } else {
        for (std::size_t i = 0; i < sizeof(T); ++i) {
            std::cout << std::setw(2) << static_cast<int>(p[i]) << ' ';
        }
    }
    std::cout << std::dec << std::setfill(' ') << '\n';
}

// Classic runtime check (mirrors the doc's isBigEndian()).
bool runtime_is_big_endian() {
    std::uint32_t test = 1;
    const auto* byte = reinterpret_cast<const unsigned char*>(&test);
    return byte[0] == 0;
}

// Portable byte swaps. Used as a fallback when std::byteswap is unavailable.
constexpr std::uint16_t bswap16(std::uint16_t v) noexcept {
    return static_cast<std::uint16_t>((v << 8) | (v >> 8));
}
constexpr std::uint32_t bswap32(std::uint32_t v) noexcept {
    return  ((v & 0x000000FFu) << 24) |
            ((v & 0x0000FF00u) << 8)  |
            ((v & 0x00FF0000u) >> 8)  |
            ((v & 0xFF000000u) >> 24);
}
constexpr std::uint64_t bswap64(std::uint64_t v) noexcept {
    return  ((v & 0x00000000000000FFull) << 56) |
            ((v & 0x000000000000FF00ull) << 40) |
            ((v & 0x0000000000FF0000ull) << 24) |
            ((v & 0x00000000FF000000ull) << 8)  |
            ((v & 0x000000FF00000000ull) >> 8)  |
            ((v & 0x0000FF0000000000ull) >> 24) |
            ((v & 0x00FF000000000000ull) >> 40) |
            ((v & 0xFF00000000000000ull) >> 56);
}

// host <-> big-endian conversion (no-op on big-endian hosts).
template <class T>
constexpr T host_to_be(T v) noexcept {
    static_assert(std::is_integral_v<T>, "integral only");
    if constexpr (std::endian::native == std::endian::big) {
        return v;
    } else {
        if constexpr (sizeof(T) == 1) return v;
        else if constexpr (sizeof(T) == 2) return static_cast<T>(bswap16(static_cast<std::uint16_t>(v)));
        else if constexpr (sizeof(T) == 4) return static_cast<T>(bswap32(static_cast<std::uint32_t>(v)));
        else if constexpr (sizeof(T) == 8) return static_cast<T>(bswap64(static_cast<std::uint64_t>(v)));
        else static_assert(sizeof(T) == 0, "unsupported size");
    }
}

template <class T>
constexpr T be_to_host(T v) noexcept { return host_to_be(v); }  // symmetric

void banner(std::string_view title) {
    std::cout << "\n=== " << title << " ===\n";
}

// --- Demos -------------------------------------------------------------------

void demo_runtime_detection() {
    banner("1. Runtime endian detection (doc's classic trick)");
    std::cout << "runtime_is_big_endian(): "
              << (runtime_is_big_endian() ? "Big-endian" : "Little-endian") << '\n';
}

void demo_compile_time_detection() {
    banner("2. Compile-time detection via std::endian (C++20)");
    if constexpr (std::endian::native == std::endian::little) {
        std::cout << "std::endian::native == little\n";
    } else if constexpr (std::endian::native == std::endian::big) {
        std::cout << "std::endian::native == big\n";
    } else {
        std::cout << "std::endian::native == mixed/unknown\n";
    }
}

void demo_byte_layout() {
    banner("3. Byte layout of 0x12345678 (doc's printBytes/printBigEndianBytes)");
    std::uint32_t number = 0x12345678u;
    std::cout << "value = 0x" << std::hex << number << std::dec << '\n';
    print_memory_bytes(number,  "  number");
    print_logical_be (number,  "  number");
}

void demo_byteswap() {
    banner("4. Byte swapping (manual + std::byteswap if available)");
    std::uint32_t v32 = 0xDEADBEEFu;
    std::uint16_t v16 = 0xABCDu;
    std::uint64_t v64 = 0x0123456789ABCDEFull;

    std::cout << std::hex << std::setfill('0');
    std::cout << "manual bswap16(0x" << std::setw(4) << v16 << ") = 0x"
              << std::setw(4) << bswap16(v16) << '\n';
    std::cout << "manual bswap32(0x" << std::setw(8) << v32 << ") = 0x"
              << std::setw(8) << bswap32(v32) << '\n';
    std::cout << "manual bswap64(0x" << std::setw(16) << v64 << ") = 0x"
              << std::setw(16) << bswap64(v64) << '\n';
    std::cout << std::dec << std::setfill(' ');

#if defined(__cpp_lib_byteswap) && __cpp_lib_byteswap >= 202110L
    std::cout << "std::byteswap available (__cpp_lib_byteswap = "
              << __cpp_lib_byteswap << ")\n";
    std::cout << std::hex << std::setfill('0');
    std::cout << "  std::byteswap<uint32_t>(0xDEADBEEF) = 0x"
              << std::setw(8) << std::byteswap(v32) << '\n';
    std::cout << std::dec << std::setfill(' ');
#else
    std::cout << "std::byteswap NOT available on this toolchain (needs C++23).\n"
              << "  Falling back to manual bswap32/16/64 above.\n";
#endif
}

// --- Robotics framing --------------------------------------------------------

// A CAN 2.0B frame layout (simplified): 29-bit extended ID + up to 8 data bytes.
// The CAN bus itself transmits bits, but when a controller hands you a frame
// struct you often need to interpret the payload as a multi-byte integer with
// a documented endianness (CANopen is little-endian, J1939 signals are LE too,
// but many OEM protocols pack big-endian "Motorola" signals).
struct CanFrame {
    std::uint32_t id;                   // host order
    std::uint8_t  dlc;                  // data length code, 0..8
    std::array<std::uint8_t, 8> data;   // raw payload bytes
};

void demo_can_frame() {
    banner("5. Robotics: CAN frame with a big-endian (Motorola) signal");
    // Suppose byte 0..3 of the payload carry a 32-bit wheel-encoder count in
    // big-endian order, byte 4..5 a 16-bit motor temperature in BE, deg*100.
    std::uint32_t encoder_counts = 0x0001F4A0u;   // 128'160 counts
    std::uint16_t motor_temp_cdg = 0x1F40u;       //  80.00 deg C  (8000)

    CanFrame f{};
    f.id  = 0x18FF50E5u;  // J1939-style 29-bit extended ID
    f.dlc = 6;

    // Pack signals into the wire (big-endian).
    std::uint32_t enc_be  = host_to_be(encoder_counts);
    std::uint16_t temp_be = host_to_be(motor_temp_cdg);
    std::memcpy(f.data.data() + 0, &enc_be,  sizeof(enc_be));
    std::memcpy(f.data.data() + 4, &temp_be, sizeof(temp_be));

    std::cout << "CAN id = 0x" << std::hex << f.id << std::dec
              << ", dlc = " << static_cast<int>(f.dlc) << '\n';
    std::cout << "payload bytes (on wire): " << std::hex << std::setfill('0');
    for (std::uint8_t i = 0; i < f.dlc; ++i) {
        std::cout << std::setw(2) << static_cast<int>(f.data[i]) << ' ';
    }
    std::cout << std::dec << std::setfill(' ') << '\n';

    // Decode side: pull bytes off the wire and convert back to host order.
    std::uint32_t enc_wire = 0;
    std::uint16_t temp_wire = 0;
    std::memcpy(&enc_wire,  f.data.data() + 0, sizeof(enc_wire));
    std::memcpy(&temp_wire, f.data.data() + 4, sizeof(temp_wire));
    std::cout << "decoded encoder counts = " << be_to_host(enc_wire)  << '\n';
    std::cout << "decoded motor temp     = "
              << (be_to_host(temp_wire) / 100.0) << " degC\n";
}

void demo_network_header() {
    banner("6. Robotics: network protocol header (network byte order = BE)");
    // ROS 1 TCPROS, custom UDP telemetry, and most IP-layer protocols use
    // big-endian on the wire. htons/htonl exist precisely for this.
    std::uint16_t host_port = 11311;      // ROS master default port
    std::uint16_t wire_port = host_to_be(host_port);

    std::cout << "host port  : " << host_port
              << "  (0x" << std::hex << host_port << std::dec << ")\n";
    std::cout << "wire port  : 0x" << std::hex << wire_port << std::dec << '\n';
    print_memory_bytes(host_port, "  host_port");
    print_memory_bytes(wire_port, "  wire_port");
}

void demo_embedded_sensor_word() {
    banner("7. Robotics: embedded sensor word with std::bit_cast");
    // A SPI IMU often returns a 16-bit two's-complement reading MSB-first.
    // Bytes arrive as {hi, lo}; reinterpret without aliasing UB.
    std::array<std::uint8_t, 2> rx_buffer{0xFF, 0x80};  // -128 raw count, BE

    std::uint16_t raw_be{};
    std::memcpy(&raw_be, rx_buffer.data(), sizeof(raw_be));
    std::uint16_t raw_host = be_to_host(raw_be);
    auto signed_count = std::bit_cast<std::int16_t>(raw_host);

    std::cout << "rx bytes  : 0x" << std::hex << static_cast<int>(rx_buffer[0])
              << ' ' << std::hex << static_cast<int>(rx_buffer[1]) << std::dec << '\n';
    std::cout << "raw_host  : 0x" << std::hex << raw_host << std::dec << '\n';
    std::cout << "signed    : " << signed_count << " counts\n";
}

// --- main --------------------------------------------------------------------

int main() {
    std::cout << "Endianness demos (companion to docs/big-endian_little-endian.md)\n";

    demo_runtime_detection();
    demo_compile_time_detection();
    demo_byte_layout();
    demo_byteswap();
    demo_can_frame();
    demo_network_header();
    demo_embedded_sensor_word();

    std::cout << "\nDone.\n";
    return 0;
}
