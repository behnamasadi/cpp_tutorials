// C-style errno: the old, thread-local integer set by libc on failure.
// Companion file: error_code.cpp covers the C++ std::error_code side.

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iostream>

int main() {
    // ---- 1. errno after a failing call ----
    std::cout << "--- errno after fopen ---\n";
    std::FILE* fp = std::fopen("missing.txt", "r");
    if (!fp) {
        // Save errno immediately: almost anything (even printf) can clobber it.
        int err = errno;
        std::cout << "errno = " << err
                  << " (" << std::strerror(err) << ")\n";
    }

    // ---- 2. perror writes "context: message\n" to stderr ----
    std::cout << "--- perror ---\n";
    fp = std::fopen("also_missing.txt", "r");
    if (!fp) {
        std::perror("open failed");
    }

    // ---- 3. errno macros vs bare integers ----
    // Numeric values are platform-specific; always use the macros.
    std::cout << "--- errno macros ---\n";
    std::cout << "ENOENT = " << ENOENT
              << " (" << std::strerror(ENOENT) << ")\n";
    std::cout << "EACCES = " << EACCES
              << " (" << std::strerror(EACCES) << ")\n";
    std::cout << "EAGAIN = " << EAGAIN
              << " (" << std::strerror(EAGAIN) << ")\n";

    // ---- 4. errno is meaningful only after a failure return ----
    std::cout << "--- errno is only valid after failure ---\n";
    errno = 0;
    fp = std::fopen("missing.txt", "r");
    if (!fp) {
        std::cout << "fopen failed, errno = " << errno << "\n";
    }
}
