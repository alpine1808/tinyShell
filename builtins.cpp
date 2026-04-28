#include "builtins.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include <iomanip>
#include <filesystem>
namespace fs = std::filesystem;
using namespace std;

static map<string,string> aliases;

void builtin_cd(const vector<string>& args) {
    if (args.size() < 2) {
        cerr << "Usage: cd <directory>\n";
        return;
    }
    try {
        fs::current_path(args[1]);
        cout << "Working directory changed to: " << fs::current_path().string() << endl;
    } catch (const fs::filesystem_error& e) {
        cerr << "cd error: " << e.what() << endl;
    }
}

bool builtin_exit() noexcept{
    cout << "Exiting tinyShell...\n";
    return true; 
}

void builtin_alias(const vector<string>& args) {
    if (args.size() < 2) {
        cerr << "Usage: alias name=command\n";
        return;
    }
    string input = args[1];
    auto pos = input.find('=');
    if (pos == string::npos) {
        cerr << "Invalid alias format. Use name=command\n";
        return;
    }
    string name = input.substr(0, pos);
    string cmd = input.substr(pos + 1);
    aliases[name] = cmd;
    cout << "Alias set: " << name << "='" << cmd << "'\n";
}

void builtin_unalias(const vector<string>& args) {
    if (args.size() < 2) {
        cerr << "Usage: unalias name\n";
        return;
    }
    string name = args[1];
    auto it = aliases.find(name);
    if (it != aliases.end()) {
        aliases.erase(it);
        cout << "Alias removed: " << name << "\n";
    } else {
        cerr << "Alias not found: " << name << "\n";
    }
}

void builtin_echo(const vector<string>& args) {
    bool add_newline = true; 
    bool interpret_escapes = false;
    size_t start_idx = 1;

    while (start_idx < args.size() && !args[start_idx].empty() && args[start_idx][0] == '-') {
        if (args[start_idx] == "--") {
            start_idx++;
            break;
        }
        bool valid_flag = true;
        bool has_n = false;
        bool has_e = false;
        if (args[start_idx].size() == 1) {
            valid_flag = false;
        } else {
            for (size_t i = 1; i < args[start_idx].size(); ++i) {
                char c = args[start_idx][i];
                if (c == 'n') has_n = true;
                else if (c == 'e') has_e = true;
                else {
                    valid_flag = false;
                    break;
                }
            }
        }
        if (valid_flag) {
            if (has_n) add_newline = false;
            if (has_e) interpret_escapes = true;
            start_idx++;
        } else {
            break;
        }
    }

    for (size_t i = start_idx; i < args.size(); i++) {
        const string& text = args[i];
        if (interpret_escapes) {
            string result;
            result.reserve(text.size());
            for (size_t j = 0; j < text.size(); j++) {
                if (text[j] == '\\' && j + 1 < text.size()) {
                    char next = text[j + 1];
                    switch (next) {
                        case 'n': result += '\n'; break;
                        case 't': result += '\t'; break;
                        case 'r': result += '\r'; break;
                        case 'a': result += '\a'; break;
                        case 'b': result += '\b'; break;
                        case 'f': result += '\f'; break;
                        case 'v': result += '\v'; break;
                        case '\\': result += '\\'; break;
                        default: result += '\\'; result += next;
                    }
                    j++;
                } else {
                    result += text[j];
                }
            }
            cout << result;
        } else {
            cout << text;
        }
        if (i < args.size() - 1) {
            cout << " ";
        }
    }
    if (add_newline) {
        cout << endl;
    }
}

