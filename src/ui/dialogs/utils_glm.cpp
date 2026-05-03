#include "utils_glm.hpp"

#include "utils.hpp"
#include "ui/ui.hpp"
#include "ui/settings.hpp"
#include "ui/audiosystem.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <rendering/shader.hpp>
#include <rendering/textures.hpp>
#include <rendering/scene.hpp>
#include <rendering/mesh.hpp>
#include <glm/vec3.hpp>

// C INCLUDE
#include "mod/globals.h"
#include "mod/dsgvarnames.h"
#include "mod/util.h"
#include "miniaudio.h"
#include "resource.h"
#include <time.h>
#include <ACP_Ray2.h>

namespace {
  Mesh glmCube;
  Mesh glmDirectionCube;
  glm::vec3 savedGlmPosition;
  bool focusCameraOnGLM = false;
  bool playSoundOnGLMChange = false;
  glm::vec3 glmCameraOffset = glm::vec3(1, 1, 1);

  ma_sound* glmSound = nullptr;

  // GLM bookmarks
  bool autoBookmarkGlmPositions = false;
  std::vector<glm::vec3> glmBookmarks;
  std::time_t lastBookmarkTimestamp;

  glm::vec3 lastGlmPos;
}

glm::vec3* GetGlmPosition() {
  if (g_DR_rayman != nullptr) {
    void* ptr = ACT_DsgVarPtr(g_DR_rayman->hLinkedObject.p_stActor, DV_RAY_INTERN_TmpVector2);
    if (ptr != nullptr) {
      return (glm::vec3*)ptr;
    }
  }
  return nullptr;
}

void DR_DLG_Utils_Init_GLM() {
  glmCube = Mesh::createCube(glm::vec3(0.1f, 0.1f, 0.885594f));
  glmDirectionCube = Mesh::createCube(glm::vec3(1.0f, 1.0f, 1.0f));

  glmSound = AudioSystem::LoadSoundFromResource(IDR_GLMSOUND, false);
}

void DR_DLG_Utils_DrawTab_GLM()
{
  ImGui::Checkbox("Enable GLM radar (hold F1)", &g_DR_settings.util_enableGLMRadar);
  if (g_DR_settings.util_enableGLMRadar) {
    ImGui::Checkbox("Run radar for each bookmark", &g_DR_settings.util_glmRadarEachBookmark);
  }
  ImGui::Checkbox("Show GLM position", &g_DR_settings.util_showGLM);

  glm::vec3* glmPos = GetGlmPosition();

  ImGui::BeginDisabled(!g_DR_settings.util_showGLM);
  {
    if (glmPos != nullptr) {
      ImGui::DragFloat3("GLM", &glmPos->x);
    } else {
      ImGui::Text("GLM position unavailable");
    }
    ImGui::Checkbox("Play sound on GLM change", &playSoundOnGLMChange);
    ImGui::Checkbox("Auto-focus camera on GLM", &focusCameraOnGLM);
    if (focusCameraOnGLM) {
      ImGui::DragFloat3("Offset", &glmCameraOffset.x);
    }
    if (ImGui::Button("Save GLM position")) {
      if (GetGlmPosition() != nullptr) {
        glmBookmarks.push_back(*(GetGlmPosition()));
      }
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear")) {
      glmBookmarks.clear();
    }
    ImGui::SameLine();
    ImGui::Checkbox("Automatically save", &autoBookmarkGlmPositions);

    if (ImGui::BeginListBox("GLM Bookmarks")) {

      for (int i = 0;i < glmBookmarks.size();i++) {

        char label[64];
        snprintf(label, sizeof(label), "Bookmark %d (%.2f, %.2f, %.2f)", i, glmBookmarks[i].x, glmBookmarks[i].y, glmBookmarks[i].z);

        if (ImGui::Selectable(label)) {
          glm::vec3* glmPos = GetGlmPosition();
          if (glmPos != nullptr) *glmPos = glmBookmarks[i];
          lastGlmPos = *glmPos;
        }
      }

      ImGui::EndListBox();
    }

  }
  ImGui::EndDisabled();

  if (g_DR_settings.util_glmRadarEachBookmark) {
    g_DR_glmBookmarkCount = glmBookmarks.size();
    for (int i = 0;i < g_DR_glmBookmarkCount;i++) {
      g_DR_glmBookmarks[i] = FromGLMVec(glmBookmarks[i]);
    }
  }
  else {
    g_DR_glmBookmarkCount = 1;
    g_DR_glmBookmarks[0] = FromGLMVec(*glmPos);
  }
}

