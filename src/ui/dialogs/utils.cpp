#include "utils.hpp"
#include "utils_glm.hpp"
#include "utils_distance.hpp"
#include "utils_misc.hpp"
#include "ui/ui.hpp"
#include "ui/settings.hpp"
#include <rendering/shader.hpp>
#include <rendering/scene.hpp>

void DR_DLG_Utils_Init() {
  DR_DLG_Utils_Init_GLM();
  DR_DLG_Utils_Init_Distance();
  DR_DLG_Utils_Init_Misc();
}

void DR_DLG_Utils_Draw() {

  if (!g_DR_settings.dlg_utils) return;

  if (ImGui::Begin("Utilities", &g_DR_settings.dlg_utils, ImGuiWindowFlags_NoCollapse)) {

    ImGui::BeginTabBar("UtilsTabs");

    if (ImGui::BeginTabItem("GLM")) {
      DR_DLG_Utils_DrawTab_GLM();
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Measure distance")) {
      DR_DLG_Utils_DrawTab_Distance();
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Misc")) {
      DR_DLG_Utils_DrawTab_Misc();
      ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
  }
  ImGui::End();
}

void DR_DLG_Utils_DrawScene(Scene* scene, Shader* shader) {

  shader->use();

  if (g_DR_settings.util_showGLM) {
    DrawGLM(scene, shader);
  }

  HandleGLMUpdates();

  if (g_DR_settings.opt_distanceCheckVisibility != DistanceCheckVisibility::Hidden) {
    DrawDistanceChecks(shader);
  }

  if (g_DR_settings.util_showDirection) {
    DrawRaymanDirection(shader);
  }
}