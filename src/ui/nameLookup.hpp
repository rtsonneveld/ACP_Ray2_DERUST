#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <map>

enum class NameType {
  AIModel_Comport,
  AIModel_Reflex,
  Family_State,
  Family_Anim3D,
  Level_Sector,
  COUNT
};

std::string NameFromIndex(NameType type, const std::string& container, int index);
int IndexFromName(NameType type, const std::string& container, const std::string& name);