#pragma once
#include <string>
#include <map>

class Environment {
private:
    static std::map<std::string, std::string> envVars;
public:
    static void setVar(const std::string& key, const std::string& value);
    static std::string getVar(const std::string& key);
    static void printVars();
    static void unsetVar(const std::string& key);
};