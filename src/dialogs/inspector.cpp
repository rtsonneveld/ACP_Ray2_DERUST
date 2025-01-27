#include "inspector.h"
#include "inspector_actor.h"

#include <sstream>
#include "ui.h"
#include "ui_util.h"

// Include last
#include "derust.h"

bool DR_DLG_Inspector_Enabled = FALSE;

void DR_DLG_Inspector_Draw() {

  if (!DR_DLG_Inspector_Enabled) return;

  bool open = true;
  if (ImGui::Begin("Inspector", &DR_DLG_Inspector_Enabled, ImGuiWindowFlags_NoCollapse)) {

    if (g_DR_selectedObject != nullptr) {
      HIE_tdstSuperObject* spo = g_DR_selectedObject; // Alias

      ImGui::Text(SPO_Name(spo).c_str());

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