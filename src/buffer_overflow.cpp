// Buffer overflow demos — mirrors docs/buffer_overflow.md.
//
// Buffer overflow = writing past the end of a buffer. In robotics this shows
// up as: a CAN frame larger than expected, a serial DMA stream that exceeds
// the ring-buffer slot, or a camera frame whose row stride was miscalculated
// (e.g. 640*3 vs 640*4 bytes when alpha is unexpectedly present).
//
// This binary DEMONSTRATES the APIs. The actually-unsafe demo is gated behind
// `--unsafe` so the program runs cleanly by default. Build with -fsanitize=address
// (or compile in Debug with stack-protector on) to see ASan / __stack_chk_fail
// catch the corruption when you do pass --unsafe.

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace {

// ---------------------------------------------------------------------------
// 1. The classic vulnerable example from the doc: strcpy into a small buffer.
//    Gated — running this with a long input would smash the stack.
// ---------------------------------------------------------------------------
void demo_unsafe_strcpy() {
    std::cout << "[unsafe] strcpy into a 10-byte buffer with a 56-byte string\n";
    char buffer[10];
    const char *payload = "This is a very long string that exceeds the buffer size.";
    // Compilers warn; ASan reports stack-buffer-overflow; -fstack-protector
    // aborts via __stack_chk_fail. NEVER do this in real code.
    std::strcpy(buffer, payload);
    std::cout << "  wrote: " << buffer << '\n';
}

// ---------------------------------------------------------------------------
// 2. Prevention #1: bounds checking. Check the size BEFORE copying.
// ---------------------------------------------------------------------------
void demo_bounds_check(std::string_view input) {
    char buffer[10] = {};
    if (input.size() >= sizeof(buffer)) {
        std::cout << "[bounds] rejecting " << input.size()
                  << "-byte input (capacity " << sizeof(buffer) - 1 << ")\n";
        return;
    }
    std::memcpy(buffer, input.data(), input.size());
    buffer[input.size()] = '\0';
    std::cout << "[bounds] accepted: " << buffer << '\n';
}

// ---------------------------------------------------------------------------
// 3. Prevention #2: safer C functions. strncpy caps the copy length, but
//    note the well-known gotcha — it does NOT null-terminate if the source
//    fills the buffer. Always terminate explicitly.
// ---------------------------------------------------------------------------
void demo_strncpy() {
    char buffer[10];
    const char *payload = "This is a very long string that exceeds the buffer size.";
    std::strncpy(buffer, payload, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';  // guarantee termination
    std::cout << "[strncpy] truncated to: " << buffer << '\n';
}

// ---------------------------------------------------------------------------
// 4. Prevention #3: dynamic allocation sized to the input. Equivalent to
//    receiving a CAN/serial frame of advertised length `n` and allocating
//    a matching scratch buffer instead of using a fixed stack array.
// ---------------------------------------------------------------------------
void demo_dynamic_alloc(std::string_view payload) {
    std::vector<char> buf(payload.size() + 1);
    std::memcpy(buf.data(), payload.data(), payload.size());
    buf[payload.size()] = '\0';
    std::cout << "[dynamic] " << buf.size() - 1 << " bytes copied safely\n";
}

// ---------------------------------------------------------------------------
// 5. Prevention #4: high-level constructs. std::string / std::vector own
//    their storage and grow as needed — the idiomatic modern C++ answer.
// ---------------------------------------------------------------------------
void demo_high_level(std::string_view payload) {
    std::string s(payload);            // no overflow possible
    std::cout << "[std::string] " << s.size() << " bytes, content: " << s << '\n';
}

// ---------------------------------------------------------------------------
// 6. Robotics example: image row stride miscalc. A 4-row RGB image at
//    width=8 needs 96 bytes (8*3*4). If the producer assumed RGBA (stride 32)
//    and the consumer assumes RGB (stride 24), the consumer reads past row 3.
//    std::span + .subspan() makes the bounds explicit and catches it.
// ---------------------------------------------------------------------------
void demo_image_stride() {
    constexpr std::size_t width = 8, height = 4, channels = 3;
    std::array<std::uint8_t, width * height * channels> frame{};
    std::span<std::uint8_t> view(frame);

    for (std::size_t row = 0; row < height; ++row) {
        auto row_view = view.subspan(row * width * channels, width * channels);
        std::fill(row_view.begin(), row_view.end(), static_cast<std::uint8_t>(row));
    }
    std::cout << "[stride] filled " << height << " rows of " << width * channels
              << " bytes; last byte = " << int(frame.back()) << '\n';
    // view.subspan(height * width * channels, 1) would throw/UB-trap — span
    // gives us a single place to assert(offset + len <= size()).
}

}  // namespace

int main(int argc, char **argv) {
    const bool run_unsafe =
        argc > 1 && std::string_view(argv[1]) == "--unsafe";

    std::cout << "=== buffer overflow: APIs and defenses ===\n\n";

    demo_bounds_check("ok");
    demo_bounds_check("way too long for ten bytes");
    demo_strncpy();
    demo_dynamic_alloc("arbitrary length sensor payload");
    demo_high_level("hello from std::string");
    demo_image_stride();

    std::cout << "\n--- unsafe demo (gated) ---\n";
    if (run_unsafe) {
        demo_unsafe_strcpy();  // ASan: stack-buffer-overflow; or SIGABRT via stack canary
    } else {
        std::cout << "skipped; pass --unsafe to run the strcpy overflow.\n"
                     "  Build with -fsanitize=address to see the diagnostic,\n"
                     "  or rely on -fstack-protector to abort via __stack_chk_fail.\n";
    }
    return 0;
}
