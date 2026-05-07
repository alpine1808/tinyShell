#pragma once
#include <string>
#include <vector>

class ProcessManager; 

using namespace std;

struct Command{
    string program;
    vector<string> args;
    bool isBackground;
};

class CommandParser{
private:
    static string expandVariables(const string& input, ProcessManager& procManager);
    static Command parseSingle(const string& input, ProcessManager& procManager);

public:
    static vector<Command> parsePipeline(const string& input, ProcessManager& procManager);
};