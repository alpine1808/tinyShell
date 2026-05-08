#include "process_executor.h"
#include "process_manager.h"
#include "logger.h"
#include "ctrl_c_handler.h"
#include "path.h"
#include "builtins.h"
#include "file_manager.h"
#include <string>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>

using namespace std;

void ProcessExecutor::executePipeline(const vector<Command>& pipeline, ProcessManager& procManager) {
    int numCmds = pipeline.size();
    int prevReadFd = -1;
    vector<pid_t> pids;
    bool isBackground = pipeline.back().isBackground;

    for (int i = 0; i < numCmds; ++i) {
        int pipefd[2];
        if (i < numCmds - 1) {
            if (pipe(pipefd) == -1) {
                perror("pipe");
                return;
            }
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            if (prevReadFd != -1) close(prevReadFd);
            return;
        }

        if (pid == 0) {
            if (isBackground) {
                setpgid(0, 0);
            }

            if (i > 0) {
                dup2(prevReadFd, STDIN_FILENO);
                close(prevReadFd);
            }

            if (i < numCmds - 1) {
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
                close(pipefd[0]);
            }

            vector<string> fullArgs;
            fullArgs.push_back(pipeline[i].program);
            fullArgs.insert(fullArgs.end(), pipeline[i].args.begin(), pipeline[i].args.end());

            bool isHandled = handle_builtin(fullArgs);
            if (!isHandled) {
                if (pipeline[i].program == "path" || pipeline[i].program == "pwd") {
                    PathManager::executeCommand(pipeline[i]);
                    isHandled = true;
                }
            }
            if (!isHandled) {
                if (pipeline[i].program == "ls" || pipeline[i].program == "dir" || pipeline[i].program == "mkdir" ||
                    pipeline[i].program == "rm" || pipeline[i].program == "mv" || pipeline[i].program == "cp" ||
                    pipeline[i].program == "touch" || pipeline[i].program == "write" || pipeline[i].program == "read") {
                    FileManager::executeCommand(pipeline[i]);
                    isHandled = true;
                }
            }
            if (!isHandled) {
                isHandled = procManager.handleCommand(pipeline[i]);
            }

            if (isHandled) {
                exit(EXIT_SUCCESS);
            }

            vector<char*> c_args;
            vector<string> args = {pipeline[i].program};
            args.insert(args.end(), pipeline[i].args.begin(), pipeline[i].args.end());
            for (auto& a : args) c_args.push_back(&a[0]);
            c_args.push_back(nullptr);

            string executablePath = pipeline[i].program;
            for (const auto& dir : PathManager::getTinyShellPath()) {
                string testPath = dir + "/" + pipeline[i].program;
                if (access(testPath.c_str(), X_OK) == 0) {
                    executablePath = testPath;
                    break;
                }
            }

            execvp(executablePath.c_str(), c_args.data());
            perror("execvp");
            exit(EXIT_FAILURE);
        } else {
            pids.push_back(pid);
            if (i > 0) close(prevReadFd);
            if (i < numCmds - 1) {
                close(pipefd[1]);
                prevReadFd = pipefd[0];
            }
        }
    }

    if (prevReadFd != -1) {
        close(prevReadFd);
    }

    if (isBackground) {
        string fullCmd = "";
        for(int i = 0; i < numCmds; i++) {
            fullCmd += pipeline[i].program;
            for(auto& a : pipeline[i].args) fullCmd += " " + a;
            if (i < numCmds - 1) fullCmd += " | ";
        }
        procManager.addProcess(pids.back(), fullCmd); 
    } else {
        CtrlCHandler::setForegroundProcess(pids.back());
        for (pid_t p : pids) {
            int status;
            waitpid(p, &status, WUNTRACED);
        }
        CtrlCHandler::resetForegroundProcess();
        cout << endl;
    }
}