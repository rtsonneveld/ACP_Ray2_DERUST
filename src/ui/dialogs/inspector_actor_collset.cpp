// C INCLUDE
#include "imgui.h"
#include <stdio.h>
#include <ACP_Ray2.h>

void DR_DLG_Inspector_Draw_MS_Collset(HIE_tdstEngineObject* actor)
{
  ZDX_tdstCollSet* collset = actor->hCollSet;

  if (collset == nullptr) {
    ImGui::Text("CollSet is null");
    return;
  }

  ImGui::InputScalar("ucCharacterPriority", ImGuiDataType_U8, &collset->ucCharacterPriority);
  ImGui::InputScalar("ucCollComputeFrequency", ImGuiDataType_U8, &collset->ucCollComputeFrequency);
  ImGui::InputScalar("ucCollisionFlag", ImGuiDataType_U8, &collset->ucCollisionFlag);

  if (ImGui::CollapsingHeader("stColliderInfo")) {

    for (int i = 0;i < ZDX_C_wCollsetNbCollisionVectors;i++) {
      char label[64];
      snprintf(label, sizeof(label), "a_stColliderVectors[%d]", i);
      ImGui::InputScalarN(label, ImGuiDataType_Float, &collset->stColliderInfo.a_stColliderVectors[i].x, 3);
    }

    for (int i = 0;i < ZDX_C_wCollsetNbCollisionReals;i++) {
      char label[64];
      snprintf(label, sizeof(label), "a_xColliderReals[%d]", i);
      ImGui::InputScalar(label, ImGuiDataType_Float, &collset->stColliderInfo.a_xColliderReals[i]);
    }

    ImGui::InputScalar("ucColliderType", ImGuiDataType_U8, &collset->stColliderInfo.ucColliderType);
    ImGui::InputScalar("ucColliderPriority", ImGuiDataType_U8, &collset->stColliderInfo.ucColliderPriority);
  }

}