#include "hierarchy.h"
#include <ImGui.h>
#include <ACP_Ray2.h>
#include <stdio.h>
#include <sstream>

bool DR_DLG_Hierarchy_Enabled = FALSE;

void DR_DLG_Hierarchy_SPO(HIE_tdstSuperObject* spo, const char* name) {
  int childCount = LST_M_DynamicGetNbOfElements(spo);

    ImGuiTreeNodeFlags flags =
    ImGuiTreeNodeFlags_OpenOnArrow |
    ImGuiTreeNodeFlags_FramePadding |
    ImGuiTreeNodeFlags_SpanFullWidth;

  if (childCount == 0) {
    flags |= ImGuiTreeNodeFlags_Leaf;
  }

  /*bool isSelected = Editor.SelectedActorID == actor.ID;

  if (isSelected) {
    flags |= ImGuiTreeNodeFlags_Selected;
  }
  #endregion*/

  ImGui::SetNextItemWidth(ImGui::GetWindowWidth());

  bool nodeOpen;

  /*
  if (Editor.SelectedActor ? .IsChildOf(actor.ID) ? ? false) {
    ImGui::SetNextItemOpen(true);
  }
  */

  std::stringstream label;

  if (name != nullptr) {
    label << name;
  }

  if (spo->ulType == HIE_C_Type_Actor) {

    GAM_tdxObjectType lPersonalType = HIE_M_lActorGetPersonalType(HIE_M_hSuperObjectGetActor(spo));

    if (lPersonalType == GAM_C_InvalidObjectType || lPersonalType >= GAM_C_AlwaysObjectType) {
      label << "(Always) #" << (lPersonalType - GAM_C_AlwaysObjectType);
    } else {
      auto name = HIE_fn_szGetObjectPersonalName(spo);
      if (name != nullptr) {
        label << name;
      } else {
        label << "null";
      }
    }
  } else {
    switch (spo->ulType) {
      case HIE_C_Type_Sector: label << "Sector"; break;
      case HIE_C_Type_IPO: label << "IPO"; break;
      case HIE_C_Type_PO: label << "PO"; break;
      case HIE_C_Type_Actor: label << "Actor"; break;
    }
  }

  label << "##tree_" << (int)spo;
  nodeOpen = ImGui::TreeNodeEx(label.str().c_str(), flags);

  bool focused = ImGui::IsItemFocused();

  if (ImGui::IsItemClicked()) {
    //Editor.SelectedActor = actor;
    //Editor.FocusSelectedActor();
  }

  //CheckContextMenu(actor);

  if (nodeOpen) {

    HIE_tdstSuperObject* child;
    LST_M_DynamicForEach(spo, child) {
      DR_DLG_Hierarchy_SPO(child, nullptr);
    }

    ImGui::TreePop();
  }
}

void DR_DLG_Hierarchy_Draw() {

  if (!DR_DLG_Hierarchy_Enabled) return;

  ImGuiWindowClass windowClass;
  windowClass.ViewportFlagsOverrideSet = ImGuiViewportFlags_TopMost | ImGuiViewportFlags_NoTaskBarIcon;
  ImGui::SetNextWindowClass(&windowClass);

  ImGui::SetNextWindowPos(ImVec2(200, 200), ImGuiCond_Once);
  ImGui::SetNextWindowSize(ImVec2(200, 500), ImGuiCond_Once);

  if (ImGui::Begin("Hierarchy", &DR_DLG_Hierarchy_Enabled, ImGuiWindowFlags_NoCollapse)) {
    DR_DLG_Hierarchy_SPO(*GAM_g_p_stDynamicWorld, "Dynamic World");
    DR_DLG_Hierarchy_SPO(*GAM_g_p_stInactiveDynamicWorld, "Inactive Dynamic World");
    DR_DLG_Hierarchy_SPO(*GAM_g_p_stFatherSector, "Father Sector");
  }
  ImGui::End();
}