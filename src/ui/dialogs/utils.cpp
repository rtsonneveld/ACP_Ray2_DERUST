#include "utils.hpp"
#include "ui/ui.hpp"
#include "ui/settings.hpp"
#include "ui/nameLookup.hpp"
#include "ui/audiosystem.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <rendering/shader.hpp>
#include <rendering/textures.hpp>
#include <rendering/scene.hpp>

// C INCLUDE
#include "mod/globals.h"
#include "mod/dsgvarnames.h"
#include "mod/util.h"
#include "mod/ai_distancechecks.h"
#include "miniaudio.h"
#include "resource.h"
#include <ACP_Ray2.h>

Mesh glmCube;
Mesh directionPlane;
Mesh distanceSphere;
glm::vec3 savedGlmPosition;
bool focusCameraOnGLM = false;
bool playSoundOnGLMChange = false;
glm::vec3 glmCameraOffset = glm::vec3(1, 1, 1);

ma_sound* glmSound = nullptr;

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
  directionPlane = Mesh::createQuad(1000.0f, 1000.0f);
  distanceSphere = Mesh::createSphere();

  glmSound = AudioSystem::LoadSoundFromResource(IDR_GLMSOUND, false);
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
        ImGui::Checkbox("Play sound on GLM change", &playSoundOnGLMChange);
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

      ImGui::Checkbox("Visualize Rayman's direction", &g_DR_settings.util_showDirection);

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

  shader->setVec3("uvScale", glm::vec3(1));
  shader->setFloat("uAlphaMult", 1.0f);
}

void DrawDistanceChecks(Shader* shader) {

  glm::mat4 matrix;

  for (int i = 0; i < DR_DistanceCheckCount; i++) {

    DistanceCheckEntry entry = distanceChecks[i];
    matrix = glm::translate(glm::mat4(1.0f), glm::vec3(entry.x, entry.y, entry.z));
    matrix = glm::scale(matrix, glm::vec3(entry.radius, entry.radius, entry.radius));

    shader->use();
    shader->setMat4("uModel", matrix);
    shader->setTex2D("tex1", entry.lifetime == 0 ? Textures::ColAi : Textures::ColAiInactive, 0);
    shader->setVec3("uvScale", glm::vec3(entry.radius));
    if (g_DR_settings.opt_distanceCheckVisibility == DistanceCheckVisibility::FadeOut) {
      float lifeFactor = (float)entry.lifetime / DR_DISTANCECHECKS_MAXLIFETIME;
      shader->setFloat("uAlphaMult", 0.5f - lifeFactor * 0.5f);
    } else {
      shader->setFloat("uAlphaMult", 0.5f);
    }

    distanceSphere.draw();

    shader->setVec3("uvScale", glm::vec3(1));
    shader->setFloat("uAlphaMult", 1.0f);
  }
}

void DrawGLM(Scene * scene, Shader * shader) {

  glm::vec3 glmPos = *(GetGlmPosition());

  if (focusCameraOnGLM) {

    if (glmPos != glm::vec3(0)) {
      scene->setCameraPosition(glmPos + glmCameraOffset, glmPos);
    }
  }

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

glm::vec3 lastGlmPos;
void HandleSoundForGLM() {

  glm::vec3 glmPos = *(GetGlmPosition());

  char* raymanState = (char*)ACT_DsgVarPtr(g_DR_rayman->hLinkedObject.p_stActor, DV_RAY_RAY_Etat);
  if (*raymanState == 4 || *raymanState == 25) { // Ignore when sliding (4) or swimming (25)
    lastGlmPos = glmPos;
    return;
  }

  if (lastGlmPos != glmPos && glmPos != glm::vec3(0)) {
    lastGlmPos = glmPos;

    if (glmSound != nullptr) {
      ma_sound_start(glmSound);
    }
  }
}

void DR_DLG_Utils_DrawScene(Scene * scene, Shader * shader) {

  shader->use();

  if (g_DR_settings.util_showGLM) {
    DrawGLM(scene, shader);
  }

  if (playSoundOnGLMChange) {
    HandleSoundForGLM();
  }

  if (g_DR_settings.opt_distanceCheckVisibility != DistanceCheckVisibility::Hidden) {
    DrawDistanceChecks(shader);
  }

  if (g_DR_settings.util_showDirection) {
    DrawRaymanDirection(shader);
  }
}