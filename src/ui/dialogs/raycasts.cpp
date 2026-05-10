#include "raycasts.hpp"
#include "ui/ui_bridge.h"
#include "ui/ui.hpp"
#include "ui/settings.hpp"
#include "ui/ui_util.hpp"
#include "rendering/scene.hpp"
#include "rendering/textures.hpp"
#include "rendering/renderutil.hpp"
#include "mod/raycastdisplay.h"
#include "mod/globals.h"

bool doViewportRaycasts = false;
HIE_tdstSuperObject* sector = NULL;

void DR_DLG_Raycasts_Init() {
}

void TryViewportRaycast() {

  *AI_g_stCollideParameterPoint = FromGLMVec(scene.mouseLook.position);
  *AI_g_stCollideParameterVector = FromGLMVec(scene.mouseLook.forward * 10000.0f);

  MOD_fn_vComputeCollideResult_RayCastDisplay(NULL);

  g_DR_selectedObject = *AI_g_hSuperObjectHit;
}

void DR_DLG_Raycasts_Draw() {

  if (!g_DR_settings.dlg_raycasts) return;

  if (ImGui::Begin("Raycasts", &g_DR_settings.dlg_raycasts, ImGuiWindowFlags_NoCollapse)) {
    ImGui::Checkbox("Show raycasts", &g_DR_settings.opt_drawRaycasts);
    ImGui::Checkbox("Show raycast results", &g_DR_settings.opt_drawRaycastResults);
    ImGui::DragInt("Raycast lifetime", (int*)& g_DR_settings.opt_raycastLifetime, 1, 0, 600);
    ImGui::Checkbox("Do raycasts from viewport", &doViewportRaycasts);

    if (doViewportRaycasts) {
      TryViewportRaycast();
    }

    MTH3D_tdstVector from = FromGLMVec(scene.mouseLook.position);
    sector = SCT_fn_hResearchInWhatSectorIAm(*GAM_g_p_stFatherSector, &from);

    std::string spoName = SPO_Name(sector);
    ImGui::Text("Sector name: %s", spoName.c_str());
  }
  ImGui::End();
}

void DR_DLG_Raycasts_DrawScene(Scene* scene, Shader* shader) {
  if (!g_DR_settings.opt_drawRaycasts) {
    return;
  }

  for(int i=0;i<DR_Raycast_Max;i++) {
    DR_Raycast raycast = DR_raycasts[i];
    if (raycast.age > 0) {

      BOOL isHit = raycast.hitSuperObject != nullptr;
      glm::vec4 color = isHit ? glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) : glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); 

      RenderUtil::DrawArrow(shader, ToGLMVec(raycast.paramPoint), ToGLMVec(raycast.paramPoint) + ToGLMVec(raycast.paramVector), Textures::White, 0.04f, color);
      if (isHit && g_DR_settings.opt_drawRaycastResults) {
        RenderUtil::DrawArrow(shader, ToGLMVec(raycast.resultPoint), ToGLMVec(raycast.resultPoint) + ToGLMVec(raycast.resultVector), Textures::White, 0.02f);
      }
    }
  }
}