#include "environment.h"
#include <iostream>
#include <stdlib.h>

std::map<std::string, std::string> Environment::envVars;

void Environment::setVar(const std::string& key, const std::string& value) {
    envVars[key] = value;
    setenv(key.c_str(), value.c_str(), 1);
}

std::string Environment::getVar(const std::string& key) {
    auto it = envVars.find(key);
    if (it != envVars.end()) {
        return it->second;
    }
    return "";
}

void Environment::printVars() {
    for (const auto& pair : envVars) {
        std::cout << pair.first << "=" << pair.second << "\n";
    }
}

void Environment::unsetVar(const std::string& key) {
    auto it = envVars.find(key);
    if (it != envVars.end()) {
        envVars.erase(it);
        unsetenv(key.c_str());
    }
}