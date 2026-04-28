#ifndef COLOR_H
#define COLOR_H

#include <string_view>
#include <iostream>

namespace Color {
    constexpr std::string_view GREEN  = "\033[32m";
    constexpr std::string_view CYAN   = "\033[36m";
    constexpr std::string_view YELLOW = "\033[33m";
    constexpr std::string_view RESET  = "\033[0m";
}

#endif // COLOR_H