void DrawGLM(Scene* scene, Shader* shader) {

  if (!g_DR_settings.util_showGLM) return;

  glm::vec3* glmPtr = GetGlmPosition();
  if (glmPtr == nullptr) return;
  glm::vec3 glmPos = *glmPtr;


  if (focusCameraOnGLM) {

    if (glmPos != glm::vec3(0.0f)) {
      scene->setCameraPosition(glmPos + glmCameraOffset, glmPos);
    }
  }

  glm::mat4 mat = glm::translate(glm::mat4(1.0f), glmPos);

  shader->use();
  shader->setMat4("uModel", mat);
  shader->setTex2D("tex1", Textures::ColUnknown, 0);
  shader->setVec3("uvScale", glm::vec3(1.0f));
  shader->setBool("useSecondTexture", false);

  if (glmPos != glm::vec3(0.0f)) {
    glm::mat4 mat2 = glm::translate(glm::mat4(1.0f), glmPos);
    shader->setMat4("uModel", mat2);
    glmCube.draw();
  }

  for (int bookmarkIndex = 0; bookmarkIndex < glmBookmarks.size(); bookmarkIndex++) {

    if (bookmarkIndex >= GLMRadar_MaxBookmarks) break; // In C we have a fixed-size array to avoid headaches

    glm::vec3 bookmarkPos = glmBookmarks[bookmarkIndex];
    GlmRadarData data = g_DR_glmData[bookmarkIndex];

    glm::vec3 glmTeleport = ToGLMVec(data.g_DR_glmTeleport);

    for (int i = 0; i < GLMRadar_NumChecks; i++) {

      glm::vec3 potentialPos = ToGLMVec(data.g_DR_glmCoordinateList[i]);

      if (potentialPos != glm::vec3(0.0f)) {
        glm::mat4 mat3 = glm::translate(glm::mat4(1.0f), potentialPos);
        shader->setMat4("uModel", mat3);
        shader->setTex2D("tex1", Textures::ColBounce, 0);
        glmCube.draw();
      }
      else {
        break;
      }
    }

    if (glm::length(glmTeleport) > 0.0f) {

      DrawLine(shader, ToGLMVec(data.g_DR_glmDirectionFrom), ToGLMVec(data.g_DR_glmDirectionTo), Textures::ColZdd);
      DrawLine(shader, ToGLMVec(g_DR_rayman->p_stGlobalMatrix->stPos), glmTeleport, Textures::ColElectric);
      DrawLine(shader, glmTeleport, ToGLMVec(data.g_DR_glmDirectionTo), Textures::ColNocol);
    }
    else {

      DrawLine(shader, ToGLMVec(data.g_DR_glmDirectionFrom), ToGLMVec(data.g_DR_glmDirectionTo), Textures::ColBounce);
    }
  }
}

void HandleGLMUpdates() {

  glm::vec3* glmPtr = GetGlmPosition();
  if (glmPtr == nullptr) return;
  glm::vec3 glmPos = *glmPtr;

  if (g_DR_rayman == NULL) return;

  char* raymanState = (char*)ACT_DsgVarPtr(g_DR_rayman->hLinkedObject.p_stActor, DV_RAY_RAY_Etat);
  if (*raymanState == 4 || *raymanState == 25) { // Ignore when sliding (4) or swimming (25)
    lastGlmPos = glmPos;
    return;
  }

  if (lastGlmPos != glmPos && glmPos != glm::vec3(0.0f)) {

    lastGlmPos = glmPos;

    if (autoBookmarkGlmPositions) {

      time_t newTime = time(NULL);
      if (abs(difftime(lastBookmarkTimestamp, newTime)) < 0.1 && glmBookmarks.size() > 0) {
        glmBookmarks.pop_back();
      }

      glmBookmarks.push_back(glmPos);

      lastBookmarkTimestamp = newTime; 
    }

    if (playSoundOnGLMChange && glmSound != nullptr) {
      ma_sound_start(glmSound);
    }
  }
}

static void DrawLine(Shader* shader, glm::vec3 A, glm::vec3 B, unsigned int texture)
{
  if (glm::distance(A, B) > 0.001f) {
    glm::vec3 dir = glm::normalize(B - A);
    float lineLength = glm::length(B - A);

    float thickness = 0.05f;

    glm::mat4 rotation = glm::inverse(glm::lookAt(glm::vec3(0.0f), dir, glm::vec3(0, 1, 0)));

    glm::mat4 lineMat = glm::mat4(1.0f);
    lineMat = glm::translate(lineMat, A);
    lineMat = lineMat * rotation;

    lineMat = glm::translate(lineMat, glm::vec3(0.0f, 0.0f, -lineLength * 0.5f));
    lineMat = glm::scale(lineMat, glm::vec3(thickness, thickness, lineLength));

    shader->setMat4("uModel", lineMat);
    shader->setTex2D("tex1", texture, 0);
    glmDirectionCube.draw();
  }
}