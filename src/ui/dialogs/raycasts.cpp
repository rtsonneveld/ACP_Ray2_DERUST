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
bool simulateTeleportToViewport = false;
HIE_tdstSuperObject* sector = NULL;

void DR_DLG_Raycasts_Init() {

}

void TryViewportRaycast() {

  *AI_g_stCollideParameterPoint = FromGLMVec(scene.mouseLook.position);
  *AI_g_stCollideParameterVector = FromGLMVec(scene.mouseLook.forward * 10000.0f);

  MOD_fn_vComputeCollideResult_RayCastDisplay(NULL);

  g_DR_selectedObject = *AI_g_hSuperObjectHit;
}

void SimulateTeleportFromViewport() {

  MTH3D_tdstVector targetPosition = FromGLMVec(scene.mouseLook.position);

  POS_tdstCompletePosition targetCompletePosition = *g_DR_rayman->p_stGlobalMatrix;
  targetCompletePosition.stPos = targetPosition;

  // Clear the collision case table
  *COL_g_lNbElementsInTable = 0;

  COL_fn_vNewStaticCollisionForCharacter(g_DR_rayman,
    g_DR_rayman->hLinkedObject.p_stActor->hSectInfo->hCurrentSector,
    g_DR_rayman->p_stGlobalMatrix,
    &targetCompletePosition);


  ImGui::Text("Num collisions: %i", *COL_g_lNbElementsInTable);
}

void DR_DLG_Raycasts_Draw() {

  if (!g_DR_settings.dlg_raycasts) return;

  if (ImGui::Begin("Raycasts", &g_DR_settings.dlg_raycasts, ImGuiWindowFlags_NoCollapse)) {
    ImGui::Checkbox("Show raycasts", &g_DR_settings.opt_drawRaycasts);
    ImGui::Checkbox("Show raycast results", &g_DR_settings.opt_drawRaycastResults);
    ImGui::DragInt("Raycast lifetime", (int*)& g_DR_settings.opt_raycastLifetime, 1, 0, 600);
    ImGui::Checkbox("Do raycasts from viewport", &doViewportRaycasts);
    ImGui::Checkbox("Simulate teleport to viewport", &simulateTeleportToViewport);

    if (doViewportRaycasts) {
      TryViewportRaycast();
    }
    if (simulateTeleportToViewport) {
      SimulateTeleportFromViewport();
    }

    MTH3D_tdstVector from = FromGLMVec(scene.mouseLook.position);
    sector = SCT_fn_hResearchInWhatSectorIAm(*GAM_g_p_stFatherSector, &from);

    std::string spoName = SPO_Name(sector);
    ImGui::Text("Sector name: %s", spoName.c_str());
  }
  ImGui::End();
}

void DrawRaycastTypeCollideResult(DR_Raycast raycast, Shader* shader) {

  auto data = raycast.collideResult;
  BOOL isHit = data.hitSuperObject != nullptr;
  glm::vec4 color = isHit ? glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) : glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

  RenderUtil::DrawArrow(shader, ToGLMVec(data.paramPoint), ToGLMVec(data.paramPoint) + ToGLMVec(data.paramVector), Textures::White, 0.04f, color);
  if (isHit && g_DR_settings.opt_drawRaycastResults) {
    RenderUtil::DrawArrow(shader, ToGLMVec(data.resultPoint), ToGLMVec(data.resultPoint) + ToGLMVec(data.resultVector), Textures::White, 0.02f);
  }
}


void DrawRaycastTypeIntersectSegmentTriangle(DR_Raycast raycast, Shader* shader) {

  auto data = raycast.intersectSegmentTriangle;
  BOOL isHit = data.isHit;

  glm::vec3 from = ToGLMVec(data.paramPoint);
  glm::vec3 to = from + ToGLMVec(data.paramVector);

  if (glm::distance(from, scene.mouseLook.position) < 3.0f || glm::distance(to, scene.mouseLook.position) < 3.0f) {
    return;
  }

  // 1. Convert vertices to glm::vec3
  glm::vec3 v1 = ToGLMVec(data.vertex1);
  glm::vec3 v2 = ToGLMVec(data.vertex2);
  glm::vec3 v3 = ToGLMVec(data.vertex3);

  // 2. Combine the vertex data to create a unique seed value
  // We use dot products with arbitrary large prime-like numbers to mix the bits
  float r_seed = glm::dot(v1, glm::vec3(12.9898f, 78.233f, 45.164f)) + glm::dot(v2, glm::vec3(59.341f, 23.142f, 73.812f));
  float g_seed = glm::dot(v2, glm::vec3(21.412f, 63.523f, 19.421f)) + glm::dot(v3, glm::vec3(82.134f, 11.712f, 66.231f));
  float b_seed = glm::dot(v3, glm::vec3(35.123f, 91.421f, 57.231f)) + glm::dot(v1, glm::vec3(44.142f, 88.312f, 22.814f));

  // 3. Take the fractional part and scale to get a pseudo-random float between 0.0 and 1.0
  float r = glm::fract(sin(r_seed) * 43758.5453f);
  float g = glm::fract(sin(g_seed) * 43758.5453f);
  float b = glm::fract(sin(b_seed) * 43758.5453f);

  // 4. Define colors (Alpha is set based on hit status)
  glm::vec4 color = isHit ? glm::vec4(r, g, b, 1.0f) : glm::vec4(r, g, b, 0.25f);

  float thickness = isHit ? 0.1f : 0.05f;
  //RenderUtil::DrawArrow(shader, from, to, Textures::White, thickness, color);

  // Draw the triangle with the new random color
  RenderUtil::DrawLine(shader, v1, v2, Textures::White, thickness, color);
  RenderUtil::DrawLine(shader, v2, v3, Textures::White, thickness, color);
  RenderUtil::DrawLine(shader, v3, v1, Textures::White, thickness, color);


  RenderUtil::DrawTriangle(shader, v1, v2, v3, Textures::White, color);

  // EXTRUDE

  if (isHit) {
    RenderUtil::DrawTriangle(shader, v1, v2, v3, Textures::White, color);
  }

  float arrowLength = 2.0f;

  glm::vec3 edge1 = v2 - v1;
  glm::vec3 edge2 = v3 - v1;
  glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

  // 2. Calculate the extruded vertices
  glm::vec3 extrusionVector = normal * arrowLength;

  RenderUtil::DrawArrow(shader, v1, v1 + extrusionVector, Textures::White, thickness, color);
  RenderUtil::DrawArrow(shader, v2, v2 + extrusionVector, Textures::White, thickness, color);
  RenderUtil::DrawArrow(shader, v3, v3 + extrusionVector, Textures::White, thickness, color);
}

void DR_DLG_Raycasts_DrawScene(Scene* scene, Shader* shader) {
  if (!g_DR_settings.opt_drawRaycasts) {
    return;
  }

  for(int i=0;i<DR_Raycast_Max;i++) {
    DR_Raycast raycast = DR_raycasts[i];
    if (raycast.age > 0) {

      switch (raycast.eType) {
        case DR_RaycastType_CollideResult: DrawRaycastTypeCollideResult(raycast, shader); break;
        case DR_RaycastType_IntersectSegmentTriangle: DrawRaycastTypeIntersectSegmentTriangle(raycast, shader); break;
      }
    }
  }
}