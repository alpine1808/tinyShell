#include "ctrl_c_handler.h"
#include "logger.h"
#include <iostream>
#include <string>
#include <signal.h>
#include <unistd.h>

pid_t CtrlCHandler::currentForegroundProcessPid = -1;

void CtrlCHandler::initialize() {
    struct sigaction sa;
    sa.sa_handler = handleSignal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    
    if (sigaction(SIGINT, &sa, nullptr) == -1) {
        log(LOG_ERROR, "Failed to set Ctrl+C handler");
    }
}

void CtrlCHandler::setForegroundProcess(pid_t pid) {
    currentForegroundProcessPid = pid;
    log(INFO, "Set foreground process PID: " + std::to_string(pid));
}

void CtrlCHandler::resetForegroundProcess() {
    currentForegroundProcessPid = -1;
}

bool CtrlCHandler::hasForegroundProcess() {
    return currentForegroundProcessPid != -1;
}

pid_t CtrlCHandler::getForegroundProcessPid() {
    return currentForegroundProcessPid;
}

void CtrlCHandler::handleSignal(int signum) {
    if (signum == SIGINT) {
        if (currentForegroundProcessPid != -1) {
            log(INFO, "Terminating process group: " + std::to_string(currentForegroundProcessPid));
            kill(-currentForegroundProcessPid, SIGINT);
            resetForegroundProcess();
            std::cout << std::endl;
        } else {
            std::cout << "^C\n";
            log(INFO, "Ctrl+C pressed (no foreground process to terminate)");
        }
    }
}