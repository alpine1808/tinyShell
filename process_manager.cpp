#include "process_manager.h"
#include "logger.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>

using namespace std;

vector<BackgroundProcess> bgProcesses;
ProcessManager processManager;

void ProcessManager::addProcess(pid_t pid, const string& cmd) {
    bgProcesses.push_back({pid, cmd, true});
    log(PROCESS, "Added process with PID: " + to_string(pid) + " | Command: " + cmd);
}

void startProcess(const string& cmdStr, bool isBackground) {
    vector<string> args;
    istringstream iss(cmdStr);
    string arg;
    while (iss >> arg) {
        args.push_back(arg);
    }

    if (args.empty()) return;

    vector<char*> c_args;
    for (auto& a : args) {
        c_args.push_back(&a[0]);
    }
    c_args.push_back(nullptr);

    pid_t pid = fork();

    if (pid < 0) {
        log(LOG_ERROR, "Failed to fork process: " + cmdStr);
    } else if (pid == 0) {
        if (isBackground) {
            setpgid(0, 0);
        }
        execvp(c_args[0], c_args.data());
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        if (isBackground) {
            ProcessManager::addProcess(pid, cmdStr);
            ProcessHistory::addProcess(pid, args[0]);
            log(INFO, "Started background process with PID: " + to_string(pid) + ", Command: " + cmdStr);
        } else {
            ProcessHistory::addProcess(pid, args[0]);
            CtrlCHandler::setForegroundProcess(pid);
            
            int status;
            waitpid(pid, &status, WUNTRACED);
            
            CtrlCHandler::resetForegroundProcess();
            ProcessHistory::updateEndTime(pid);
        }
    }
}

struct ProcInfo {
    pid_t pid;
    string comm;
    char state;
    pid_t ppid;
};

bool getProcInfo(pid_t pid, ProcInfo& info) {
    ifstream statFile("/proc/" + to_string(pid) + "/stat");
    if (!statFile.is_open()) return false;

    string line;
    getline(statFile, line);
    
    size_t leftParen = line.find('(');
    size_t rightParen = line.find_last_of(')');
    
    if (leftParen == string::npos || rightParen == string::npos) return false;

    info.pid = pid;
    info.comm = line.substr(leftParen + 1, rightParen - leftParen - 1);
    
    string rest = line.substr(rightParen + 2);
    istringstream iss(rest);
    iss >> info.state >> info.ppid;
    
    return true;
}

string getStateString(char state) {
    switch (state) {
        case 'R': return "Running";
        case 'S': return "Sleeping";
        case 'D': return "Disk Sleep";
        case 'Z': return "Zombie";
        case 'T': return "Stopped";
        case 't': return "Tracing Stop";
        case 'X': case 'x': return "Dead";
        case 'I': return "Idle";
        default: return "Unknown";
    }
}

void ProcessManager::listProcesses() {
    log(INFO, "Listing background processes:");
    
    cout << setw(10) << left << "PID" 
         << setw(40) << left << "Command" 
         << setw(20) << left << "Parent PID"
         << setw(20) << left << "Status" << endl;
    cout << string(90, '=') << endl;
    
    for (auto it = bgProcesses.begin(); it != bgProcesses.end(); ) {
        ProcInfo info;
        if (getProcInfo(it->pid, info)) {
            cout << setw(10) << left << it->pid
                 << setw(40) << left << it->command
                 << setw(20) << left << info.ppid
                 << setw(20) << left << getStateString(info.state) << endl;
            it->isRunning = (info.state != 'Z' && info.state != 'X');
            ++it;
        } else {
            cout << setw(10) << left << it->pid
                 << setw(40) << left << it->command
                 << setw(20) << left << "N/A"
                 << setw(20) << left << "Terminated" << endl;
            ProcessHistory::updateEndTime(it->pid);
            it = bgProcesses.erase(it);
        }
    }
}

