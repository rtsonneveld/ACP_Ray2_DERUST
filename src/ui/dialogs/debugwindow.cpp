#include "debugwindow.hpp"
#include <ui/settings.hpp>
#include <imgui.h>

void DR_DLG_DebugWindow_Draw() {
  if (!g_DR_settings.dlg_debugwindow) return;

  if (ImGui::Begin("Debugger", &g_DR_settings.dlg_debugwindow, ImGuiWindowFlags_NoCollapse)) {

    ImGui::Checkbox("Enable Debugger", &g_DR_debuggerEnabled);
    if (!g_DR_debuggerEnabled) {
      ImGui::TextWrapped("Debugger is disabled.");
      ImGui::End();
      return;
    }

    if (g_DR_debuggerPaused) {
      ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Paused by debugger");
    }
    else {
      ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Running");
    }
    ImGui::Separator();
    ImGui::Text("Breakpoints:");
    for (size_t i = 0; i < g_DR_breakpoint_count; ++i) {
      ImGui::BulletText("0x%p", g_DR_breakpoints[i]);
      ImGui::SameLine();
      std::string removeLabel = "remove##bp" + std::to_string(i);
      if (ImGui::Button(removeLabel.c_str())) {
        DR_Debugger_UnsetBreakpoint(g_DR_breakpoints[i]);
        --i; // Adjust index since we removed an element
      }
    }
    ImGui::End();
  }
}