#pragma once

#include "commandParser.h"
#include "process_manager.h"

class ProcessExecutor {
public:
    static void execute(const Command& cmd, ProcessManager& procManager);
};