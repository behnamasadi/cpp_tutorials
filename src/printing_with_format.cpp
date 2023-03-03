#include <format>
#include <iostream>
#include <string>
#include <string_view>
 
template <typename... Args>
std::string dyna_print(std::string_view rt_fmt_str, Args&&... args) {
    return std::vformat(rt_fmt_str, std::make_format_args(args...));
}
 
int main() {
    std::cout << std::format("Hello {}!\n", "world");
 
    std::string fmt;
    for (int i{}; i != 3; ++i) {
        fmt += "{} "; // constructs the formatting string
        std::cout << fmt << " : ";
        std::cout << dyna_print(fmt, "alpha", 'Z', 3.14, "unused");
        std::cout << '\n';
    }
}
