#ifndef BUILTINS_H
#define BUILTINS_H

#include <vector>
#include <string>
#include <string_view> 

void builtin_help();
void builtin_cd(const std::vector<std::string>& args);
bool builtin_exit() noexcept; 
void builtin_alias(const std::vector<std::string>& args);
void builtin_unalias(const std::vector<std::string>& args);
void builtin_echo(const std::vector<std::string>& args);

bool is_alias(std::string_view name) noexcept;
std::string resolve_alias(std::string_view name);

bool handle_builtin(std::vector<std::string>& args); 

#endif