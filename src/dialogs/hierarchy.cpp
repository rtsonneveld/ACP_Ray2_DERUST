#include "hierarchy.h"
#include <imgui.h>

void DR_DLG_Hierarchy_Draw() {

  ImGuiWindowClass windowClass;
  windowClass.ViewportFlagsOverrideSet = ImGuiViewportFlags_TopMost | ImGuiViewportFlags_NoTaskBarIcon;
  ImGui::SetNextWindowClass(&windowClass);

  ImGui::SetNextWindowPos(ImVec2(200, 200), ImGuiCond_Once);
  ImGui::SetNextWindowSize(ImVec2(200, 500), ImGuiCond_Once);

  bool open = true;
  ImGui::Begin("Hierarchy!");

  ImGui::Text("Hello World");

  ImGui::End();
}