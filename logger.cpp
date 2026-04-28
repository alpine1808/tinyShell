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
    static bool needNewline = false;
    if (needNewline) {
        std::cout << "\n"; 
        needNewline = false;
    }
    
    switch(level) {
        case INFO:
            std::cout << Color::BOLD << Color::BLUE << "[INFO] " << Color::RESET;
            break;
        case LOG_ERROR:
            std::cout << Color::BOLD << Color::RED << "[ERROR] " << Color::RESET;
            break;
        case WARN:
            std::cout << Color::BOLD << Color::YELLOW << "[WARN] " << Color::RESET;
            break;
        case SUCCESS:
            std::cout << Color::BOLD << Color::GREEN << "[SUCCESS] " << Color::RESET;
            break;
        case PROCESS:
            std::cout << Color::BOLD << Color::CYAN << "[PROCESS] " << Color::RESET;
            break;
    }

    std::cout << message << std::endl;
}

void printInitialBanner() {
    pid_t pid = getpid(); 

    std::cout << Color::BOLD << Color::GREEN;
    std::cout << "=====================================\n";
    std::cout << "        Welcome to manhShell         \n";
    std::cout << "       Your tiny command shell       \n";
    std::cout << "=====================================\n";
    std::cout << Color::RESET;

    std::cout << "Welcome to Tiny Shell!\n";
    std::cout << "Process ID of Tiny Shell: " << pid << "\n";
    std::cout << "Type 'help' to see available commands.\n";
}