#include "engineglobals.hpp"
#include "ui/ui.hpp"
#include "ui/custominputs.hpp"
#include "ui/settings.hpp"
#include "mod/globals.h"
#include <ACP_Ray2.h>

void DR_DLG_EngineGlobals_Draw() {

  if (!g_DR_settings.dlg_engineglobals) return;

  if (ImGui::Begin("Globals", &g_DR_settings.dlg_engineglobals, ImGuiWindowFlags_NoCollapse)) {

    ImGui::Text("(DR) Total frames: %lu", g_DR_totalFrameCount);
    ImGui::Text("(DR) Current map frames: %lu", g_DR_currentMapFrameCount);

    ImGui::BeginTabBar("engineGlobalsTabs");

    if (ImGui::BeginTabItem("Engine Structure")) {

      DR_DLG_EngineGlobals_Draw_EngineStructure();

      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Collision Report")) {

      DR_DLG_EngineGlobals_Draw_CollisionReport();

      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Input Structure")) {

      DR_DLG_EngineGlobals_Draw_InputStructure();

      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Always")) {

      DR_DLG_EngineGlobals_Draw_Always();

      ImGui::EndTabItem();
    }

    ImGui::EndTabBar();

  }
  ImGui::End();
}
