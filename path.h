#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include "commandParser.h"
namespace fs = std::filesystem;

class PathManager {
public:
    static void executeCommand(const Command& cmd);

private:
    static std::vector<std::string> tinyShellPath; 
    static void printWorkingDirectory(const std::vector<std::string>& args);
    static void managePath(const std::vector<std::string>& args);
};