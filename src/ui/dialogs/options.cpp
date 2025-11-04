#include "options.hpp"
#include "ui/ui.hpp"
#include "ui/custominputs.hpp"
#include "ui/settings.hpp"

// C INCLUDE

#include <ACP_Ray2.h>
#include <windows.h>

bool IsCollisionZoneEnabled(CollisionZoneMask zone)
{
  if (g_DR_settings.opt_drawVisuals) return false;
  return (g_DR_settings.opt_drawCollisionZones & static_cast<unsigned int>(zone)) != 0;
}

void DR_DLG_Options_Draw() {

  if (!g_DR_settings.dlg_options) return;

  if (ImGui::Begin("Options", &g_DR_settings.dlg_options, ImGuiWindowFlags_NoCollapse)) {

    const char* inactiveItems[] = { "Visible", "Transparent", "Hidden" };

    ImGui::Checkbox("Draw visual sets", &g_DR_settings.opt_drawVisuals);
    if (g_DR_settings.opt_drawVisuals) ImGui::BeginDisabled();
    InputBitField("Draw collision zones", (unsigned long*)&g_DR_settings.opt_drawCollisionZones, BITFIELD_DR_DISPLAYOPTIONS_ZONE, IM_ARRAYSIZE(BITFIELD_DR_DISPLAYOPTIONS_ZONE));

    // Inactive zone visibility dropdown
    int inactiveZoneSetting = static_cast<int>(g_DR_settings.opt_inactiveZoneVisibility);
    if (ImGui::Combo("Inactive zone visibility", &inactiveZoneSetting, inactiveItems, IM_ARRAYSIZE(inactiveItems))) {
      g_DR_settings.opt_inactiveZoneVisibility = static_cast<InactiveItemVisibility>(inactiveZoneSetting);
    }

    ImGui::Checkbox("Transparent ZDR walls", &g_DR_settings.opt_transparentZDRWalls);
    ImGui::Checkbox("Draw no-collision objects", &g_DR_settings.opt_drawNoCollisionObjects);
    if (g_DR_settings.opt_drawVisuals) ImGui::EndDisabled();
    ImGui::Checkbox("Draw invisible objects", &g_DR_settings.opt_drawInvisibleObjects);
    ImGui::Checkbox("Draw virtual sectors", &g_DR_settings.opt_drawVirtualSectors);
    ImGui::Checkbox("Draw sector borders", &g_DR_settings.opt_drawSectorBorders);

    // Inactive sector visibility dropdown
    int inactiveSectorSetting = static_cast<int>(g_DR_settings.opt_inactiveSectorVisibility);
    if (ImGui::Combo("Inactive sector visibility", &inactiveSectorSetting, inactiveItems, IM_ARRAYSIZE(inactiveItems))) {
      g_DR_settings.opt_inactiveSectorVisibility = static_cast<InactiveItemVisibility>(inactiveSectorSetting);
    }

    ImGui::Checkbox("Show TooFarLimit (Animations)", &g_DR_settings.opt_showTooFarLimitAnim);
    ImGui::Checkbox("Show TooFarLimit (Mechanics)", &g_DR_settings.opt_showTooFarLimitMeca);
    ImGui::Checkbox("Show TooFarLimit (AI)", &g_DR_settings.opt_showTooFarLimitAI);
    ImGui::DragIntRange2("TooFarLimit size filter", &g_DR_settings.opt_tooFarLimitMinSize, &g_DR_settings.opt_tooFarLimitMaxSize, 1.0f, 0, 255, "Min: %d", "Max: %d");
    ImGui::SliderFloat("TooFarLimit visibility range", &g_DR_settings.opt_tooFarLimitHideRange, 0.0f, 100.0f);

  }
  ImGui::End();
}