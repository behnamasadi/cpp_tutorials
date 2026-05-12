// std::error_code: a typed (value, category) pair for recoverable errors.
// Companion file: error_handling.cpp covers the C-style errno side.

#include <cerrno>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <system_error>

// 2.1 Reporting an error via an out parameter.
void open_file(const std::string& path, std::error_code& ec) {
    std::ifstream file(path);
    if (!file) {
        ec = std::make_error_code(std::errc::no_such_file_or_directory);
        return;
    }
    ec.clear();
}

// 2.2 Wrapping an errno into an error_code.
void remove_file(const std::string& path, std::error_code& ec) {
    if (std::remove(path.c_str()) != 0)
        ec = std::error_code(errno, std::system_category());
    else
        ec.clear();
}

// 2.5 A custom error category.
enum class parse_err { ok = 0, empty_input, bad_number, out_of_range };

class parse_category_t : public std::error_category {
public:
    const char* name() const noexcept override { return "parse"; }
    std::string message(int v) const override {
        switch (static_cast<parse_err>(v)) {
            case parse_err::empty_input:  return "empty input";
            case parse_err::bad_number:   return "not a number";
            case parse_err::out_of_range: return "value out of range";
            default:                      return "ok";
        }
    }
};

const std::error_category& parse_category() {
    static parse_category_t c;
    return c;
}

std::error_code make_error_code(parse_err e) {
    return {static_cast<int>(e), parse_category()};
}

namespace std {
template <>
struct is_error_code_enum<parse_err> : true_type {};
}  // namespace std

int main() {
    // ---- 2.1 out-parameter style ----
    std::cout << "--- out-parameter error_code ---\n";
    std::error_code ec;
    open_file("nope.txt", ec);
    if (ec) {
        std::cout << "open failed: " << ec.message()
                  << " (value " << ec.value()
                  << ", category " << ec.category().name() << ")\n";
    }

    // ---- 2.2 wrapping errno ----
    std::cout << "--- wrap errno into error_code ---\n";
    remove_file("nope.txt", ec);
    if (ec) {
        std::cout << "remove failed: " << ec.message()
                  << " (value " << ec.value() << ")\n";
    }

    // ---- 2.3 std::filesystem non-throwing overload ----
    std::cout << "--- filesystem non-throwing API ---\n";
    auto sz = std::filesystem::file_size("nope.txt", ec);
    if (ec) {
        std::cout << "file_size failed: " << ec.message() << "\n";
    } else {
        std::cout << "size = " << sz << "\n";
    }

    // ---- 2.4 comparing against a portable std::errc condition ----
    std::cout << "--- compare against std::errc ---\n";
    if (ec == std::errc::no_such_file_or_directory) {
        std::cout << "matched std::errc::no_such_file_or_directory\n";
    }

    // ---- 2.5 custom category ----
    std::cout << "--- custom parse_category ---\n";
    std::error_code pec = make_error_code(parse_err::bad_number);
    std::cout << "category: " << pec.category().name()
              << ", value: " << pec.value()
              << ", message: " << pec.message() << "\n";
    if (pec == parse_err::bad_number) {
        std::cout << "matched parse_err::bad_number\n";
    }

    // ---- 4. throwing form: std::system_error wraps an error_code ----
    std::cout << "--- std::system_error wraps error_code ---\n";
    try {
        throw std::system_error(
            std::error_code(EACCES, std::system_category()),
            "demo");
    } catch (const std::system_error& e) {
        std::cout << "caught: " << e.what()
                  << " (code " << e.code().value() << ")\n";
    }
}
