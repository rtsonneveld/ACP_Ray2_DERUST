#include "utils.hpp"
#include "ui/ui.hpp"
#include "ui/settings.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <rendering/shader.hpp>
#include <rendering/textures.hpp>
#include "ui/nameLookup.hpp"

// C INCLUDE
#include "mod/globals.h"
#include "mod/dsgvarnames.h"
#include "mod/util.h"
#include <ACP_Ray2.h>
#include <rendering/scene.hpp>

Mesh glmCube;
glm::vec3 savedGlmPosition;
bool focusCameraOnGLM = false;
glm::vec3 glmCameraOffset = glm::vec3(1, 1, 1);

glm::vec3* GetGlmPosition() {
  if (g_DR_rayman != nullptr) {
    void * ptr = ACT_DsgVarPtr(g_DR_rayman->hLinkedObject.p_stActor, DV_RAY_INTERN_TmpVector2);
    if (ptr != nullptr) {
      return (glm::vec3*)ptr;
    }
  }
}

void DR_DLG_Utils_Init() {
  glmCube = Mesh::createCube(glm::vec3(0.1, 0.1, 0.885594));
}

void DR_DLG_Utils_Draw() {

  if (!g_DR_settings.dlg_utils) return;

  if (ImGui::Begin("Utilities", &g_DR_settings.dlg_utils, ImGuiWindowFlags_NoCollapse)) {

    ImGui::BeginTabBar("UtilsTabs");

    if (ImGui::BeginTabItem("GLM")) {

      ImGui::Checkbox("Show GLM position", &g_DR_settings.util_showGLM);

      glm::vec3* glmPos = GetGlmPosition();

      ImGui::BeginDisabled(!g_DR_settings.util_showGLM);
      {
        ImGui::DragFloat3("GLM", &glmPos->x);
        ImGui::Checkbox("Auto-focus camera on GLM", &focusCameraOnGLM);
        if (focusCameraOnGLM) {
          ImGui::DragFloat3("Offset", &glmCameraOffset.x);
        }
        if (ImGui::Button("Save GLM position")) {
          savedGlmPosition = *(GetGlmPosition());
        }
        ImGui::SameLine();
        if (ImGui::Button("Load GLM position")) {
          glm::vec3* glmPos = GetGlmPosition();
          *glmPos = savedGlmPosition;
        }
      }
      ImGui::EndDisabled();

      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Misc")) {

      if (ImGui::Button("Trigger IBG")) {
        HIE_tdstSuperObject** dsgVarPersoGenerated = (HIE_tdstSuperObject**)ACT_DsgVarPtr(g_DR_rayman->hLinkedObject.p_stActor, DV_RAY_PersoGenerated);
        *dsgVarPersoGenerated = g_DR_global; // DsgVar has to be filled in order for barrel flying to remain active, use a dummy actor that always exists
        ACT_ChangeComportRule(g_DR_rayman->hLinkedObject.p_stActor, IndexFromName(NameType::AIModel_Comport, "YLT_RaymanModel", "YLT_TonneauFuseeComport"));
        ACT_ChangeComportReflex(g_DR_rayman->hLinkedObject.p_stActor, IndexFromName(NameType::AIModel_Reflex, "YLT_RaymanModel", "BNT_TonneauFuseeReflexe"));
        PLA_fn_bSetNewState(g_DR_rayman, ACT_GetStateByIndex(g_DR_rayman->hLinkedObject.p_stActor, 102), true, false);
      }

      ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
  }
  ImGui::End();
}

void DR_DLG_Utils_DrawScene(Scene * scene, Shader * shader) {

  if (focusCameraOnGLM) {
    glm::vec3 glmPos = *(GetGlmPosition());
    if (glmPos != glm::vec3(0)) {
      scene->setCameraPosition(glmPos + glmCameraOffset, glmPos);
    }
  }

  shader->use();

  auto glmPos = *(GetGlmPosition());

  glm::mat4 mat = glm::translate(glm::mat4(1.0f), glmPos);

  shader->use();
  shader->setMat4("uModel", mat);
  shader->setTex2D("tex1", Textures::ColUnknown, 0);
  shader->setVec3("uvScale", glm::vec3(1));
  shader->setBool("useSecondTexture", false);

  if (glmPos != glm::vec3(0)) {

    glmCube.draw();
  }
}