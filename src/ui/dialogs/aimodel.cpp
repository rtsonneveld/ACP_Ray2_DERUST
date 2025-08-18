#include "aimodel.hpp"

#include "ui/ui.hpp"
#include "ui/ui_util.hpp"
#include "ui/comportNames.hpp"
#include "ui/comportnodegraph.hpp"
#include <sstream>

// C INCLUDE
#include <ACP_Ray2.h>
#include <mod/globals.h>
#include <imgui/imnodes.h>
#include <mod/ai_strings.h>
#include <string>

#if _DEBUG
bool DR_DLG_AIModel_Enabled = true;
#else
bool DR_DLG_AIModel_Enabled = false;
#endif

const int comportListBoxHeight = 8;
int selectedComportIndex = -1;
bool selectedComportIsReflex = false;

void DR_DLG_AIModel_Draw_Rule(const AI_tdstTreeInterpret& tree)
{
  const AI_tdstNodeInterpret* node = &tree.p_stNodeInterpret[0];
  int stackDepth = 1; // how many TreePush() we actually performed

  for (;; ++node)
  {
    if (node->eType == AI_E_ti_EndTree)
      break;

    const unsigned char depth = node->ucDepth;

    // If we're under a closed parent, skip nodes until we come back up.
    if (depth > stackDepth)
      continue;

    // Climb up to this node's parent depth.
    while (stackDepth > depth) {
      ImGui::TreePop();
      --stackDepth;
    }

    // Peek the next node to know if this is a leaf.
    const AI_tdstNodeInterpret* next = node + 1;
    const bool has_next = (next->eType != AI_E_ti_EndTree);
    const bool has_children = has_next && (next->ucDepth > depth);

    if (!has_children) {
      // Leaf: render without pushing.
      ImGui::TreeNodeEx((void*)node,
        ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen,
        AI_GetNodeString(node), node->eType, depth);
      // stackDepth unchanged
    }
    else {
      // Non-leaf: only push if actually open.
      const bool open = ImGui::TreeNode((void*)node, AI_GetNodeString(node));
      if (open) {
        ++stackDepth; // We pushed; children will be processed.
      }
    }
  }

  // Close any remaining open nodes.
  while (stackDepth-- > 1)
    ImGui::TreePop();
}



void DR_DLG_AIModel_Draw_Comport(AI_tdstComport comport) {

  for (int i = 0;i < comport.ucNbRules;i++) {
    //ComportNodeGraph nodeGraph;
    //nodeGraph.Render(&comport.a_stRules[i]);
    DR_DLG_AIModel_Draw_Rule(comport.a_stRules[i]);
  }
}

void DR_DLG_AIModel_Draw_ComportList(HIE_tdstEngineObject* actor, AI_tdstMind* mind, AI_tdstAIModel* model, bool isReflex, const ImVec2& size) {

  AI_tdstScriptAI* scriptAI = isReflex ? model->a_stScriptAIReflex : model->a_stScriptAIIntel;

  if (scriptAI == nullptr) {
    ImGui::BeginDisabled();
      ImGui::BeginListBox(isReflex ? "Reflexes" : "Comports", size);
        ImGui::Selectable("No behaviors");
      ImGui::EndListBox();
    ImGui::EndDisabled();
    return;
  }

  char* modelName = HIE_fn_szGetObjectModelName(actor->hStandardGame->p_stSuperObject);
  char label[32];
  if (ImGui::BeginListBox(isReflex ? "Reflexes" : "Comports", size)) {

    for (int i = 0;i < scriptAI->ulNbComport;i++) {
      snprintf(label, sizeof(label), "%s##selection", (isReflex ? GetReflexName(modelName, i) : GetComportName(modelName, i)).c_str());
      if (ImGui::Selectable(label, selectedComportIsReflex == isReflex && i == selectedComportIndex)) {
        selectedComportIndex = i;
        selectedComportIsReflex = isReflex;
      }
    }

    ImGui::EndListBox();
  }
}

void DR_DLG_AIModel_Draw_AIModel(HIE_tdstEngineObject* actor, AI_tdstMind* mind, AI_tdstAIModel* model) {


  // Get the available height of the window's content region
  float leftColumnWidth = 300.0f; // You can adjust this width as needed

  ImGui::BeginChild("AIModelLeftPane", ImVec2(leftColumnWidth, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);

  float availHeight = ImGui::GetContentRegionAvail().y;
  float listBoxHeight = (availHeight - ImGui::GetStyle().ItemSpacing.y) * 0.5f;

  DR_DLG_AIModel_Draw_ComportList(actor, mind, model, false, ImVec2(0, listBoxHeight));
  DR_DLG_AIModel_Draw_ComportList(actor, mind, model, true, ImVec2(0, listBoxHeight));

  ImGui::EndChild();
  
  ImGui::SameLine();

  AI_tdstScriptAI* scriptAI = selectedComportIsReflex ? model->a_stScriptAIReflex : model->a_stScriptAIIntel;

  ImGui::BeginChild("AIModelRightPane", ImGui::GetContentRegionAvail(), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX); 
  if (scriptAI != nullptr && selectedComportIndex < scriptAI->ulNbComport) {
    
    AI_tdstComport comport = scriptAI->a_stComport[selectedComportIndex];

    DR_DLG_AIModel_Draw_Comport(comport);

  } else {
    ImGui::Text("Select a rule/reflex on the left");
  }
  ImGui::EndChild();
}

void DR_DLG_AIModel_Draw() {

  if (!DR_DLG_AIModel_Enabled) return;

  if (ImGui::Begin("AI Model", &DR_DLG_AIModel_Enabled, ImGuiWindowFlags_NoCollapse)) {

    if (g_DR_selectedObject != nullptr && g_DR_selectedObject->ulType == HIE_C_Type_Actor) {
      HIE_tdstSuperObject* spo = g_DR_selectedObject; // Alias
      HIE_tdstEngineObject* actor = spo->hLinkedObject.p_stActor;

      if (actor != nullptr && actor->hBrain != nullptr &&
        actor->hBrain->p_stMind != nullptr && actor->hBrain->p_stMind->p_stAIModel != nullptr) {
        DR_DLG_AIModel_Draw_AIModel(actor, actor->hBrain->p_stMind, actor->hBrain->p_stMind->p_stAIModel);
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