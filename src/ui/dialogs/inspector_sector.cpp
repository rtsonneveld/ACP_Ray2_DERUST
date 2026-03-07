#include "ui/ui.hpp"
#include "ui/ui_util.hpp"

#include "inspector_sector.hpp"

#include <ACP_Ray2.h>

void DR_DLG_Inspector_Draw_Sector(HIE_tdstSuperObject* spo) {

  SCT_tdstSector* sector = spo->hLinkedObject.p_stSector;
  if (spo == NULL || sector == NULL) {
    ImGui::Text("Invalid SuperObject!");
    return;
  }

  if (ImGui::CollapsingHeader("Sector")) {

    ImGui::DragFloat3("a_stMinMaxPoints[0]", &(sector->a_stMinMaxPoints[0].x));
    ImGui::DragFloat3("a_stMinMaxPoints[1]", &(sector->a_stMinMaxPoints[1].x));
    ImGui::InputScalar("xZFar", ImGuiDataType_Float, &sector->xZFar);

    ImGui::Checkbox("bVirtual", (bool*)&sector->bVirtual);
    ImGui::InputScalar("cCameraType", ImGuiDataType_S8, &sector->cCameraType);
    ImGui::InputScalar("cCounter", ImGuiDataType_S8, &sector->cCounter);
    ImGui::InputScalar("cPriority", ImGuiDataType_S8, &sector->cPriority);
    
    // TODO: VirtualSkyMaterial

    ImGui::InputScalar("ucFogIntensity", ImGuiDataType_U8, &sector->ucFogIntensity);
  }

  if (ImGui::CollapsingHeader("Characters")) {
    // TODO: Draw list of actors (sector->stListOfCharacters)
  }

  if (ImGui::CollapsingHeader("Static Lights")) {
    // TODO: Draw list of lights (sector->stListOfStaticLights)
  }

  if (ImGui::CollapsingHeader("Dynamic Lights")) {
    // TODO: Draw list of static lights (sector->stListOfDNMLights)
  }

}