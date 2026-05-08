#ifndef CTRL_C_HANDLER_H
#define CTRL_C_HANDLER_H

#include <sys/types.h>

class CtrlCHandler {
public:
    static void initialize();
    static void setForegroundProcess(pid_t pid);
    static void resetForegroundProcess();
    static bool hasForegroundProcess();
    static pid_t getForegroundProcessPid();

private:
    static void handleSignal(int signum);
    static volatile pid_t currentForegroundProcessPid;
};

#endif