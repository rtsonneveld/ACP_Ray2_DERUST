#include <imgui.h>
#include "ui/custominputs.hpp"
#include <ACP_Ray2.h>
#include <stdio.h>
#include <string>

const char* GetObstacleTypeName(int type)
{
  switch (type)
  {
  case 0:     return "Nothing";
  case 0x1:   return "Scenery";
  case 0x2:   return "Mobile";
  case 0x4:   return "DoubleEdge";
  case 0x9:   return "MobileWall";
  default:    return "Unknown";
  }
}

/*


  MTH_tdxReal m_xRate;
  MTH3D_tdstVector m_stNorm;
  MTH3D_tdstVector m_stContact;
  GMT_tdstGameMaterial* hMyMaterial;
  GMT_tdstGameMaterial *hCollidedMaterial;
  HIE_tdstSuperObject* mp_stSupObj; /* Collided object */
/*
DNM_tdeObstacleType m_eType;
ACP_tdxIndex m_xMyEntity; /* type of element : segment, point, edge, ... */
/*ACP_tdxIndex m_xCollidedEntity; /* type of element : segment, point, edge, ... */
/*MTH3D_tdstVector m_stTranslation; /* translation to get the actor out of the obstacle */
/*MTH3D_tdstVector m_stZoneMove; /* zone movement */
/*MTH3D_tdstVector m_stZonePosition; /* zone position when collide */
/*MTH_tdxReal m_xZoneRadius; /* radius of the collide zone */
/*DNM_tdstMecMatCharacteristics m_stProperties; /* Material properties */
/*DNM_tdstMecMatCharacteristics m_stMyProperties; /* Material properties of mine */

void DrawMecMatCharacteristics(const char* label, DNM_tdstMecMatCharacteristics mat)
{
  if (ImGui::TreeNode(label)) {
    ImGui::DragFloat("Slide", &mat.xSlide);
    ImGui::DragFloat("Rebound", &mat.xRebound);
    ImGui::TreePop();
  }
}

void DrawMecObstacle(const char* label, DNM_tdstMecObstacle obs, bool collision)
{

  if (ImGui::TreeNode(label, "%s%s", label, collision ? " (hit)" : "")) {
    ImGui::Text("Rate: %.3f", obs.m_xRate);
    ImGui::DragFloat3("Normal", (float*)&(obs.m_stNorm.x));
    ImGui::DragFloat3("Contact", (float*)&(obs.m_stContact.x));

    if (ImGui::TreeNode("My Material")) {
      InputGameMaterial(obs.hMyMaterial);
      ImGui::TreePop();
    }

    if (ImGui::TreeNode("Collided Material")) {
      InputGameMaterial(obs.hCollidedMaterial);
      ImGui::TreePop();
    }

    ImGui::Text("SuperObject: %p", (void*)obs.mp_stSupObj);

    // Mechanics specific
    ImGui::Text("m_eType: %d", GetObstacleTypeName(obs.m_eType));
    ImGui::Text("m_xMyEntity: 0x%X", obs.m_xMyEntity);
    ImGui::Text("m_xCollidedEntity: 0x%X", obs.m_xCollidedEntity);
    ImGui::DragFloat3("m_stTranslation", &obs.m_stTranslation.x);
    ImGui::DragFloat3("m_stZoneMove", &obs.m_stZoneMove.x);
    ImGui::DragFloat3("m_stZonePosition", &obs.m_stZonePosition.x);
    ImGui::DragFloat("m_xZoneRadius", &obs.m_xZoneRadius);
    DrawMecMatCharacteristics("m_stProperties", obs.m_stProperties);
    DrawMecMatCharacteristics("m_stMyProperties", obs.m_stMyProperties);

    ImGui::TreePop();
  }
}


void DR_DLG_EngineGlobals_Draw_CollisionReport() {
  DNM_tdstMecReport* report = MEC_g_stReport;
  
  std::string stateText;

  unsigned long state = report->m_ulCurrSurfaceState;

  if (state & MEC_C_WOT_ulError)        stateText += "Error | ";
  if (state & MEC_C_WOT_ulGround)       stateText += "Ground | ";
  if (state & MEC_C_WOT_ulWall)         stateText += "Wall | ";
  if (state & MEC_C_WOT_ulCeiling)      stateText += "Ceiling | ";
  if (state & MEC_C_WOT_ulWater)        stateText += "Water | ";
  if (state & MEC_C_WOT_ulForceMobile)  stateText += "ForceMobile | ";
  if (state & MEC_C_WOT_ulMobile)       stateText += "Mobile | ";
  if ((state & MEC_C_WOT_ulFather) == MEC_C_WOT_ulFather) stateText += "Father | "; // Unused anyways

  if (stateText.empty())
    stateText = "NoObstacle";
  else
    stateText.erase(stateText.size() - 3); // remove last " | "

  ImGui::Text("%s : 0x%lX (%s)", "m_ulCurrSurfaceState", state, stateText.c_str());

  ImGui::Separator();

  DrawMecObstacle("Obstacle", report->m_stObstacle, report->m_ulCurrSurfaceState != 0);
  DrawMecObstacle("Ground", report->m_stGround, report->m_ulCurrSurfaceState & MEC_C_WOT_ulGround);
  DrawMecObstacle("Wall", report->m_stWall, report->m_ulCurrSurfaceState & MEC_C_WOT_ulWall);
  DrawMecObstacle("Character", report->m_stCharacter, report->m_ulCurrSurfaceState & MEC_C_WOT_ulMobile);
  DrawMecObstacle("Water", report->m_stWater, report->m_ulCurrSurfaceState & MEC_C_WOT_ulWater);
  DrawMecObstacle("Ceiling", report->m_stCeil, report->m_ulCurrSurfaceState & MEC_C_WOT_ulCeiling);

  ImGui::Separator();

}