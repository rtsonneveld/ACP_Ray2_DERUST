#include "utils_misc.hpp"

#include "utils.hpp"
#include "ui/ui.hpp"
#include "ui/settings.hpp"
#include "ui/nameLookup.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <rendering/shader.hpp>
#include <rendering/textures.hpp>
#include <rendering/scene.hpp>

#include "mod/globals.h"
#include "mod/util.h"
#include "mod/dsgvarnames.h"
#include <ACP_Ray2.h>

namespace {
  int colliderType = 52;
  Mesh directionPlane;
  Mesh directionArrow;
}

void DR_DLG_Utils_Init_Misc() {
  directionPlane = Mesh::createQuad(1000.0f, 1000.0f);
  directionArrow = Mesh::createCube(glm::vec3(1.0f, 1.0f, 1.0f));
}

void DR_DLG_Utils_DrawTab_Misc()
{
  ImGui::Checkbox("Visualize Rayman's direction", &g_DR_settings.util_showDirection);
  ImGui::Checkbox("Visualize Rayman's slide speed", &g_DR_settings.util_showSlideSpeed);

  glm::vec3 slideSpeed = ToGLMVec(*(MTH3D_tdstVector*)ACT_DsgVarPtr(g_DR_rayman->hLinkedObject.p_stActor, DV_RAY_INTERN_RaySlideSpeed));
  ImGui::Text("|%.2f, %.2f, %.2f| = %.2f", slideSpeed.x, slideSpeed.y, slideSpeed.z, glm::length(slideSpeed));

  if (ImGui::Button("Trigger IBG")) {
    HIE_tdstSuperObject** dsgVarPersoGenerated = (HIE_tdstSuperObject**)ACT_DsgVarPtr(g_DR_rayman->hLinkedObject.p_stActor, DV_RAY_PersoGenerated);
    *dsgVarPersoGenerated = g_DR_global; // DsgVar has to be filled in order for barrel flying to remain active, use a dummy actor that always exists
    ACT_ChangeComportRule(g_DR_rayman->hLinkedObject.p_stActor, IndexFromName(NameType::AIModel_Comport, "YLT_RaymanModel", "YLT_TonneauFuseeComport"));
    ACT_ChangeComportReflex(g_DR_rayman->hLinkedObject.p_stActor, IndexFromName(NameType::AIModel_Reflex, "YLT_RaymanModel", "BNT_TonneauFuseeReflexe"));
    PLA_fn_bSetNewState(g_DR_rayman, ACT_GetStateByIndex(g_DR_rayman->hLinkedObject.p_stActor, 102), true, false);
  }

  int step = 1;
  ImGui::InputScalar("Damage type", ImGuiDataType_U8, &colliderType, &step, 0, 0, 0);
  if (ImGui::Button("Apply damage")) {
    HIE_tdstEngineObject* pRayman = HIE_M_hSuperObjectGetActor(HIE_M_hGetMainActor());
    pRayman->hCollSet->stColliderInfo.ucColliderType = colliderType;
    pRayman->hCollSet->stColliderInfo.ucColliderPriority = 255;
    pRayman->hStandardGame->ucHitPoints--;
  }
}

void DrawRaymanDirection(Shader* shader) {

  if (g_DR_rayman == nullptr) return;
  glm::mat4 matrix = ToGLMMat4(*g_DR_rayman->p_stGlobalMatrix);

  matrix = glm::rotate(matrix, glm::radians(90.0f), glm::vec3(0, 1, 0));

  shader->use();
  shader->setMat4("uModel", matrix);
  shader->setTex2D("tex1", Textures::ColBounce, 0);
  shader->setVec3("uvScale", glm::vec3(1,1,1));
  shader->setFloat("uAlphaMult", 0.5f);

  directionPlane.draw();

  shader->setVec3("uvScale", glm::vec3(1.0f));
  shader->setFloat("uAlphaMult", 1.0f);
}

static void DrawLine(Shader* shader, glm::vec3 A, glm::vec3 B, unsigned int texture)
{
  if (glm::distance(A, B) > 0.001f) {
    glm::vec3 dir = glm::normalize(B - A);
    float lineLength = glm::length(B - A);

    float thickness = 0.1f;

    glm::mat4 rotation = glm::inverse(glm::lookAt(glm::vec3(0.0f), dir, glm::vec3(0, 1, 0)));

    glm::mat4 lineMat = glm::mat4(1.0f);
    lineMat = glm::translate(lineMat, A);
    lineMat = lineMat * rotation;

    lineMat = glm::translate(lineMat, glm::vec3(0.0f, 0.0f, -lineLength * 0.5f));
    lineMat = glm::scale(lineMat, glm::vec3(thickness, thickness, lineLength));

    shader->setMat4("uModel", lineMat);
    shader->setTex2D("tex1", texture, 0);
    directionArrow.draw();
  }
}

void DrawRaymanSlideSpeed(Shader* shader) {
  if (g_DR_rayman == nullptr) return;
  const float lengthMultiplier = 1.0f;

  glm::vec3 offset(0.0f, 0.0f, 2.0f);

  MTH3D_tdstVector slideSpeed = *(MTH3D_tdstVector*)ACT_DsgVarPtr(g_DR_rayman->hLinkedObject.p_stActor, DV_RAY_INTERN_RaySlideSpeed);
  DrawLine(shader, ToGLMVec(g_DR_rayman->p_stGlobalMatrix->stPos) + offset, ToGLMVec(g_DR_rayman->p_stGlobalMatrix->stPos) + offset + (ToGLMVec(slideSpeed) * lengthMultiplier),
    Textures::ColUser3);
}