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
    const char* distanceCheckItems[] = { "Visible", "Fade out", "Hidden" };

    ImGui::SeparatorText("General");
    ImGui::Checkbox("Disable autosaves", &g_DR_settings.opt_disableAutoSave);
    ImGui::SetItemTooltip("Autosave interferes with saving/loading/freezing progress, so it's best to disable it.");
    ImGui::Checkbox("Try catching exceptions", &g_DR_settings.opt_tryCatchExceptions); 
    ImGui::SeparatorText("Zones");
    ImGui::Checkbox("Draw visual sets", &g_DR_settings.opt_drawVisuals);
    ImGui::BeginDisabled(g_DR_settings.opt_drawVisuals); {
      InputBitField("Draw collision zones", (unsigned long*)&g_DR_settings.opt_drawCollisionZones, BITFIELD_DR_DISPLAYOPTIONS_ZONE, IM_ARRAYSIZE(BITFIELD_DR_DISPLAYOPTIONS_ZONE));

      // Inactive zone visibility dropdown
      int inactiveZoneSetting = static_cast<int>(g_DR_settings.opt_inactiveZoneVisibility);
      if (ImGui::Combo("Inactive zone visibility", &inactiveZoneSetting, inactiveItems, IM_ARRAYSIZE(inactiveItems))) {
        g_DR_settings.opt_inactiveZoneVisibility = static_cast<InactiveItemVisibility>(inactiveZoneSetting);
      }

      ImGui::Checkbox("Draw no-collision objects", &g_DR_settings.opt_drawNoCollisionObjects);
    }
    ImGui::EndDisabled();
    ImGui::Checkbox("Draw invisible objects", &g_DR_settings.opt_drawInvisibleObjects);

    ImGui::SeparatorText("Transparent ZDR walls/slopes");
    ImGui::Checkbox("Enable", &g_DR_settings.opt_transparentZDRSlopes);
    
    ImGui::BeginDisabled(!g_DR_settings.opt_transparentZDRSlopes);

    ImGui::Checkbox("Invert", &g_DR_settings.opt_transparentZDRSlopesInvert);
    ImGui::DragFloatRange2("Min/Max Z component", &g_DR_settings.opt_transparentZDRSlopesMin, &g_DR_settings.opt_transparentZDRSlopesMax, 0.01f, -1.0f, 1.0f);
    
    if (ImGui::Button("Hide walls")) {
      g_DR_settings.opt_transparentZDRSlopesMin = -0.7071067812f;
      g_DR_settings.opt_transparentZDRSlopesMax = 0.7071067812f;
      g_DR_settings.opt_transparentZDRSlopesInvert = FALSE;
    } ImGui::SameLine();
    if (ImGui::Button("Hide floors")) {
      g_DR_settings.opt_transparentZDRSlopesMin = 0.7071067812f;
      g_DR_settings.opt_transparentZDRSlopesMax = 1.0f;
      g_DR_settings.opt_transparentZDRSlopesInvert = FALSE;
    } ImGui::SameLine();
    if (ImGui::Button("Hide ceilings")) {
      g_DR_settings.opt_transparentZDRSlopesMin = -1.0f;
      g_DR_settings.opt_transparentZDRSlopesMax = -0.7071067812f;
      g_DR_settings.opt_transparentZDRSlopesInvert = FALSE;
    }

    if (ImGui::Button("Only walls")) {
      g_DR_settings.opt_transparentZDRSlopesMin = -0.7071067812f;
      g_DR_settings.opt_transparentZDRSlopesMax = 0.7071067812f;
      g_DR_settings.opt_transparentZDRSlopesInvert = TRUE;
    } ImGui::SameLine();
    if (ImGui::Button("Only floors")) {
      g_DR_settings.opt_transparentZDRSlopesMin = 0.7071067812f;
      g_DR_settings.opt_transparentZDRSlopesMax = 1.0f;
      g_DR_settings.opt_transparentZDRSlopesInvert = TRUE;
    } ImGui::SameLine();
    if (ImGui::Button("Only ceilings")) {
      g_DR_settings.opt_transparentZDRSlopesMin = -1.0f;
      g_DR_settings.opt_transparentZDRSlopesMax = -0.7071067812f;
      g_DR_settings.opt_transparentZDRSlopesInvert = TRUE;
    }
    if (ImGui::Button("Only glideable angles")) {
      g_DR_settings.opt_transparentZDRSlopesMin = 0.420f;
      g_DR_settings.opt_transparentZDRSlopesMax = 0.7071067812f;
      g_DR_settings.opt_transparentZDRSlopesInvert = TRUE;
    }
    if (ImGui::Button("Only GLM walls")) {
      g_DR_settings.opt_transparentZDRSlopesMin = 0.01f;
      g_DR_settings.opt_transparentZDRSlopesMax = 0.7071067812f;
      g_DR_settings.opt_transparentZDRSlopesInvert = TRUE;
    }
    ImGui::SameLine();
    if (ImGui::Button("Only GLM floors")) {
      g_DR_settings.opt_transparentZDRSlopesMin = 0.7071067812f;
      g_DR_settings.opt_transparentZDRSlopesMax = 0.910f;
      g_DR_settings.opt_transparentZDRSlopesInvert = TRUE;
    }

    ImGui::DragFloat("Alpha", &g_DR_settings.opt_transparentZDRSlopesAlpha, 0.01f, 0.f, 1.f);
    ImGui::EndDisabled();

    ImGui::SeparatorText("Sectors");
    ImGui::Checkbox("Draw virtual sectors", &g_DR_settings.opt_drawVirtualSectors);
    ImGui::Checkbox("Draw sector borders", &g_DR_settings.opt_drawSectorBorders);

    // Inactive sector visibility dropdown
    int inactiveSectorSetting = static_cast<int>(g_DR_settings.opt_inactiveSectorVisibility);
    if (ImGui::Combo("Inactive sector visibility", &inactiveSectorSetting, inactiveItems, IM_ARRAYSIZE(inactiveItems))) {
      g_DR_settings.opt_inactiveSectorVisibility = static_cast<InactiveItemVisibility>(inactiveSectorSetting);
    }

    ImGui::SeparatorText("Too Far Limit");
    ImGui::Checkbox("Show TooFarLimit (Animations)", &g_DR_settings.opt_showTooFarLimitAnim);
    ImGui::Checkbox("Show TooFarLimit (Mechanics)", &g_DR_settings.opt_showTooFarLimitMeca);
    ImGui::Checkbox("Show TooFarLimit (AI)", &g_DR_settings.opt_showTooFarLimitAI);
    ImGui::DragIntRange2("TooFarLimit size filter", &g_DR_settings.opt_tooFarLimitMinSize, &g_DR_settings.opt_tooFarLimitMaxSize, 1.0f, 0, 255, "Min: %d", "Max: %d");
    ImGui::SliderFloat("TooFarLimit visibility range", &g_DR_settings.opt_tooFarLimitHideRange, 0.0f, 100.0f);

    ImGui::SeparatorText("Distance checks");
    int distanceCheckSetting = static_cast<int>(g_DR_settings.opt_distanceCheckVisibility);
    if (ImGui::Combo("Show distance checks", &distanceCheckSetting, distanceCheckItems, IM_ARRAYSIZE(distanceCheckItems))) {
      g_DR_settings.opt_distanceCheckVisibility = static_cast<DistanceCheckVisibility>(distanceCheckSetting);
    }

  }
  ImGui::End();
}