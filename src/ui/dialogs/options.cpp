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

  DR_Settings newSettings = g_DR_settings;

  if (ImGui::Begin("Options", &newSettings.dlg_options, ImGuiWindowFlags_NoCollapse)) {

    const char* inactiveItems[] = { "Visible", "Transparent", "Hidden" };
    const char* distanceCheckItems[] = { "Visible", "Fade out", "Hidden" };

    ImGui::SeparatorText("General");
    ImGui::Checkbox("Disable autosaves", &newSettings.opt_disableAutoSave);
    ImGui::SetItemTooltip("Autosave interferes with saving/loading/freezing progress, so it's best to disable it.");
    ImGui::Checkbox("Try catching exceptions", &newSettings.opt_tryCatchExceptions);
    ImGui::Checkbox("Display while seeking", &newSettings.opt_displaySeeking);
    ImGui::SeparatorText("Zones");
    ImGui::Checkbox("Draw visual sets", &newSettings.opt_drawVisuals);
    ImGui::BeginDisabled(newSettings.opt_drawVisuals); {
      InputBitField("Draw collision zones", (unsigned long*)&newSettings.opt_drawCollisionZones, BITFIELD_DR_DISPLAYOPTIONS_ZONE, IM_ARRAYSIZE(BITFIELD_DR_DISPLAYOPTIONS_ZONE));

      // Inactive zone visibility dropdown
      int inactiveZoneSetting = static_cast<int>(newSettings.opt_inactiveZoneVisibility);
      if (ImGui::Combo("Inactive zone visibility", &inactiveZoneSetting, inactiveItems, IM_ARRAYSIZE(inactiveItems))) {
        g_DR_settings.opt_inactiveZoneVisibility = static_cast<InactiveItemVisibility>(inactiveZoneSetting);
      }

      ImGui::Checkbox("Draw no-collision objects", &newSettings.opt_drawNoCollisionObjects);
    }
    ImGui::EndDisabled();
    ImGui::Checkbox("Draw invisible objects", &newSettings.opt_drawInvisibleObjects);

    ImGui::SeparatorText("Transparent ZDR walls/slopes");
    ImGui::Checkbox("Enable", &newSettings.opt_transparentZDRSlopes);
    
    ImGui::BeginDisabled(!newSettings.opt_transparentZDRSlopes);

    ImGui::Checkbox("Invert", &newSettings.opt_transparentZDRSlopesInvert);
    ImGui::DragFloatRange2("Min/Max Z component", &newSettings.opt_transparentZDRSlopesMin, &newSettings.opt_transparentZDRSlopesMax, 0.01f, -1.0f, 1.0f);
    
    if (ImGui::Button("Hide walls")) {
      newSettings.opt_transparentZDRSlopesMin = -0.7071067812f;
      newSettings.opt_transparentZDRSlopesMax = 0.7071067812f;
      newSettings.opt_transparentZDRSlopesInvert = FALSE;
    } ImGui::SameLine();
    if (ImGui::Button("Hide floors")) {
      newSettings.opt_transparentZDRSlopesMin = 0.7071067812f;
      newSettings.opt_transparentZDRSlopesMax = 1.0f;
      newSettings.opt_transparentZDRSlopesInvert = FALSE;
    } ImGui::SameLine();
    if (ImGui::Button("Hide ceilings")) {
      newSettings.opt_transparentZDRSlopesMin = -1.0f;
      newSettings.opt_transparentZDRSlopesMax = -0.7071067812f;
      newSettings.opt_transparentZDRSlopesInvert = FALSE;
    }

    if (ImGui::Button("Only walls")) {
      newSettings.opt_transparentZDRSlopesMin = -0.7071067812f;
      newSettings.opt_transparentZDRSlopesMax = 0.7071067812f;
      newSettings.opt_transparentZDRSlopesInvert = TRUE;
    } ImGui::SameLine();
    if (ImGui::Button("Only floors")) {
      newSettings.opt_transparentZDRSlopesMin = 0.7071067812f;
      newSettings.opt_transparentZDRSlopesMax = 1.0f;
      newSettings.opt_transparentZDRSlopesInvert = TRUE;
    } ImGui::SameLine();
    if (ImGui::Button("Only ceilings")) {
      newSettings.opt_transparentZDRSlopesMin = -1.0f;
      newSettings.opt_transparentZDRSlopesMax = -0.7071067812f;
      newSettings.opt_transparentZDRSlopesInvert = TRUE;
    }
    if (ImGui::Button("Only glideable angles")) {
      newSettings.opt_transparentZDRSlopesMin = 0.420f;
      newSettings.opt_transparentZDRSlopesMax = 0.7071067812f;
      newSettings.opt_transparentZDRSlopesInvert = TRUE;
    }
    if (ImGui::Button("Only GLM walls")) {
      newSettings.opt_transparentZDRSlopesMin = 0.01f;
      newSettings.opt_transparentZDRSlopesMax = 0.7071067812f;
      newSettings.opt_transparentZDRSlopesInvert = TRUE;
    }
    ImGui::SameLine();
    if (ImGui::Button("Only GLM floors")) {
      newSettings.opt_transparentZDRSlopesMin = 0.7071067812f;
      newSettings.opt_transparentZDRSlopesMax = 0.910f;
      newSettings.opt_transparentZDRSlopesInvert = TRUE;
    }

    ImGui::DragFloat("Alpha", &newSettings.opt_transparentZDRSlopesAlpha, 0.01f, 0.f, 1.f);
    ImGui::EndDisabled();

    ImGui::SeparatorText("Sectors");
    ImGui::Checkbox("Draw virtual sectors", &newSettings.opt_drawVirtualSectors);
    ImGui::Checkbox("Draw sector borders", &newSettings.opt_drawSectorBorders);

    // Inactive sector visibility dropdown
    int inactiveSectorSetting = static_cast<int>(newSettings.opt_inactiveSectorVisibility);
    if (ImGui::Combo("Inactive sector visibility", &inactiveSectorSetting, inactiveItems, IM_ARRAYSIZE(inactiveItems))) {
      newSettings.opt_inactiveSectorVisibility = static_cast<InactiveItemVisibility>(inactiveSectorSetting);
    }

    ImGui::SeparatorText("Too Far Limit");
    ImGui::Checkbox("Show TooFarLimit (Animations)", &newSettings.opt_showTooFarLimitAnim);
    ImGui::Checkbox("Show TooFarLimit (Mechanics)", &newSettings.opt_showTooFarLimitMeca);
    ImGui::Checkbox("Show TooFarLimit (AI)", &newSettings.opt_showTooFarLimitAI);
    ImGui::DragIntRange2("TooFarLimit size filter", &newSettings.opt_tooFarLimitMinSize, &newSettings.opt_tooFarLimitMaxSize, 1.0f, 0, 255, "Min: %d", "Max: %d");
    ImGui::SliderFloat("TooFarLimit visibility range", &newSettings.opt_tooFarLimitHideRange, 0.0f, 100.0f);

    ImGui::SeparatorText("Distance checks");
    int distanceCheckSetting = static_cast<int>(newSettings.opt_distanceCheckVisibility);
    if (ImGui::Combo("Show distance checks", &distanceCheckSetting, distanceCheckItems, IM_ARRAYSIZE(distanceCheckItems))) {
      newSettings.opt_distanceCheckVisibility = static_cast<DistanceCheckVisibility>(distanceCheckSetting);
    }

    if (g_DR_settings != newSettings) {
      g_DR_settings = newSettings;
      DR_SaveSettings();
    }
  }
  ImGui::End();
}