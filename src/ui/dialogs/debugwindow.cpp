#include <fstream>
#include <sstream>
#include <string>
#include "debugwindow.hpp"
#include <ui/settings.hpp>
#include <ui/ui_util.hpp>
#include <ui/custominputs.hpp>
#include "mod/ai_strings.h"
#include <imgui.h>
#include <mod/globals.h>
#include <mod/ai_dump.h>
#include <ui/nameLookup.hpp>

int manualBreakpointAddress = 0;
bool watchBreakpointValues = FALSE;

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

void DrawGlobalBreakpoints() {

  ImGui::Checkbox("Enable Global Breakpoint", &g_DR_debugger_globalBreakpointEnabled);

  if (!g_DR_debugger_globalBreakpointEnabled) {
    return;
  }

  ImGui::Checkbox("Filter by Model", &g_DR_debugger_globalBreakpointEnableModelTypeFilter);
  if (g_DR_debugger_globalBreakpointEnableModelTypeFilter) {
    InputModelType("Model type", &g_DR_debugger_globalBreakpointModelTypeFilter);
  }

  const char* nodeTypeLabel = "Node type";

  DrawStringCombo(nodeTypeLabel, AI_TypeInterpretStrings, AI_E_Nb_ti, (unsigned long*)&g_DR_debugger_globalBreakpointNodeType);

  const char* paramLabel = "Parameter";

  switch (g_DR_debugger_globalBreakpointNodeType) {

    case AI_E_ti_KeyWord:
      ImGui::SameLine();
      DrawStringCombo(paramLabel, AI_KeyWordIdStrings, AI_E_Nb_kw, &g_DR_debugger_globalBreakpointNodeParam);
      break;
    case AI_E_ti_MetaAction:
      ImGui::SameLine();
      DrawStringCombo(paramLabel, AI_MetaActionStrings, AI_E_Nb_ma, &g_DR_debugger_globalBreakpointNodeParam);
      break;
    case AI_E_ti_Operator:
      ImGui::SameLine();
      DrawStringCombo(paramLabel, AI_OperatorStrings, AI_E_Nb_op, &g_DR_debugger_globalBreakpointNodeParam);
      break;
    case AI_E_ti_Procedure:
      ImGui::SameLine();
      DrawStringCombo(paramLabel, AI_ProcedureStrings, AI_E_Nb_proc, &g_DR_debugger_globalBreakpointNodeParam);
      break;
    case AI_E_ti_Function:
      ImGui::SameLine();
      DrawStringCombo(paramLabel, AI_FunctionStrings, AI_E_Nb_func, &g_DR_debugger_globalBreakpointNodeParam);
      break;
    case AI_E_ti_Field:
      ImGui::SameLine();
      DrawStringCombo(paramLabel, AI_FieldStrings, AI_E_Nb_fd, &g_DR_debugger_globalBreakpointNodeParam);
      break;
    case AI_E_ti_Condition:
      ImGui::SameLine();
      DrawStringCombo(paramLabel, AI_ConditionStrings, AI_E_Nb_cond, &g_DR_debugger_globalBreakpointNodeParam);
      break;

    case AI_E_ti_PersoRef:
      ImGui::SameLine();
      InputPerso(paramLabel, (HIE_tdstEngineObject**) &g_DR_debugger_globalBreakpointNodeParam);
      break;
    case AI_E_ti_Constant:
      ImGui::SameLine();
      ImGui::InputInt(paramLabel, (int*) &g_DR_debugger_globalBreakpointNodeParam);
      break;
    case AI_E_ti_Real:
      ImGui::SameLine();
      ImGui::InputFloat(paramLabel, (float*) &g_DR_debugger_globalBreakpointNodeParam);
      break;
    case AI_E_ti_DsgVarRef:
      /*ImGui::SameLine();
      DrawDsgVarId(context, node->uParam.ulValue);
      break;*/
    case AI_E_ti_DsgVar:
    case AI_E_ti_Button:
    case AI_E_ti_ConstantVector:
    case AI_E_ti_Vector:
    case AI_E_ti_Mask:
    case AI_E_ti_Module:
    case AI_E_ti_DsgVarId:
    case AI_E_ti_String:
    case AI_E_ti_LipsSynchroRef:
    case AI_E_ti_FamilyRef:
    case AI_E_ti_ActionRef:
    case AI_E_ti_SuperObjectRef:
    case AI_E_ti_WayPointRef:
    case AI_E_ti_TextRef:
    case AI_E_ti_ComportRef:
    case AI_E_ti_ModuleRef:
    case AI_E_ti_SoundEventRef:
    case AI_E_ti_ObjectTableRef:
    case AI_E_ti_GameMaterialRef:
    case AI_E_ti_ParticleGenerator:
    case AI_E_ti_Color:
    case AI_E_ti_ModelRef:
    case AI_E_ti_Light:
    case AI_E_ti_Caps:
    case AI_E_ti_Graph:
    case AI_E_ti_MacroRef:
    case AI_E_ti_Unknown:
    default:
    {
      ImGui::SameLine();
      ImGui::Text("Not supported");
      break;
    }
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

        ImGui::Checkbox("Enable Breakpoints", &g_DR_debuggerEnableBreakpoints);
        ImGui::Checkbox("Watch Breakpoint Values", &watchBreakpointValues);

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
        ImGui::Text("Global breakpoints");

        DrawGlobalBreakpoints();

        ImGui::Text("Breakpoints");
        ImGui::Spacing();

        /* ---- Add manual breakpoint (nicer input) ---- */
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Add:");
        ImGui::SameLine();

        ImGui::SetNextItemWidth(120.0f);
        ImGui::InputScalar(
          "##bp_address",
          ImGuiDataType_U32,
          &manualBreakpointAddress,
          nullptr,
          nullptr,
          "%08X",
          ImGuiInputTextFlags_CharsHexadecimal
        );

        ImGui::SameLine();
        if (ImGui::Button("Add##bp")) {
          DR_Debugger_SetBreakpoint((const void*)manualBreakpointAddress, nullptr);
          manualBreakpointAddress = 0;
        }

        if (ImGui::IsItemHovered())
          ImGui::SetTooltip("Add breakpoint at hexadecimal address");


        /* ---- Breakpoints table ---- */
        ImGui::Spacing();

        if (ImGui::BeginTable(
          "BreakpointsTable",
          3,
          ImGuiTableFlags_Borders |
          ImGuiTableFlags_RowBg |
          ImGuiTableFlags_Resizable |
          ImGuiTableFlags_SizingStretchProp))
        {
          ImGui::TableSetupColumn("Address");
          if (watchBreakpointValues) {
            ImGui::TableSetupColumn("Value");
          }
          ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 60.0f);
          ImGui::TableHeadersRow();

          for (size_t i = 0; i < g_DR_breakpoint_count; ++i) {
            ImGui::TableNextRow();

            /* Address column */
            int column = 0;
            ImGui::TableSetColumnIndex(column++);
            auto bp = g_DR_breakpoints[i];

            std::string name = SPO_Name(bp.spo);
            ImGui::Text("0x%p", bp.address);
            ImGui::Text("%s", name.c_str());

            if (watchBreakpointValues) {
              ImGui::TableSetColumnIndex(column++);

              AI_tdstNodeInterpret* node = (AI_tdstNodeInterpret*)bp.address;
              AI_tdstGetSetParam param;
              std::string spoName = "null";

              switch (node->eType) {

                // Ignore nodes that affect game state
                case AI_E_ti_KeyWord: break;
                case AI_E_ti_Procedure: break;
                case AI_E_ti_MetaAction: break;
                case AI_E_ti_Operator: break;

                default:
                  AI_fn_p_stEvalTree(bp.spo, node, &param);

                  switch (param.ulType) {
                    case AI_E_vt_Boolean: ImGui::Text("%s", param.uParam.ucValue ? "true" : "false"); break;
                    case AI_E_vt_Float: ImGui::Text("%f", param.uParam.xValue); break;
                    case AI_E_vt_Perso: 
                      
                      if (param.uParam.pvValue != NULL && param.uParam.hActor->hStandardGame != NULL) {
                        spoName = SPO_Name(param.uParam.hActor->hStandardGame->p_stSuperObject);
                      }
                      ImGui::Text("%s", spoName.c_str());
                      break;
                    case AI_E_vt_SuperObject:
                      spoName = SPO_Name(param.uParam.hSuperObject);
                      ImGui::Text("%s", spoName.c_str());
                      break;
                    case AI_E_vt_Vector: 
                      ImGui::Text("X: %f", param.uParam.stVector.x);
                      ImGui::Text("Y: %f", param.uParam.stVector.y); 
                      ImGui::Text("Z: %f", param.uParam.stVector.z);
                      break;
                    default:
                      ImGui::Text("%u", param.uParam.ulValue);
                    }

              }
            }

            /* Delete button column */
            ImGui::TableSetColumnIndex(column++);

            std::string showLabel = "Show##bp" + std::to_string(i);
            if (ImGui::Button(showLabel.c_str())) {
              DR_Debugger_SelectObjectAndComport(bp.spo, (AI_tdstNodeInterpret*)bp.address);
            }

            ImGui::SameLine();

            std::string deleteLabel = "Delete##bp" + std::to_string(i);
            if (ImGui::Button(deleteLabel.c_str())) {
              DR_Debugger_UnsetBreakpoint(bp.address, bp.spo);
              --i; // adjust index after removal
            }
          }

          ImGui::EndTable();
        }

        ImGui::EndChild();

      }

      ImGui::EndTable();
    }

    ImGui::End();
  }
}