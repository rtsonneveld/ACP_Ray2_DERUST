#include "aimodel.hpp"

#include "ui/ui.hpp"
#include "ui/ui_util.hpp"
#include "ui/nameLookup.hpp"
#include "ui/settings.hpp"
#include <ui/custominputs.hpp>
#include "ui/ui_bridge.h"
#include <sstream>
#include <string>

// C INCLUDE
#include <ACP_Ray2.h>
#include <mod/globals.h>
#include <imgui/imnodes.h>
#include <mod/ai_strings.h>
#include "mod/debugger.h"

const int comportListBoxHeight = 8;
int selectedComportIndex = -1;
bool selectedComportIsReflex = false;

void DR_DLG_AiModel_SetSelectedComport(int comportIndex, bool isReflex) {
  selectedComportIndex = comportIndex;
  selectedComportIsReflex = isReflex;
}

bool DR_Settings_IsCatchExceptionsEnabled()
{
  return g_DR_settings.opt_tryCatchExceptions;
}

// Draw a combo box from a string array and store the selected index directly in a variable
static void DrawStringCombo(const char* label, const char** items, unsigned long* pValue)
{
  if (!items || !items[0] || !pValue)
    return;

  // Make sure current value is within bounds
  unsigned long currentIndex = *pValue;

  // Clip to valid range
  int maxIndex = 0;
  while (items[maxIndex] != nullptr) ++maxIndex;
  if (currentIndex >= (unsigned long)maxIndex)
    currentIndex = 0;

  const char* currentLabel = items[currentIndex];

  ImGui::SetNextItemWidth(150.0f);
  if (ImGui::BeginCombo(label, currentLabel)) {
    for (int i = 0; items[i] != nullptr; ++i) {
      bool is_selected = (currentIndex == (unsigned long)i);
      if (ImGui::Selectable(items[i], is_selected)) {
        currentIndex = i;
        *pValue = (unsigned long)i;  // directly update the value
      }
      if (is_selected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }
}


void DrawNodeOptions(HIE_tdstSuperObject* spo, AI_tdstNodeInterpret* node) {

  std::string label = "##param_" + std::to_string((int)node);

  switch (node->eType) {

    case AI_E_ti_KeyWord:
      ImGui::SameLine();
      DrawStringCombo(label.c_str(), AI_KeyWordIdStrings, (unsigned long *)&node->uParam.ulValue);
      break;
    case AI_E_ti_MetaAction:
      ImGui::SameLine();
      DrawStringCombo(label.c_str(), AI_MetaActionStrings, (unsigned long*)&node->uParam.ulValue);
      break;
    case AI_E_ti_Operator:
      ImGui::SameLine();
      DrawStringCombo(label.c_str(), AI_OperatorStrings, (unsigned long*)&node->uParam.ulValue);
      break;
    case AI_E_ti_Procedure:
      ImGui::SameLine();
      DrawStringCombo(label.c_str(), AI_ProcedureStrings, (unsigned long*)&node->uParam.ulValue);
      break;
    case AI_E_ti_Function:
      ImGui::SameLine();
      DrawStringCombo(label.c_str(), AI_FunctionStrings, (unsigned long*)&node->uParam.ulValue);
      break;
    case AI_E_ti_Field:
      ImGui::SameLine();
      DrawStringCombo(label.c_str(), AI_FieldStrings, (unsigned long*)&node->uParam.ulValue);
      break;
    case AI_E_ti_Condition:
      ImGui::SameLine();
      DrawStringCombo(label.c_str(), AI_ConditionStrings, (unsigned long*)&node->uParam.ulValue);
      break;

    case AI_E_ti_PersoRef:
      ImGui::SameLine();
      InputPerso(label.c_str(), (HIE_tdstSuperObject**)&node->uParam.pvValue);
      break;
    case AI_E_ti_Constant:
      ImGui::SameLine();
      ImGui::InputInt(label.c_str(), (int*)&node->uParam.lValue);
      break;
    case AI_E_ti_Real:
      ImGui::SameLine();
      ImGui::InputFloat(label.c_str(), (float*)&node->uParam.xValue);
      break;
    case AI_E_ti_DsgVarRef:
      ImGui::SameLine();
      DrawDsgVarId(spo, node->uParam.ulValue);
      break;
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
    {
      ImGui::SameLine();
      ImGui::Text("uParam: 0x%p", node->uParam.pvValue);
      break;
    }

    default:
      break;
  }

  // Execute button for all nodes
  std::string execLabel = "Exec##" + std::to_string((uintptr_t)(node));

  ImGui::SameLine();
  if (ImGui::Button(execLabel.c_str())) {
    AI_tdstGetSetParam param;
    AI_fn_p_stEvalTree(spo, node, &param);
  }
}

void DrawBreakpointCheckbox(const void * node)
{
  if (!g_DR_debuggerEnabled) return; 

  bool has_bp = DR_Debugger_HasBreakpoint(node);
  bool bp_toggle = has_bp;

  // Unique ID for ImGui
  std::string checkbox_id = "##bp_" + std::to_string((int)node);

  if (ImGui::Checkbox(checkbox_id.c_str(), &bp_toggle)) {
    if (bp_toggle) DR_Debugger_SetBreakpoint(node);
    else DR_Debugger_UnsetBreakpoint(node);
  }

  ImGui::SameLine();
}

// Helper function to draw a node with optional children
bool DrawAINode(AI_tdstNodeInterpret* node, bool forceOpen)
{
  // Draw breakpoint checkbox
  DrawBreakpointCheckbox(node);

  const AI_tdeTypeInterpret eType = node->eType;
  const AI_tdstNodeInterpret* next = node + 1;

  const bool has_next = (next->eType != AI_E_ti_EndTree);
  const bool has_children = has_next && (next->ucDepth > node->ucDepth);

  ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;

  bool isDebugPointer = (g_DR_debuggerInstructionPtr == node && g_DR_debuggerPaused);
  if (isDebugPointer) {
    flags |= ImGuiTreeNodeFlags_Framed;
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.8f, 0.1f, 0.1f, 0.5f));
  }

  if (!has_children) {
    flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
  }

  if (forceOpen) {
    ImGui::SetNextItemOpen(true, ImGuiCond_Always);
  }
  bool result = ImGui::TreeNodeEx((void*)node, flags, "%s", AI_GetTypeInterpretString(eType));

  if (isDebugPointer) {
    ImGui::PopStyleColor();
  }

  if (!has_children) {
    return false;
  }

  return result;
}