void ProcessManager::globalList() {
    cout << setw(10) << left << "PID"
         << setw(40) << left << "Process Name"
         << setw(20) << left << "Parent PID" 
         << setw(20) << left << "Status" << endl;
    cout << string(90, '=') << endl;

    DIR* dir = opendir("/proc");
    if (!dir) return;

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_DIR) {
            string dirName = entry->d_name;
            if (all_of(dirName.begin(), dirName.end(), ::isdigit)) {
                ProcInfo info;
                if (getProcInfo(stoi(dirName), info)) {
                    cout << setw(10) << left << info.pid
                         << setw(40) << left << info.comm
                         << setw(20) << left << info.ppid
                         << setw(20) << left << getStateString(info.state) << endl;
                }
            }
        }
    }
    closedir(dir);
}

void ProcessManager::killProcess(pid_t pid) {
    if (kill(pid, SIGKILL) == 0) {
        ProcessHistory::updateEndTime(pid);
        cout << "Process with PID: " << pid << " terminated successfully" << endl;
    } else {
        log(LOG_ERROR, "Failed to kill process with PID: " + to_string(pid));
    }
}

void ProcessManager::stopProcess(pid_t pid) {
    if (kill(pid, SIGSTOP) == 0) {
        log(INFO, "Stopped process with PID: " + to_string(pid));
    } else {
        log(LOG_ERROR, "Failed to stop process with PID: " + to_string(pid));
    }
}

void ProcessManager::resumeProcess(pid_t pid) {
    if (kill(pid, SIGCONT) == 0) {
        log(INFO, "Resumed process with PID: " + to_string(pid));
    } else {
        log(LOG_ERROR, "Failed to resume process with PID: " + to_string(pid));
    }
}

void ProcessManager::findChildProcesses(pid_t parentPid) {
    log(INFO, "Child processes of PID " + to_string(parentPid) + ":");
    
    cout << setw(10) << left << "PID" 
         << setw(40) << left << "Process Name" << endl;
    cout << string(50, '=') << endl;

    int count = 0;
    DIR* dir = opendir("/proc");
    if (!dir) return;

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_DIR) {
            string dirName = entry->d_name;
            if (all_of(dirName.begin(), dirName.end(), ::isdigit)) {
                ProcInfo info;
                if (getProcInfo(stoi(dirName), info)) {
                    if (info.ppid == parentPid) {
                        cout << setw(10) << left << info.pid 
                             << setw(40) << left << info.comm << endl;
                        count++;
                    }
                }
            }
        }
    }
    closedir(dir);
    log(INFO, "Found " + to_string(count) + " child process(es).");
}

bool ProcessManager::handleCommand(const Command& cmd) {
    if (cmd.program == "globalList") {
        globalList();
        return true;
    }
    if (cmd.program == "myList") {
        listProcesses();
        return true;
    }
    if (cmd.program == "start" && !cmd.args.empty()) {
        string fullCmd = cmd.args[0];
        for (size_t i = 1; i < cmd.args.size(); ++i) {
            fullCmd += " " + cmd.args[i];
        }
        startProcess(fullCmd, cmd.isBackground);
        return true;
    }    
    if (cmd.program == "kill" && !cmd.args.empty()) {
        for (const auto& arg : cmd.args) {
            try {
                killProcess(stoi(arg));
            } catch (const invalid_argument&) {}
        }
        return true;
    }
    if (cmd.program == "stop" && !cmd.args.empty()) {
        for (const auto& arg : cmd.args) {
            try {
                stopProcess(stoi(arg));
            } catch (const invalid_argument&) {}
        }
        return true;
    }    
    if (cmd.program == "resume" && !cmd.args.empty()) {
        for (const auto& arg : cmd.args) {
            try {
                resumeProcess(stoi(arg));
            } catch (const invalid_argument&) {}
        }
        return true;
    }    
    if (cmd.program == "history") {
        ProcessHistory::displayHistory();
        return true;
    }
    if (cmd.program == "child" && !cmd.args.empty()) {
        findChildProcesses(stoi(cmd.args[0]));
        return true;
    }
    return false;
}