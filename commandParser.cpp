#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <unistd.h>
#include <sys/wait.h>
#include "commandParser.h"
#include "environment.h"
#include "process_manager.h"

using namespace std;

extern void processLine(const string& input, ProcessManager& procManager);

string CommandParser::expandVariables(const string& input, ProcessManager& procManager) {
    string result = "";
    int i = 0;
    char quoteType = '\0';
    
    while (i < input.length()) {
        char c = input[i];
        if ((c == '\'' || c == '\"') && quoteType == '\0') {
            quoteType = c;
            result += c;
            i++;
        } else if (c == quoteType) {
            quoteType = '\0';
            result += c;
            i++;
        } else if (c == '$' && i + 1 < input.length() && input[i+1] == '(' && quoteType != '\'') {
            i += 2;
            string subCmd = "";
            int parenCount = 1;
            char subQuoteType = '\0';
            
            while (i < input.length() && parenCount > 0) {
                char sc = input[i];
                if ((sc == '\'' || sc == '\"') && subQuoteType == '\0') {
                    subQuoteType = sc;
                } else if (sc == subQuoteType) {
                    subQuoteType = '\0';
                } else if (sc == '(' && subQuoteType == '\0') {
                    parenCount++;
                } else if (sc == ')' && subQuoteType == '\0') {
                    parenCount--;
                }
                
                if (parenCount > 0) {
                    subCmd += sc;
                }
                i++;
            }
            
            int pipefd[2];
            if (pipe(pipefd) == 0) {
                pid_t pid = fork();
                if (pid == 0) {
                    close(pipefd[0]);
                    dup2(pipefd[1], STDOUT_FILENO);
                    close(pipefd[1]);
                    processLine(subCmd, procManager);
                    exit(EXIT_SUCCESS);
                } else {
                    close(pipefd[1]);
                    char buffer[128];
                    ssize_t bytesRead;
                    string subResult = "";
                    while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer)-1)) > 0) {
                        buffer[bytesRead] = '\0';
                        subResult += buffer;
                    }
                    close(pipefd[0]);
                    waitpid(pid, nullptr, 0);
                    
                    while(!subResult.empty() && subResult.back() == '\n') {
                        subResult.pop_back();
                    }
                    
                    for (char& ch : subResult) {
                        if (ch == '\n') ch = ' ';
                    }
                    
                    result += subResult;
                }
            }
        } else if (c == '$' && quoteType != '\'') {
            i++;
            string varName = "";
            while (i < input.length() && (isalnum(input[i]) || input[i] == '_')) {
                varName += input[i];
                i++;
            }
            if (!varName.empty()) {
                result += Environment::getVar(varName);
            } else {
                result += "$";
            }
        } else {
            result += c;
            i++;
        }
    }
    return result;
}

Command CommandParser::parseSingle(const string& inputRaw, ProcessManager& procManager) {
    string input = expandVariables(inputRaw, procManager); 
    string token = "";
    vector<string> tokens;
    char quoteType = '\0';

    for (int i = 0; i < input.length(); i++) {
        char c = input[i];
        if ((c == '\"' || c == '\'') && quoteType == '\0') {
            quoteType = c;
        } else if (c == quoteType) {
            quoteType = '\0';
        } else if (c == ' ' && quoteType == '\0') {
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

vector<Command> CommandParser::parsePipeline(const string& inputRaw, ProcessManager& procManager) {
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
    char quoteType = '\0';
    int parenCount = 0;
    
    for (int i = 0; i < input.length(); i++) {
        char c = input[i];
        
        if ((c == '\"' || c == '\'') && quoteType == '\0') {
            quoteType = c;
            currentCmdStr += c;
        } else if (c == quoteType) {
            quoteType = '\0';
            currentCmdStr += c;
        } else if (c == '$' && i + 1 < input.length() && input[i+1] == '(' && quoteType == '\0') {
            parenCount++;
            currentCmdStr += c;
            currentCmdStr += input[i+1];
            i++;
        } else if (c == '(' && parenCount > 0 && quoteType == '\0') {
            parenCount++;
            currentCmdStr += c;
        } else if (c == ')' && parenCount > 0 && quoteType == '\0') {
            parenCount--;
            currentCmdStr += c;
        } else if (c == '|' && quoteType == '\0' && parenCount == 0) {
            Command cmd = parseSingle(currentCmdStr, procManager);
            cmd.isBackground = isBackground; 
            pipeline.push_back(cmd);
            currentCmdStr = "";
        } else {
            currentCmdStr += c;
        }
    }
    
    if (!currentCmdStr.empty()) {
        Command cmd = parseSingle(currentCmdStr, procManager);
        cmd.isBackground = isBackground;
        pipeline.push_back(cmd);
    }

    return pipeline;
}