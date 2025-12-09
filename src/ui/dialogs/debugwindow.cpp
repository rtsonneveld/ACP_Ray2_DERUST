#include <fstream>
#include <sstream>
#include <string>
#include "debugwindow.hpp"
#include <ui/settings.hpp>
#include <ui/ui_util.hpp>
#include "mod/ai_strings.h"
#include <imgui.h>

#include <mod/ai_dump.h>
#include <ui/nameLookup.hpp>

int manualBreakpointAddress = 0;

static std::string MakeFrameHeader(int frame) {
  std::ostringstream oss;
  oss << "========-------- Frame " << frame << " --------========";
  return oss.str();
}

static std::string MakeSpoHeader(std::string name) {
  std::ostringstream oss;
  oss << " -------- " << name << " --------"; 
  return oss.str();
}

void SaveAiDump() {
  int itemCount = DR_AIDUMP_GetLogCount();
  if (itemCount == 0) return;

  // determine final frame number (for filename)
  int lastFrame = DR_AIDUMP_GetLogItem(itemCount-1).Frame;

  // filename
  std::ostringstream filename;
  filename << "AI DUMP " << lastFrame << ".txt";

  std::ofstream file(filename.str());
  if (!file.is_open()) return;

  int currentFrame = -1;
  HIE_tdstSuperObject* currentSPO = nullptr;
  AI_tdstComport* currentComport = nullptr;
  AI_tdstComport* currentReflex = nullptr;

  for (int i = 0; i < itemCount; i++) {

    LogItem item = DR_AIDUMP_GetLogItem(i);

    // New frame → write frame header
    if (item.Frame != currentFrame) {
      currentFrame = item.Frame;
      file << MakeFrameHeader(currentFrame) << "\n";
    }

    // New SPO → write spo name header
    if (item.Spo != currentSPO) {
      currentSPO = item.Spo;
      std::string name = SPO_Name(item.Spo);
      file << MakeSpoHeader(name) << "\n";
    }

    if (item.Comport != currentComport && item.Spo->hLinkedObject.p_stActor->hBrain != NULL) {
      currentComport = item.Comport;

      if (currentComport != nullptr) {
        AI_tdstAIModel* aiModel = item.Spo->hLinkedObject.p_stActor->hBrain->p_stMind->p_stAIModel;
        for (int i = 0;i < aiModel->a_stScriptAIIntel->ulNbComport;i++) {
          if (currentComport == &aiModel->a_stScriptAIIntel->a_stComport[i]) {
            file << " Comport[" << i << "]: " << NameFromIndex(NameType::AIModel_Comport, HIE_fn_szGetObjectModelName(item.Spo), i) << "\n";
            break;
          }
        }
      }
    }

    if (item.Reflex != currentReflex && item.Spo->hLinkedObject.p_stActor->hBrain != NULL) {
      currentReflex = item.Reflex;

      if (currentReflex != nullptr) {
        AI_tdstAIModel* aiModel = item.Spo->hLinkedObject.p_stActor->hBrain->p_stMind->p_stAIModel;
        for (int i = 0;i < aiModel->a_stScriptAIReflex->ulNbComport;i++) {
          if (currentReflex == &aiModel->a_stScriptAIReflex->a_stComport[i]) {
            file << " Reflex[" << i << "]: " << NameFromIndex(NameType::AIModel_Reflex, HIE_fn_szGetObjectModelName(item.Spo), i) << "\n";
            break;
          }
        }
      }
    }

    // indentation
    int depth = item.Node ? item.Node->ucDepth : 0;
    for (int d = 0; d < depth; d++)
      file << "  "; // 2 spaces per depth

    // node string
    const char* str = AI_GetNodeString(item.Node);
    file << (str ? str : "<null>") << "\n";
  }
}

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

        if (!DR_AIDUMP_IsActive()) {
          if (ImGui::Button("Start Recording")) {
            DR_AIDUMP_Start();
          }
        } else {
          if (ImGui::Button("Stop Recording")) {
            DR_AIDUMP_Stop();
            SaveAiDump();
          }
        }

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

        ImGui::BeginChild("DebuggerRightColumn", ImVec2(0, 0), ImGuiChildFlags_Borders);
        ImGui::Text("Breakpoints");

        ImGui::SetNextItemWidth(80.0f);
        ImGui::InputScalar("Address", ImGuiDataType_U32, &manualBreakpointAddress, nullptr, nullptr,
          "%X", ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::SameLine();
        if (ImGui::Button("Add manual breakpoint")) {
          DR_Debugger_SetBreakpoint((const void*)manualBreakpointAddress);
          manualBreakpointAddress = 0;
        }

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
      }

      ImGui::EndTable();
    }

    ImGui::End();
  }
}