#include "inspector.hpp"
#include "inspector_actor.hpp"

#include <sstream>
#include "ui/ui.hpp"
#include "ui/ui_util.hpp"
#include "ui/custominputs.hpp"
#include <vector>

// C INCLUDE
#include <mod/globals.h>

#include <ACP_Ray2.h>

bool DR_DLG_Inspector_Enabled = FALSE;
bool DR_DLG_Inspector_DebugSphereEnabled = FALSE;
float DR_DLG_Inspector_DebugSphereRadius = 1.0f;

void DR_DLG_Inspector_Draw() {

  if (!DR_DLG_Inspector_Enabled) return;

  bool open = true;
  if (ImGui::Begin("Inspector", &DR_DLG_Inspector_Enabled, ImGuiWindowFlags_NoCollapse)) {

    if (g_DR_selectedObject != nullptr) {
      HIE_tdstSuperObject* spo = g_DR_selectedObject; // Alias

      ImGui::Text(SPO_Name(spo).c_str());

      ImGui::DragFloat3("GlobalMatrix.Position", (float*)&spo->p_stGlobalMatrix->stPos);
      ImGui::DragFloat3("LocalMatrix.Position",  (float*)&spo->p_stLocalMatrix->stPos);

      InputBitField("SuperObject Flags", (unsigned long*)&spo->ulFlags, BITFIELD_SPOFLAGS, IM_ARRAYSIZE(BITFIELD_SPOFLAGS));

      ImGui::Checkbox("Debug Sphere", &DR_DLG_Inspector_DebugSphereEnabled);
      if (DR_DLG_Inspector_DebugSphereEnabled) {
        ImGui::InputFloat("Radius", &DR_DLG_Inspector_DebugSphereRadius);
      }

      switch (spo->ulType) {
        case HIE_C_Type_Unknown: break;
        case HIE_C_Type_Actor: DR_DLG_Inspector_Draw_Actor(spo); break;
      }
    }
    else {
      ImGui::Text("Select a superobject in the hierarchy");
    }
  }
  ImGui::End();
}