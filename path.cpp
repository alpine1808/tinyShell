#include "path.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
using namespace std;
std::vector<std::string> PathManager::tinyShellPath;

void PathManager::executeCommand(const Command& cmd) {
    if (cmd.program == "path") {
        managePath(cmd.args);
    } else if (cmd.program == "pwd") {
        printWorkingDirectory(cmd.args);
    } else {
        std::cerr << "Unknown command: " << cmd.program << std::endl;
    }
}

void PathManager::printWorkingDirectory(const vector<string>& args) {
    if (!args.empty()) {
        cout << "pwd: no arguments expected" << endl;
        return;
    }
    try {
        cout << fs::current_path().string() << endl;
    } catch (const fs::filesystem_error& e) {
        cout << "Error getting current directory: " << e.what() << endl;
    }
}

void PathManager::managePath(const vector<string>& args) {
    if (args.empty()) {
        if (tinyShellPath.empty()) {
            cout << "tinyShell PATH is empty" << endl;
        } else {
            cout << "tinyShell PATH:" << endl;
            for (size_t i = 0; i < tinyShellPath.size(); ++i) {
                cout << "  " << setw(2) << i + 1 << ". " << tinyShellPath[i] << endl;
            }
        }
        return;
    }

    if (args[0] == "add" && args.size() >= 2) {
        // Add directory to tinyShell PATH
        string new_dir = args[1];
        if (!fs::exists(new_dir)) {
            cout << "path add: directory does not exist: " << new_dir << endl;
            return;
        }
        // Check if directory is already in PATH
        if (find(tinyShellPath.begin(), tinyShellPath.end(), new_dir) != tinyShellPath.end()) {
            cout << "path add: directory already in tinyShell PATH: " << new_dir << endl;
            return;
        }
        tinyShellPath.push_back(new_dir);
        cout << "Added to tinyShell PATH: " << new_dir << endl;
    } else if (args[0] == "remove" && args.size() >= 2) {
        // Remove directory from tinyShell PATH
        string dir_to_remove = args[1];
        auto it = find(tinyShellPath.begin(), tinyShellPath.end(), dir_to_remove);
        if (it == tinyShellPath.end()) {
            cout << "path remove: directory not found in tinyShell PATH: " << dir_to_remove << endl;
            return;
        }
        tinyShellPath.erase(it);
        cout << "Removed from tinyShell PATH: " << dir_to_remove << endl;
    } else {
        cout << "Usage: path [add <dir> | remove <dir>]" << endl;
    }
}