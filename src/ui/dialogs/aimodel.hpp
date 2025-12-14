#pragma once

#include <cstdint>

enum class SelectedComportType : std::uint8_t {
    Intelligence,
    Reflex,
    Macro
};

void DR_DLG_AIModel_Draw();

