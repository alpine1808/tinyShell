#pragma once

#include "commandParser.h"
#include "process_manager.h"

class ProcessExecutor {
public:
    static void executePipeline(const vector<Command>& pipeline, ProcessManager& procManager);
};