void DR_DLG_AIModel_Draw_Rule(HIE_tdstSuperObject* spo, AI_tdstTreeInterpret* tree)
{
  AI_tdstNodeInterpret* node = tree->p_stNodeInterpret;
  int stackDepth = 1;

  while (node->eType != AI_E_ti_EndTree)
  {
    const unsigned char depth = node->ucDepth;

    if (depth > stackDepth) {
      ++node;
      continue;
    }

    while (stackDepth > depth) {
      ImGui::TreePop();
      --stackDepth;
    }

    // Determine if this node is on the path to the node currently active in the debugger
    bool isTarget = (node == g_DR_debuggerInstructionPtr);
    bool isAncestor = false;

    if (!isTarget) {
      // Look ahead for the target node
      AI_tdstNodeInterpret* lookahead = node + 1;
      while (lookahead->eType != AI_E_ti_EndTree && lookahead->ucDepth > depth) {
        if (lookahead == g_DR_debuggerInstructionPtr) {
          isAncestor = true;
          break;
        }
        ++lookahead;
      }
    }

    bool open = DrawAINode(node, isAncestor);
    DrawNodeOptions(spo, node);

    if (open)
      ++stackDepth;

    ++node;
  }

  while (stackDepth-- > 1)
    ImGui::TreePop();
}


void DR_DLG_AIModel_Draw_Comport(HIE_tdstSuperObject* spo, AI_tdstComport comport) {

  for (int i = 0;i < comport.ucNbRules;i++) {
    DR_DLG_AIModel_Draw_Rule(spo, &comport.a_stRules[i]);
  }

  if (comport.p_stSchedule != nullptr) {
    ImGui::Separator();
    ImGui::Text("Schedule:");
    DR_DLG_AIModel_Draw_Rule(spo, comport.p_stSchedule);
  }
}

