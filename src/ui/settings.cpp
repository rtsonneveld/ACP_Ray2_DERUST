#include "settings.hpp"
#include "json.hpp"
#include <iostream>
#include <fstream>

using json = nlohmann::json;

DR_Settings g_DR_settings;

static const char* SETTINGS_FILE = "derustSettings.json";

void DR_SaveSettings() {
  json j = g_DR_settings; // automatic via to_json

  std::ofstream file(SETTINGS_FILE, std::ios::out | std::ios::trunc);
  if (!file.is_open()) {
    std::cerr << "Failed to open " << SETTINGS_FILE << " for writing.\n";
    return;
  }

  try {
    file << j.dump(4); // pretty print
  }
  catch (const std::exception& e) {
    std::cerr << "Error saving settings: " << e.what() << "\n";
  }
}

void DR_LoadSettings() {
  std::ifstream file(SETTINGS_FILE);

  if (!file.is_open()) {
    std::cerr << "Settings file not found. Using default settings.\n";
    DR_SaveSettings(); // create new file with defaults
    return;
  }

  try {
    json j;
    file >> j;
    file.close();

    // Automatic merge with defaults via from_json
    g_DR_settings = j.get<DR_Settings>();

  }
  catch (const std::exception& e) {
    std::cerr << "Error loading settings: " << e.what() << "\n";
    std::cerr << "Using default settings.\n";
    g_DR_settings = DR_Settings(); // fallback to defaults
    DR_SaveSettings(); // overwrite corrupted file with defaults
  }
}