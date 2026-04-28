#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include "builtins.h"
#include "commandParser.h"
#include "path.h"
#include "file_manager.h" // Bổ sung header để quản lý file

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
            // Chuyển đổi sang định dạng vector<string> để tương thích với handle_builtin
            vector<string> fullArgs;
            fullArgs.push_back(cmd.program);
            fullArgs.insert(fullArgs.end(), cmd.args.begin(), cmd.args.end());

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

            // BƯỚC 3: Bổ sung kiểm tra các lệnh thuộc FileManager (ls, mkdir, rm, cp, mv, touch, write, read)
            if (!isHandled) {
                // Danh sách các lệnh mà FileManager hỗ trợ dựa trên file_manager.cpp của bạn
                if (cmd.program == "ls" || cmd.program == "dir" || cmd.program == "mkdir" || 
                    cmd.program == "rm" || cmd.program == "mv" || cmd.program == "cp" || 
                    cmd.program == "touch" || cmd.program == "write" || cmd.program == "read") {
                    
                    FileManager::executeCommand(cmd);
                    isHandled = true;
                }
            }

            // BƯỚC 4: Xử lý các trường hợp còn lại (Lệnh hệ thống bên ngoài hoặc báo lỗi)
            if (!isHandled) {
                if (cmd.isBackground) {
                    cout << "[Thông tin] Lệnh '" << cmd.program << "' được yêu cầu chạy nền.\n";
                    // Tương lai bạn sẽ thêm logic fork/exec cho tiến trình chạy nền ở đây
                }
                
                cout << "Command '" << cmd.program << "' is not a recognized builtin or internal command.\n";
            }
        }
    }
    return 0;
}