void builtin_help() {
    int x = 30; 
    std::cout << std::left; 

    std::cout << std::setw(x) << "Command" << "Description" << std::endl;
    std::cout << "-------------------------------------------------------------------------" << std::endl;

    std::cout << "Shell Command:\n";
    std::cout << std::setw(x) << "help" << ": Show this command list" << std::endl;
    std::cout << std::setw(x) << "exit" << ": Exit tinyShell" << std::endl;
    std::cout << std::setw(x) << "echo [-n] [-e] [text]" << ": Display a line of text" << std::endl;
    std::cout << std::setw(x) << "  -n" << ": Do not output the trailing newline" << std::endl;
    std::cout << std::setw(x) << "  -e" << ": Enable interpretation of backslash escapes" << std::endl;
    std::cout << "-------------------------------------------------------------------------" << std::endl;

    std::cout << "File/Directory Command:\n";
    std::cout << std::setw(x) << "cd <dir>" << ": Change current working directory" << std::endl;
    std::cout << std::setw(x) << "ls [dir]" << ": List directory contents " << std::endl;
    std::cout << std::setw(x) << "mkdir <dir>" << ": Create a new directory " << std::endl;
    std::cout << std::setw(x) << "rm <path>" << ": Remove a file or directory " << std::endl;
    std::cout << std::setw(x) << "mv <src> <dest>" << ": Move or rename a file/directory " << std::endl;
    std::cout << std::setw(x) << "cp <src> <dest>" << ": Copy a file or directory " << std::endl;
    std::cout << std::setw(x) << "touch <file>" << ": Create or update a file " << std::endl;
    std::cout << std::setw(x) << "write <file> <text>" << ": Write text to a file (overwrites) " << std::endl;
    std::cout << std::setw(x) << "read <file>" << ": Read and display file contents " << std::endl;
    std::cout << "-------------------------------------------------------------------------" << std::endl;

    std::cout << "Path Command:\n";
    std::cout << std::setw(x) << "pwd" << ": Print current working directory " << std::endl;
    std::cout << std::setw(x) << "path" << ": Display tinyShell PATH " << std::endl;
    std::cout << std::setw(x) << "path add <dir>" << ": Add directory to tinyShell PATH " << std::endl;
    std::cout << std::setw(x) << "path remove <dir>" << ": Remove directory from tinyShell PATH " << std::endl;
    std::cout << "-------------------------------------------------------------------------" << std::endl;

    std::cout << "Process Command:\n";
    std::cout << std::setw(x) << "start <processName>" << ": Start foreground process " << std::endl;
    std::cout << std::setw(x) << "start <processName> &" << ": Start background process " << std::endl;
    std::cout << std::setw(x) << "myList" << ": List background processes created by tinyShell " << std::endl;
    std::cout << std::setw(x) << "globalList" << ": List all background processes " << std::endl;
    std::cout << std::setw(x) << "child <pid>" << ": List all child processes " << std::endl;
    std::cout << std::setw(x) << "kill <pid>" << ": Kill a process by PID " << std::endl;
    std::cout << std::setw(x) << "stop <pid>" << ": Stop a process by PID " << std::endl;
    std::cout << std::setw(x) << "resume <pid>" << ": Resume a stopped process by PID " << std::endl;
    std::cout << "-------------------------------------------------------------------------" << std::endl;

    std::cout << "Miscellaneous Command:\n";
    std::cout << std::setw(x) << "alias name=cmd" << ": Define a command alias" << std::endl;
    std::cout << std::setw(x) << "unalias name" << ": Remove an alias" << std::endl;
    std::cout << "-------------------------------------------------------------------------" << std::endl;

    std::cout << "Other Command:\n";
}

bool is_alias(string_view name) noexcept{
    return aliases.find(string(name)) != aliases.end();
}

string resolve_alias(string_view name) {
    return aliases[string(name)];
}

bool handle_builtin(vector<string>& args) {
    if (args.empty()) return false;
    string cmd = args[0];
    if (cmd == "cd") {
        builtin_cd(args);
        return true;
    } else if (cmd == "alias") {
        builtin_alias(args);
        return true;
    } else if (cmd == "unalias") {
        builtin_unalias(args);
        return true;
    } else if (cmd == "help") {
        builtin_help();
        return true;
    } else if (cmd == "exit") {
        if (builtin_exit()) {
            exit(0);
        }
        return true;
    } else if (cmd == "echo") {
        builtin_echo(args);
        return true;
    }
    if (is_alias(cmd)) {
        args[0] = resolve_alias(cmd);
        return false; 
    }
    return false; 
}