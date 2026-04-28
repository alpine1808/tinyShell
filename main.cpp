#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include "builtins.h"
#include "commandParser.h"
#include "path.h" // Thêm header mới

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
            // Chuyển đổi sang định dạng vector<string> để tương thích với handle_builtin cũ
            vector<string> fullArgs;
            fullArgs.push_back(cmd.program);
            fullArgs.insert(fullArgs.end(), cmd.args.begin(), cmd.args.end());

            // Biến cờ để kiểm tra lệnh đã được xử lý chưa
            bool isHandled = false;

            // BƯỚC 1: Kiểm tra các lệnh built-in (cd, exit, echo, alias...)
            isHandled = handle_builtin(fullArgs);
            
            // BƯỚC 2: Nếu chưa xử lý, kiểm tra các lệnh thuộc PathManager (path, pwd)
            if (!isHandled) {
                if (cmd.program == "path" || cmd.program == "pwd") {
                    PathManager::executeCommand(cmd);
                    isHandled = true;
                }
            }

            // BƯỚC 3: Nếu vẫn chưa xử lý, báo lỗi hoặc chạy nền
            if (!isHandled) {
                if (cmd.isBackground) {
                    cout << "[Thông tin] Lệnh '" << cmd.program << "' được yêu cầu chạy nền.\n";
                }
                
                cout << "Command '" << cmd.program << "' is not a recognized builtin.\n";
            }
        }
    }
    return 0;
}