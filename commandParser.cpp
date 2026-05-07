#include<sstream>
#include<string>
#include<vector>
#include "commandParser.h"
using namespace std;

Command CommandParser::parseSingle(const string& input) {
    string token = "";
    vector<string> tokens;
    bool inQuotes = false;

    for (int i = 0; i < input.length(); i++) {
        char c = input[i];
        if (c == '\"') {
            inQuotes = !inQuotes;
        } else if (c == ' ' && !inQuotes) {
            if (!token.empty()) {
                tokens.push_back(token);
                token = "";
            }
        } else {
            token += c;
        }
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }

    if (tokens.empty()) return {"", {}, false};

    return {
        tokens[0],
        vector<string>(tokens.begin() + 1, tokens.end()),
        false
    };
}

vector<Command> CommandParser::parsePipeline(const string& inputRaw) {
    vector<Command> pipeline;
    string input = inputRaw;
    bool isBackground = false;

    while(!input.empty() && input.back() == ' ') {
        input.pop_back();
    }

    if(!input.empty() && input.back() == '&') {
        isBackground = true;
        input.pop_back();
    }

    string currentCmdStr = "";
    bool inQuotes = false;
    
    for (int i = 0; i < input.length(); i++) {
        char c = input[i];
        if (c == '\"') {
            inQuotes = !inQuotes;
            currentCmdStr += c;
        } else if (c == '|' && !inQuotes) {
            Command cmd = parseSingle(currentCmdStr);
            cmd.isBackground = isBackground; 
            pipeline.push_back(cmd);
            currentCmdStr = "";
        } else {
            currentCmdStr += c;
        }
    }
    
    if (!currentCmdStr.empty()) {
        Command cmd = parseSingle(currentCmdStr);
        cmd.isBackground = isBackground;
        pipeline.push_back(cmd);
    }

    return pipeline;
}