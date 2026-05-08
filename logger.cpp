#include "logger.h"
#include <iostream>
#include <unistd.h>

namespace Color {
    const char* RESET   = "\033[0m";
    const char* RED     = "\033[31m";
    const char* GREEN   = "\033[32m";
    const char* YELLOW  = "\033[33m";
    const char* BLUE    = "\033[34m";
    const char* CYAN    = "\033[36m";
    const char* BOLD    = "\033[1m";
}

void log(LogLevel level, const std::string& message) {
    if (level == INFO || level == PROCESS) {
        return;
    }

    static bool needNewline = false;
    if (needNewline) {
        std::cerr << "\n"; 
        needNewline = false;
    }
    
    switch(level) {
        case LOG_ERROR:
            std::cerr << Color::BOLD << Color::RED << "[ERROR] " << Color::RESET;
            break;
        case WARN:
            std::cerr << Color::BOLD << Color::YELLOW << "[WARN] " << Color::RESET;
            break;
        case SUCCESS:
            std::cerr << Color::BOLD << Color::GREEN << "[SUCCESS] " << Color::RESET;
            break;
        default:
            break;
    }

    std::cerr << message << std::endl;
}

void printInitialBanner() {
    pid_t pid = getpid(); 

    std::cerr << Color::BOLD << Color::GREEN;
    std::cerr << "=====================================\n";
    std::cerr << "        Welcome to manhShell         \n";
    std::cerr << "       Your tiny command shell       \n";
    std::cerr << "=====================================\n";
    std::cerr << Color::RESET;
}