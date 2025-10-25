#include "options.hpp"
#include "ui/ui.hpp"
#include "ui/custominputs.hpp"

// C INCLUDE

#include <ACP_Ray2.h>
#include <windows.h>

bool DR_DLG_Options_Enabled = FALSE;

bool opt_drawVisuals = FALSE;
unsigned int opt_drawCollisionZones = (int)CollisionZoneMask::ZDM | (int)CollisionZoneMask::ZDR | (int)CollisionZoneMask::ZDE | (int)CollisionZoneMask::ZDD;
bool opt_transparentZDRWalls = FALSE;
bool opt_drawNoCollisionObjects = FALSE;
bool opt_drawInvisibleObjects = FALSE;
InactiveSectorVisibility opt_inactiveSectorVisibility = InactiveSectorVisibility::Transparent;

bool IsCollisionZoneEnabled(CollisionZoneMask zone)
{
  if (opt_drawVisuals) return false;
  return (opt_drawCollisionZones & static_cast<unsigned int>(zone)) != 0;
}

void DR_DLG_Options_Draw() {

  if (!DR_DLG_Options_Enabled) return;

  bool open = true;
  if (ImGui::Begin("Options", &DR_DLG_Options_Enabled, ImGuiWindowFlags_NoCollapse)) {

    ImGui::Checkbox("Draw visual sets", &opt_drawVisuals);
    if (opt_drawVisuals) ImGui::BeginDisabled();
    InputBitField("Draw collision zones", (unsigned long*)&opt_drawCollisionZones, BITFIELD_DR_DISPLAYOPTIONS_ZONE, IM_ARRAYSIZE(BITFIELD_DR_DISPLAYOPTIONS_ZONE));
    ImGui::Checkbox("Transparent ZDR walls", &opt_transparentZDRWalls);
    ImGui::Checkbox("Draw no-collision objects", &opt_drawNoCollisionObjects);
    if (opt_drawVisuals) ImGui::EndDisabled();
    ImGui::Checkbox("Draw invisible objects", &opt_drawInvisibleObjects);

    // Inactive sector visibility dropdown
    int inactiveIndex = static_cast<int>(opt_inactiveSectorVisibility);
    const char* inactiveItems[] = { "Visible", "Transparent", "Hidden" }; 
    if (ImGui::Combo("Inactive sector visibility", &inactiveIndex, inactiveItems, IM_ARRAYSIZE(inactiveItems))) {
      opt_inactiveSectorVisibility = static_cast<InactiveSectorVisibility>(inactiveIndex);
    }

  }
  ImGui::End();
}