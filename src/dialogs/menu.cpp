#include "menu.h"
#include <imgui.h>

void DR_DLG_Menu_Draw() {

  ImGuiWindowClass windowClass;
  windowClass.ViewportFlagsOverrideSet = ImGuiViewportFlags_TopMost | ImGuiViewportFlags_NoTaskBarIcon;

  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImVec2(200, 20));

  ImGui::SetNextWindowClass(&windowClass);
  ImGui::SetNextWindowBgAlpha(0);
  ImGui::Begin("Main Menu", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground);
    ImGui::BeginMenuBar();

    ImGui::MenuItem("Menu Item");

    ImGui::EndMenuBar();
  ImGui::End();
}