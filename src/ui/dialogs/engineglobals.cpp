#include "engineglobals.hpp"
#include "ui/ui.hpp"
#include "ui/custominputs.hpp"
#include "ui/settings.hpp"
#include <ACP_Ray2.h>

void DR_DLG_EngineGlobals_Draw() {

  if (!g_DR_settings.dlg_engineglobals) return;

  if (ImGui::Begin("Globals", &g_DR_settings.dlg_playback, ImGuiWindowFlags_NoCollapse)) {
    
    ImGui::BeginTabBar("engineGlobalsTabs");

    if (ImGui::BeginTabItem("Engine Structure")) {

      DR_DLG_EngineGlobals_Draw_EngineStructure();

      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Collision Report")) {

      DR_DLG_EngineGlobals_Draw_CollisionReport();

      ImGui::EndTabItem();
    }

    ImGui::EndTabBar();

  }
  ImGui::End();
}
