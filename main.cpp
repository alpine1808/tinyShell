#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include "builtins.h"
#include "commandParser.h"
#include "path.h"
#include "file_manager.h"
#include "logger.h"

namespace fs = std::filesystem;
using namespace std;

int main() {
    printInitialBanner();
    string input;
    while (true) {
        try {
            string current_dir = fs::current_path().string();
            cout << "[" << current_dir << "] manhShell> ";
        } catch (const fs::filesystem_error& e) {
            cout << "[unknown directory] manhShell> ";
        }
        
        if (!getline(cin, input)) break;
        if (input.empty()) continue;

        Command cmd = CommandParser::parse(input);

        if (!cmd.program.empty()) {
            vector<string> fullArgs;
            fullArgs.push_back(cmd.program);
            fullArgs.insert(fullArgs.end(), cmd.args.begin(), cmd.args.end());

            bool isHandled = handle_builtin(fullArgs);
            
            if (!isHandled) {
                if (cmd.program == "path" || cmd.program == "pwd") {
                    PathManager::executeCommand(cmd);
                    isHandled = true;
                }
            }

            if (!isHandled) {
                if (cmd.program == "ls" || cmd.program == "dir" || cmd.program == "mkdir" || 
                    cmd.program == "rm" || cmd.program == "mv" || cmd.program == "cp" || 
                    cmd.program == "touch" || cmd.program == "write" || cmd.program == "read") {
                    
                    FileManager::executeCommand(cmd);
                    isHandled = true;
                }
            }

            if (!isHandled) {
                if (cmd.isBackground) {
                    log(PROCESS, "Lenh '" + cmd.program + "' duoc yeu cau chay nen.");
                } else {
                    log(LOG_ERROR, "Command '" + cmd.program + "' is not a recognized builtin or internal command.");
                }
            }
        }
    }
    return 0;
}