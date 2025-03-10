#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <map>

std::string GetComportName(std::string aiModel, int comportIndex);
std::string GetReflexName(std::string aiModel, int comportIndex);

extern const std::unordered_map<std::string, std::vector<std::string>> comportNames;
extern const std::unordered_map<std::string, std::vector<std::string>> reflexNames;