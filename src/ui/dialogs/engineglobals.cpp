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

    /*
    *IPT_g_FieldPadGlobalVectorX = 0;
    *IPT_g_FieldPadGlobalVectorY = 0;
    *IPT_g_FieldPadGlobalVectorZ = 0;
    *IPT_g_FieldPadHorizontalAxis = 0;
    *IPT_g_FieldPadVerticalAxis = 0;
    *IPT_g_FieldPadAnalogForce = 0;
    *IPT_g_FieldPadTrueAnalogForce = 0;
    *IPT_g_FieldPadRotationAngle = 0;
    *IPT_g_FieldPadSector = 0;*/
    ImGui::Text("PadAnalogForce = %.2f", *IPT_g_FieldPadAnalogForce);
    ImGui::Text("PadTrueAnalogForce = %.2f", *IPT_g_FieldPadTrueAnalogForce);

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
