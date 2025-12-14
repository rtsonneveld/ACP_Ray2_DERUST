#include "hierarchy.hpp"
#include <stdio.h>
#include <sstream>
#include "ui/ui.hpp"
#include "ui/ui_util.hpp"
#include "ui/settings.hpp"

// C INCLUDE
#include "mod/globals.h"

void DR_DLG_Hierarchy_SPO(HIE_tdstSuperObject* spo, const char* name, const std::string& search) {

  std::string spoName = (name != nullptr) ? name : SPO_Name(spo);
  std::string spoNameLower = spoName;
  std::transform(spoNameLower.begin(), spoNameLower.end(), spoNameLower.begin(), ::tolower);

  bool matches = (search.empty() || spoNameLower.find(search) != std::string::npos);

  // Check children for match
  bool childMatches = false;
  if (!search.empty()) {
    HIE_tdstSuperObject* child;
    LST_M_DynamicForEach(spo, child) {
      std::string childName = SPO_Name(child);
      std::transform(childName.begin(), childName.end(), childName.begin(), ::tolower);

      if (childName.find(search) != std::string::npos) {
        childMatches = true;
        break;
      }
    }
  }

  // If neither this node nor any child matches — hide it
  if (!matches && !childMatches) return;

  int childCount = LST_M_DynamicGetNbOfElements(spo);

    ImGuiTreeNodeFlags flags =
    ImGuiTreeNodeFlags_OpenOnArrow |
    ImGuiTreeNodeFlags_FramePadding |
    ImGuiTreeNodeFlags_SpanFullWidth;

  if (childCount == 0) {
    flags |= ImGuiTreeNodeFlags_Leaf;
  }

  bool appearActive = true;
  bool highlight = false;

  if (spo->ulType == HIE_C_Type_Actor) {
    if (!(spo->hLinkedObject.p_stActor->hStandardGame->ucMiscFlags & Std_C_MiscFlag_Active) && !(spo->hLinkedObject.p_stActor->hStandardGame->ucMiscFlags & Std_C_MiscFlag_AlwaysActive)) {
      appearActive = false;
    }
  } else if (spo->ulType == HIE_C_Type_Sector) {
    
    auto sector = spo->hLinkedObject.p_stSector;

    auto currentSector = GAM_g_stEngineStructure->g_hMainActor->hLinkedObject.p_stActor->hSectInfo->hCurrentSector->hLinkedObject.p_stSector;

    appearActive = false;
    if (sector == currentSector) {

      appearActive = true;
      highlight = true;
    }
    else {

      auto iterSector = currentSector->stListOfSectorsInActivityInteraction.hFirstElementSta;
      LST_M_StaticForEach(&currentSector->stListOfSectorsInActivityInteraction, iterSector) {
        if (iterSector->hPointerOfSectorSO->hLinkedObject.p_stSector == sector) {
          appearActive = true;
        }
      }
    }

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

  if (!appearActive) {
    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
  }
  if (highlight) {
    ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(80, 80, 0, 255));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(100, 100, 0, 255));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, IM_COL32(100, 100, 0, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 3)); // smaller padding
    flags |= ImGuiTreeNodeFlags_Framed;
  }

  nodeOpen = ImGui::TreeNodeEx(label.str().c_str(), flags);

  if (!appearActive) {
    ImGui::PopStyleColor();
  }

  if (highlight) {
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();
  }

  bool focused = ImGui::IsItemFocused();

  if (ImGui::IsItemClicked()) {
    g_DR_selectedObject = spo;
  }

  //CheckContextMenu(actor);

  if (nodeOpen) {

    HIE_tdstSuperObject* child;
    LST_M_DynamicForEach(spo, child) {
      DR_DLG_Hierarchy_SPO(child, nullptr, search);
    }

    ImGui::TreePop();
  }
}

static char searchBuf[128] = "";

void DR_DLG_Hierarchy_Draw() {

  if (!g_DR_settings.dlg_hierarchy) return;

  if (ImGui::Begin("Hierarchy", &g_DR_settings.dlg_hierarchy, ImGuiWindowFlags_NoCollapse)) {

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputTextWithHint("##hierarchy_search", "Search...", searchBuf, sizeof(searchBuf));

    std::string search = searchBuf;
    std::transform(search.begin(), search.end(), search.begin(), ::tolower);

    DR_DLG_Hierarchy_SPO(*GAM_g_p_stDynamicWorld, "Dynamic World", search);
    DR_DLG_Hierarchy_SPO(*GAM_g_p_stInactiveDynamicWorld, "Inactive Dynamic World", search);
    DR_DLG_Hierarchy_SPO(*GAM_g_p_stFatherSector, "Father Sector", search);
  }
  ImGui::End();
}