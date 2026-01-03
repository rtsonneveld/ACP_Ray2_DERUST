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
#include <mod/ai_strings.h>
#include "mod/debugger.h"

const int comportListBoxHeight = 8;
int selectedComportIndex = -1;
SelectedComportType selectedComportType = SelectedComportType::Intelligence;

void DR_DLG_AiModel_SetSelectedComport_Intelligence(int comportIndex) {
  selectedComportIndex = comportIndex;
  selectedComportType = SelectedComportType::Intelligence;
}

void DR_DLG_AiModel_SetSelectedComport_Reflex(int comportIndex) {
  selectedComportIndex = comportIndex;
  selectedComportType = SelectedComportType::Reflex;
}

void DR_DLG_AiModel_SetSelectedComport_Macro(int comportIndex) {
  selectedComportIndex = comportIndex;
  selectedComportType = SelectedComportType::Macro;
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


void DrawNodeOptions(HIE_tdstSuperObject* spo, HIE_tdstSuperObject* context, AI_tdstNodeInterpret* node) {

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
      InputPerso(label.c_str(), (HIE_tdstEngineObject**)&node->uParam.pvValue);
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
      DrawDsgVarId(context, node->uParam.ulValue);
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

void DR_DLG_AIModel_Draw_Rule(HIE_tdstSuperObject* spo, AI_tdstNodeInterpret* node)
{
  int stackDepth = 1;

  HIE_tdstSuperObject* context = spo;
  int dotOperatorStackDepth = 1;

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

    if (node->eType == AI_E_ti_Operator && node->uParam.cValue == AI_E_op_Dot) {
      dotOperatorStackDepth = stackDepth;
      context = ((HIE_tdstEngineObject*)(node+1)->uParam.pvValue)->hStandardGame->p_stSuperObject;
    }
    else if (stackDepth < dotOperatorStackDepth) {
      context = spo;
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
    DrawNodeOptions(spo, context, node);

    if (open)
      ++stackDepth;

    ++node;
  }

  while (stackDepth-- > 1)
    ImGui::TreePop();
}


void DR_DLG_AIModel_Draw_Comport(HIE_tdstSuperObject* spo, AI_tdstComport comport) {

  for (int i = 0;i < comport.ucNbRules;i++) {
    DR_DLG_AIModel_Draw_Rule(spo, comport.a_stRules[i].p_stNodeInterpret);
  }

  if (comport.p_stSchedule != nullptr) {
    ImGui::Separator();
    ImGui::Text("Schedule:");
    DR_DLG_AIModel_Draw_Rule(spo, comport.p_stSchedule->p_stNodeInterpret);
  }
}

void DR_DLG_AIModel_Draw_ComportList(HIE_tdstEngineObject* actor, AI_tdstMind* mind, AI_tdstAIModel* model, SelectedComportType comportType, const ImVec2& size) {

  int comportNum = 0;
  const char* listBoxLabel = "";

  if (model == nullptr) {
    ImGui::BeginDisabled();
    if (ImGui::BeginListBox(listBoxLabel, size)) {
      ImGui::Selectable("No model");
      ImGui::EndListBox();
    }
    ImGui::EndDisabled();
    return;
  }
  
  switch (comportType) {
    case SelectedComportType::Intelligence:
      listBoxLabel = "Comports";
      comportNum = model->a_stScriptAIIntel != nullptr ? model->a_stScriptAIIntel->ulNbComport : 0; break;
    case SelectedComportType::Reflex:
      listBoxLabel = "Reflexes";
      comportNum = model->a_stScriptAIReflex != nullptr ? model->a_stScriptAIReflex->ulNbComport : 0; break;
    case SelectedComportType::Macro:
      listBoxLabel = "Macros";
      comportNum = model->p_stListOfMacro != nullptr ? model->p_stListOfMacro->ucNbMacro : 0; break;
  }


  if (comportNum == 0) {
    ImGui::BeginDisabled();
    if (ImGui::BeginListBox(listBoxLabel, size)) {
      ImGui::Selectable("No behaviors");
      ImGui::EndListBox();
    }
    ImGui::EndDisabled();
    return;
  }

  char* modelName = HIE_fn_szGetObjectModelName(actor->hStandardGame->p_stSuperObject);
  char label[32];
  if (ImGui::BeginListBox(listBoxLabel, size)) {

    for (int i = 0;i < comportNum; i++) {

      std::string itemLabel;

      switch (comportType) {
        case SelectedComportType::Intelligence: itemLabel = NameFromIndex(NameType::AIModel_Comport, modelName, i); break;
        case SelectedComportType::Reflex: itemLabel = NameFromIndex(NameType::AIModel_Reflex, modelName, i); break;
        case SelectedComportType::Macro: itemLabel = "Macro " + std::to_string(i); break;
      }

      snprintf(label, sizeof(label), "%s##selection", itemLabel.c_str());
      if (ImGui::Selectable(label, selectedComportType == comportType && i == selectedComportIndex)) {
        selectedComportIndex = i;
        selectedComportType = comportType;
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
    DR_DLG_AIModel_Draw_ComportList(actor, mind, model, SelectedComportType::Intelligence, ImVec2(0, listBoxHeight));
    DR_DLG_AIModel_Draw_ComportList(actor, mind, model, SelectedComportType::Reflex, ImVec2(0, listBoxHeight));
    DR_DLG_AIModel_Draw_ComportList(actor, mind, model, SelectedComportType::Macro, ImVec2(0, listBoxHeight));
    ImGui::EndChild();

    // ----- RIGHT COLUMN -----
    ImGui::TableNextColumn();

    ImGui::BeginChild("AIModelRightPane", ImVec2(0, 0), ImGuiChildFlags_Borders);

    if (selectedComportType != SelectedComportType::Macro) {

      AI_tdstScriptAI* scriptAI = selectedComportType == SelectedComportType::Intelligence ? model->a_stScriptAIIntel : model->a_stScriptAIReflex;

      if (scriptAI && selectedComportIndex < scriptAI->ulNbComport) {
        AI_tdstComport comport = scriptAI->a_stComport[selectedComportIndex];
        DR_DLG_AIModel_Draw_Comport(spo, comport);
      }
      else {
        ImGui::Text("Select an item on the left");
      }
    }
    else {
      AI_tdstListOfMacro* macroList = model->p_stListOfMacro;

      if (macroList && selectedComportIndex < macroList->ucNbMacro) {
        DR_DLG_AIModel_Draw_Rule(spo, macroList->p_stMacro[selectedComportIndex].p_stInitTree->p_stNodeInterpret);
      }
      else {
        ImGui::Text("Select an item on the left");
      }
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