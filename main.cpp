#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include "builtins.h"
#include "commandParser.h" // 1. Tích hợp header của Parser

namespace fs = std::filesystem;
using namespace std;

int main() {
    string input;
    while (true) {
        try {
            string current_dir = fs::current_path().string();
            cout << "[" << current_dir << "] tinyshell> ";
        } catch (const fs::filesystem_error& e) {
            cout << "[unknown directory] tinyshell> ";
        }
        
        if (!getline(cin, input)) break;
        if (input.empty()) continue;

        Command cmd = CommandParser::parse(input);

        if (!cmd.program.empty()) {
            vector<string> fullArgs;
            fullArgs.push_back(cmd.program);
            fullArgs.insert(fullArgs.end(), cmd.args.begin(), cmd.args.end());

            bool isBuiltin = handle_builtin(fullArgs);
            
            if (!isBuiltin) {
                if (cmd.isBackground) {
                    cout << "[Thông tin] Lệnh '" << cmd.program << "' được yêu cầu chạy nền.\n";
                }
                
                cout << "Command '" << cmd.program << "' is not a recognized builtin.\n";
            }
        }
    }
    return 0;
}