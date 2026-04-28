#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <sys/types.h>

struct ProcessHistoryEntry {
    pid_t pid;
    std::string processName;
    std::time_t startTime;
    std::time_t endTime;
};

class ProcessHistory {
public:
    static void addProcess(pid_t pid, const std::string& processName);
    static void updateEndTime(pid_t pid);
    static void displayHistory();
    static std::string formatTimestamp(std::time_t timestamp);

private:
    static std::vector<ProcessHistoryEntry> processHistory;
};

std::time_t getCurrentTime();