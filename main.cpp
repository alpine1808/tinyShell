#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include "builtins.h"
#include "commandParser.h"
#include "path.h"
#include "file_manager.h"
#include "logger.h"
#include "ctrl_c_handler.h"
#include "history.h"
#include "process_manager.h"
#include "process_executor.h"
#include "environment.h"

namespace fs = std::filesystem;
using namespace std;

void processLine(const string& input, ProcessManager& procManager) {
    if (input.empty()) return;

    vector<Command> pipeline = CommandParser::parsePipeline(input, procManager);
    if (pipeline.empty()) return;

    if (pipeline.size() == 1) {
        Command& cmd = pipeline[0];
        if (cmd.program.empty()) return;

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
            isHandled = procManager.handleCommand(cmd);
        }

        if (!isHandled) {
            ProcessExecutor::executePipeline(pipeline, procManager);
        }
    } else {
        ProcessExecutor::executePipeline(pipeline, procManager);
    }
}

int main(int argc, char* argv[], char* envp[]) {
    for (char **env = envp; *env != 0; env++) {
        string envStr = *env;
        auto pos = envStr.find('=');
        if (pos != string::npos) {
            Environment::setVar(envStr.substr(0, pos), envStr.substr(pos + 1));
        }
    }

    printInitialBanner();
    CtrlCHandler::initialize();
    ProcessManager procManager;

    if (argc == 2) {
        ifstream scriptFile(argv[1]);
        if (!scriptFile.is_open()) {
            cerr << "Error: Cannot open script file " << argv[1] << endl;
            return 1;
        }
        string line;
        while (getline(scriptFile, line)) {
            processLine(line, procManager);
        }
        return 0;
    }

    string input;
    while (true) {
        try {
            string current_dir = fs::current_path().string();
            cout << "[" << current_dir << "] manhShell> ";
        } catch (const fs::filesystem_error& e) {
            cout << "[unknown directory] manhShell> ";
        }

        if (!getline(cin, input)) break;
        processLine(input, procManager);
    }
    return 0;
}