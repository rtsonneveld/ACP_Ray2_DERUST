#include "hierarchy.hpp"
#include <stdio.h>
#include <sstream>
#include "ui/ui.hpp"
#include "ui/ui_util.hpp"

// C INCLUDE
#include "mod/globals.h"

bool DR_DLG_Hierarchy_Enabled = false;

void DR_DLG_Hierarchy_SPO(HIE_tdstSuperObject* spo, const char* name) {
  int childCount = LST_M_DynamicGetNbOfElements(spo);

    ImGuiTreeNodeFlags flags =
    ImGuiTreeNodeFlags_OpenOnArrow |
    ImGuiTreeNodeFlags_FramePadding |
    ImGuiTreeNodeFlags_SpanFullWidth;

  if (childCount == 0) {
    flags |= ImGuiTreeNodeFlags_Leaf;
  }

  // If the selected actor was an always and is destroyed
  if (g_DR_selectedObject != NULL && g_DR_selectedObject->ulType == HIE_C_Type_Actor && g_DR_selectedObject->hLinkedObject.p_stActor->hStandardGame == NULL) {
    g_DR_selectedObject = NULL;
  }

  bool isSelected = g_DR_selectedObject == spo;

  if (isSelected) {
    flags |= ImGuiTreeNodeFlags_Selected;
  }

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
  } else {
    label << SPO_Name(spo);
  }
  label << "##tree_" << (int)spo;
  nodeOpen = ImGui::TreeNodeEx(label.str().c_str(), flags);

  bool focused = ImGui::IsItemFocused();

  if (ImGui::IsItemClicked()) {
    g_DR_selectedObject = spo;
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

  if (ImGui::Begin("Hierarchy", &DR_DLG_Hierarchy_Enabled, ImGuiWindowFlags_NoCollapse)) {
    DR_DLG_Hierarchy_SPO(*GAM_g_p_stDynamicWorld, "Dynamic World");
    DR_DLG_Hierarchy_SPO(*GAM_g_p_stInactiveDynamicWorld, "Inactive Dynamic World");
    DR_DLG_Hierarchy_SPO(*GAM_g_p_stFatherSector, "Father Sector");
  }
  ImGui::End();
}