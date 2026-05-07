#pragma once
#include<string>
#include<vector>
using namespace std;

struct Command{
    string program;
    vector<string> args;
    bool isBackground;
};

class CommandParser{
private:
    static Command parseSingle(const string& input);

public:
    static vector<Command> parsePipeline(const string& input);
};