#include "debugwindow.hpp"
#include <ui/settings.hpp>
#include <imgui.h>

void DR_DLG_DebugWindow_Draw() {
  if (!g_DR_settings.dlg_debugwindow) return;

  if (ImGui::Begin("Debugger", &g_DR_settings.dlg_debugwindow, ImGuiWindowFlags_NoCollapse)) {

    if (ImGui::BeginTable("DebuggerTable", 2,
      ImGuiTableFlags_Resizable |
      ImGuiTableFlags_NoSavedSettings |
      ImGuiTableFlags_SizingStretchProp)) {

      ImGui::TableSetupColumn("LeftPane", ImGuiTableColumnFlags_WidthFixed, 300.0f);

      ImGui::TableNextColumn();
      {
        ImGui::BeginChild("DebuggerLeftColumn", ImVec2(0, 0), ImGuiChildFlags_Borders);

        ImGui::Checkbox("Enable Debugger", &g_DR_debuggerEnabled);
        if (!g_DR_debuggerEnabled) {
          ImGui::TextWrapped("Debugger is disabled.");
        }

        if (g_DR_debuggerPaused) {
          ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Paused by debugger");
        }
        else {
          ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Running");
        }

        // Debugger controls
        {
          ImGui::BeginDisabled(!g_DR_debuggerPaused);
          if (ImGui::Button("Step into")) {
            DR_Debugger_StepInto();
          }
          ImGui::SameLine();
          if (ImGui::Button("Step over")) {
            DR_Debugger_StepOver();
          }
          ImGui::SameLine();
          if (ImGui::Button("Continue")) {
            DR_Debugger_Continue();
          }
          ImGui::EndDisabled();
        }

        ImGui::EndChild();
      }

      ImGui::TableNextColumn();
      {
        ImGui::BeginDisabled(!g_DR_debuggerPaused);

        ImGui::BeginChild("DebuggerRightColumn", ImVec2(0, 0), ImGuiChildFlags_Borders);
        ImGui::Text("Breakpoints");
        for (size_t i = 0; i < g_DR_breakpoint_count; ++i) {
          ImGui::BulletText("0x%p", g_DR_breakpoints[i]);
          ImGui::SameLine();
          std::string removeLabel = "remove##bp" + std::to_string(i);
          if (ImGui::Button(removeLabel.c_str())) {
            DR_Debugger_UnsetBreakpoint(g_DR_breakpoints[i]);
            --i; // Adjust index since we removed an element
          }
        }
        ImGui::EndChild();
        
        ImGui::EndDisabled();
      }

      ImGui::EndTable();
    }

    ImGui::End();
  }
}