import json

# Read data from file
with open("comports.json", "r") as file:
    data = json.load(file)

cpp_template = """
#include "comportNames.hpp"

const std::unordered_map<std::string, std::vector<std::string>> comportNames = {{
{}
}};

const std::unordered_map<std::string, std::vector<std::string>> reflexNames = {{
{}
}};
"""

comport_entries = []
reflex_entries = []

for model in data["aiModels"]:
    name = model["name"]
    rules = "{ " + ", ".join(f'"{rule}"' for rule in model["rules"]) + " }"
    reflexes = "{ " + ", ".join(f'"{reflex}"' for reflex in model["reflexes"]) + " }"
    comport_entries.append(f'    {{"{name}", {rules}}}')
    reflex_entries.append(f'    {{"{name}", {reflexes}}}')

cpp_content = cpp_template.format(
    ",\n".join(comport_entries),
    ",\n".join(reflex_entries)
)

# Write output to file
with open("comportNames.cpp", "w") as file:
    file.write(cpp_content)