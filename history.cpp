#include "history.h"
#include "logger.h"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

vector<ProcessHistoryEntry> ProcessHistory::processHistory;

time_t getCurrentTime() {
    return chrono::system_clock::to_time_t(chrono::system_clock::now());
}

void ProcessHistory::addProcess(pid_t pid, const string& processName) {
    ProcessHistoryEntry entry;
    entry.pid = pid;
    entry.processName = processName;
    entry.startTime = getCurrentTime();
    entry.endTime = -1;
    
    processHistory.push_back(entry);
}

void ProcessHistory::updateEndTime(pid_t pid) {
    for (auto& entry : processHistory) {
        if (entry.pid == pid && entry.endTime == -1) {
            entry.endTime = getCurrentTime();
            return;
        }
    }
}

string ProcessHistory::formatTimestamp(time_t timestamp) {
    if (timestamp == -1) {
        return "Running";
    }
    
    struct tm timeInfo;
    localtime_r(&timestamp, &timeInfo);
    
    stringstream ss;
    ss << put_time(&timeInfo, "%H:%M:%S %d-%m-%Y");
    return ss.str();
}

void ProcessHistory::displayHistory() {
    cout << "\nProcess History:\n";
    cout << setw(10) << left << "PID"
         << setw(30) << left << "Process Name"
         << setw(25) << left << "Start Time"
         << setw(25) << left << "End Time" << endl;
    cout << string(90, '=') << endl;
    
    for (const auto& entry : processHistory) {
        cout << setw(10) << left << entry.pid
             << setw(30) << left << entry.processName
             << setw(25) << left << formatTimestamp(entry.startTime)
             << setw(25) << left << formatTimestamp(entry.endTime) << endl;
    }
}