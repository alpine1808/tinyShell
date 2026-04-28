#pragma once

#include <vector>
#include <string>
#include <sys/types.h>
#include "commandParser.h"
#include "history.h"
#include "ctrl_c_handler.h"

struct BackgroundProcess {
    pid_t pid;
    std::string command;
    bool isRunning;
};

class ProcessManager {
public:
    static void addProcess(pid_t pid, const std::string& cmd);
    static void listProcesses();
    static void globalList();
    static void killProcess(pid_t pid);
    static void stopProcess(pid_t pid);
    static void findChildProcesses(pid_t parentPid);
    static void resumeProcess(pid_t pid);
    bool handleCommand(const Command& cmd);
};