void DR_DLG_AIModel_Draw_ComportList(HIE_tdstEngineObject* actor, AI_tdstMind* mind, AI_tdstAIModel* model, bool isReflex, const ImVec2& size) {

  AI_tdstScriptAI* scriptAI = isReflex ? model->a_stScriptAIReflex : model->a_stScriptAIIntel;

  if (scriptAI == nullptr) {
    ImGui::BeginDisabled();
    if (ImGui::BeginListBox(isReflex ? "Reflexes" : "Comports", size)) {
      ImGui::Selectable("No behaviors");
      ImGui::EndListBox();
    }
    ImGui::EndDisabled();
    return;
  }

  char* modelName = HIE_fn_szGetObjectModelName(actor->hStandardGame->p_stSuperObject);
  char label[32];
  if (ImGui::BeginListBox(isReflex ? "Reflexes" : "Comports", size)) {

    for (int i = 0;i < scriptAI->ulNbComport;i++) {
      snprintf(label, sizeof(label), "%s##selection", (isReflex ? NameFromIndex(NameType::AIModel_Reflex, modelName, i) : NameFromIndex(NameType::AIModel_Comport, modelName, i)).c_str());
      if (ImGui::Selectable(label, selectedComportIsReflex == isReflex && i == selectedComportIndex)) {
        selectedComportIndex = i;
        selectedComportIsReflex = isReflex;
      }
    }

    ImGui::EndListBox();
  }
}

void DR_DLG_AIModel_Draw_AIModel(HIE_tdstSuperObject* spo, HIE_tdstEngineObject* actor, AI_tdstMind* mind, AI_tdstAIModel* model) {// Create a 2-column table that fills the window

  if (ImGui::BeginTable("AIModelTable", 2,
    ImGuiTableFlags_Resizable |
    ImGuiTableFlags_NoSavedSettings |
    ImGuiTableFlags_SizingStretchProp))
  {
    // ----- LEFT COLUMN -----
    ImGui::TableSetupColumn("LeftPane", ImGuiTableColumnFlags_WidthFixed, 300.0f);
    ImGui::TableNextColumn();

    float availHeight = ImGui::GetContentRegionAvail().y;
    float listBoxHeight = (availHeight - ImGui::GetStyle().ItemSpacing.y) * 0.5f;

    ImGui::BeginChild("AIModelLeftPane", ImVec2(0, 0), ImGuiChildFlags_Borders);
    DR_DLG_AIModel_Draw_ComportList(actor, mind, model, false, ImVec2(0, listBoxHeight));
    DR_DLG_AIModel_Draw_ComportList(actor, mind, model, true, ImVec2(0, listBoxHeight));
    ImGui::EndChild();

    // ----- RIGHT COLUMN -----
    ImGui::TableNextColumn();

    ImGui::BeginChild("AIModelRightPane", ImVec2(0, 0), ImGuiChildFlags_Borders);

    AI_tdstScriptAI* scriptAI = selectedComportIsReflex
      ? model->a_stScriptAIReflex
      : model->a_stScriptAIIntel;

    if (scriptAI && selectedComportIndex < scriptAI->ulNbComport) {
      AI_tdstComport comport = scriptAI->a_stComport[selectedComportIndex];
      DR_DLG_AIModel_Draw_Comport(spo, comport);
    }
    else {
      ImGui::Text("Select a rule/reflex on the left");
    }

    ImGui::EndChild();

    ImGui::EndTable();
  }
}

void DR_DLG_AIModel_Draw() {

  if (!g_DR_settings.dlg_aimodel) return;

  if (ImGui::Begin("AI Model", &g_DR_settings.dlg_aimodel, ImGuiWindowFlags_NoCollapse)) {

    if (g_DR_selectedObject != nullptr && g_DR_selectedObject->ulType == HIE_C_Type_Actor) {
      HIE_tdstSuperObject* spo = g_DR_selectedObject; // Alias
      HIE_tdstEngineObject* actor = spo->hLinkedObject.p_stActor;

      if (actor != nullptr && actor->hBrain != nullptr &&
        actor->hBrain->p_stMind != nullptr && actor->hBrain->p_stMind->p_stAIModel != nullptr) {
        DR_DLG_AIModel_Draw_AIModel(spo, actor, actor->hBrain->p_stMind, actor->hBrain->p_stMind->p_stAIModel);
      }
      else {
        ImGui::Text("This actor has no brain, mind or AI Model.");
      }
    }
    else {
      ImGui::Text("Select an actor in the hierarchy to view its AI Model.");
    }
  }
  ImGui::End();
}