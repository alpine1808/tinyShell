#include "process_executor.h"
#include "process_manager.h"
#include "logger.h"
#include "ctrl_c_handler.h"
#include <string>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

using namespace std;

void ProcessExecutor::execute(const Command& cmd, ProcessManager& procManager) {
    string fullCmd = cmd.program;
    for (const auto& arg : cmd.args) {
        fullCmd += " " + arg;
    }

    vector<string> args;
    args.push_back(cmd.program);
    for (const auto& arg : cmd.args) {
        args.push_back(arg);
    }

    vector<char*> c_args;
    for (auto& a : args) {
        c_args.push_back(&a[0]);
    }
    c_args.push_back(nullptr);

    pid_t pid = fork();

    if (pid < 0) {
        log(LOG_ERROR, "Failed to start process: " + fullCmd);
    } else if (pid == 0) {
        if (cmd.isBackground) {
            setpgid(0, 0);
        }
        execvp(c_args[0], c_args.data());
        exit(EXIT_FAILURE);
    } else {
        if (cmd.isBackground) {
            procManager.addProcess(pid, fullCmd);
            log(INFO, "Started background process with PID: " + to_string(pid) +
                      ", Command: " + fullCmd);
        } else {
            CtrlCHandler::setForegroundProcess(pid);
            
            int status;
            waitpid(pid, &status, WUNTRACED);
            
            if (WIFEXITED(status)) {
                int exitCode = WEXITSTATUS(status);
                if (exitCode != 0) {
                    log(INFO, "Process exited with code: " + to_string(exitCode));
                }
            } else if (WIFSIGNALED(status)) {
                log(INFO, "Process terminated by signal: " + to_string(WTERMSIG(status)));
            }
            
            CtrlCHandler::resetForegroundProcess();
            cout << endl;
        }
    }
}