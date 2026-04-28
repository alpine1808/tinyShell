#include<sstream>
#include<string>
#include "commandParser.h"
using namespace std;

Command CommandParser::parse(const string& inputRaw){
    string input = inputRaw;
    bool isBackground = false;
    string token="";
    vector<string> tokens;
    bool inQuotes = false;

    while(!input.empty() && input.back() == ' ') {
        input.pop_back();
    }

    if(!input.empty() && input.back() == '&') {
        isBackground = true;
        input.pop_back();
    }

    for(int i=0;i<input.length();i++){
        char c = input[i];
        if(c=='\"'){
            inQuotes = !inQuotes;
        }else if(c==' ' && !inQuotes){
            if (!token.empty()) {
                tokens.push_back(token);
                token = "";
            }
        }else {
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
        isBackground
    };
}