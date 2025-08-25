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

std::string AI_BuildExpression(const AI_tdstNodeInterpret* node,
  const AI_tdstNodeInterpret* end,
  unsigned char depth)
{
  std::string expr;

  while (node < end && node->eType != AI_E_ti_EndTree && node->ucDepth >= depth)
  {
    if (node->ucDepth > depth) {
      ++node;
      continue; // deeper, already consumed by child builder
    }

    const char* text = AI_GetNodeString(node);

    // ---- Handle by type ----
    switch (node->eType)
    {
    case AI_E_ti_Condition: {
      AI_tdeCondition cond = static_cast<AI_tdeCondition>(node->uParam.ulValue);
      if (cond == AI_E_cond_Not) {
        // Unary condition
        std::string rhs = AI_BuildExpression(node + 1, end, depth + 1);
        expr += "(!" + rhs + ")";
      }
      else {
        // Binary condition
        std::string lhs = AI_BuildExpression(node + 1, end, depth + 1);

        // Find sibling at same depth for rhs
        const AI_tdstNodeInterpret* next = node + 1;
        while (next < end && next->eType != AI_E_ti_EndTree) {
          if (next->ucDepth == depth + 1) break;
          if (next->ucDepth <= depth) { next = nullptr; break; }
          ++next;
        }
        std::string rhs = next ? AI_BuildExpression(next, end, depth + 1) : "";

        expr += "(" + lhs + " " + std::string(text) + " " + rhs + ")";
      }
      ++node;
      break;
    }

    case AI_E_ti_Operator: {
      AI_tdeOperator op = static_cast<AI_tdeOperator>(node->uParam.ulValue);
      if (op == AI_E_op_UnaryMinus || op == AI_E_op_VectorUnaryMinus) {
        // Unary operator
        std::string rhs = AI_BuildExpression(node + 1, end, depth + 1);
        expr += "(-" + rhs + ")";
      } else if (op == AI_E_op_Affect) {
        // Binary operator
        std::string lhs = AI_BuildExpression(node + 1, end, depth + 1);

        const AI_tdstNodeInterpret* next = node + 1;
        while (next < end && next->eType != AI_E_ti_EndTree) {
          if (next->ucDepth == depth + 1) break;
          if (next->ucDepth <= depth) { next = nullptr; break; }
          ++next;
        }
        std::string rhs = next ? AI_BuildExpression(next, end, depth + 1) : "";

        expr += lhs + " = " + rhs;
      } else {
        // Binary operator
        std::string lhs = AI_BuildExpression(node + 1, end, depth + 1);

        const AI_tdstNodeInterpret* next = node + 1;
        while (next < end && next->eType != AI_E_ti_EndTree) {
          if (next->ucDepth == depth + 1) break;
          if (next->ucDepth <= depth) { next = nullptr; break; }
          ++next;
        }
        std::string rhs = next ? AI_BuildExpression(next, end, depth + 1) : "";

        expr += "(" + lhs + " " + std::string(text) + " " + rhs + ")";
      }
      ++node;
      break;
    }

    case AI_E_ti_KeyWord: {
      // Special forms like If/Then/Else
      if (node->uParam.ulValue == AI_E_kw_If) {
        std::string cond = AI_BuildExpression(node + 1, end, depth + 1);
        expr += "if " + cond;
      }
      else if (node->uParam.ulValue == AI_E_kw_Then) {
        std::string body = AI_BuildExpression(node + 1, end, depth + 1);
        expr += " then " + body;
      }
      else if (node->uParam.ulValue == AI_E_kw_Else) {
        std::string body = AI_BuildExpression(node + 1, end, depth + 1);
        expr += " else " + body;
      }
      else {
        expr += text;
      }
      ++node;
      break;
    }

    default:
      // Leaf
      expr += text;
      ++node;
      break;
    }
  }

  return expr;
}

// Builds expression for a single node (and consumes its subtree).
// Returns <expr, nextNodeAfterThisSubtree>
std::pair<std::string, const AI_tdstNodeInterpret*>
AI_BuildNodeExpression(const AI_tdstNodeInterpret* node,
  const AI_tdstNodeInterpret* end)
{
  if (node >= end || node->eType == AI_E_ti_EndTree)
    return { "", node };

  unsigned char depth = node->ucDepth;
  const char* text = AI_GetNodeString(node);

  std::string expr;

  switch (node->eType)
  {
  case AI_E_ti_Condition: {
    AI_tdeCondition cond = static_cast<AI_tdeCondition>(node->uParam.ulValue);
    if (cond == AI_E_cond_Not) {
      auto [rhs, after] = AI_BuildNodeExpression(node + 1, end);
      expr = "(!" + rhs + ")";
      return { expr, after };
    }
    else {
      auto [lhs, afterL] = AI_BuildNodeExpression(node + 1, end);
      auto [rhs, afterR] = AI_BuildNodeExpression(afterL, end);
      expr = "(" + lhs + " " + std::string(text) + " " + rhs + ")";
      return { expr, afterR };
    }
  }

  case AI_E_ti_Operator: {
    AI_tdeOperator op = static_cast<AI_tdeOperator>(node->uParam.ulValue);
    if (op == AI_E_op_UnaryMinus || op == AI_E_op_VectorUnaryMinus) {
      auto [rhs, after] = AI_BuildNodeExpression(node + 1, end);
      expr = "(-" + rhs + ")";
      return { expr, after };
    }
    else {
      auto [lhs, afterL] = AI_BuildNodeExpression(node + 1, end);
      auto [rhs, afterR] = AI_BuildNodeExpression(afterL, end);
      expr = "(" + lhs + " " + std::string(text) + " " + rhs + ")";
      return { expr, afterR };
    }
  }

  case AI_E_ti_KeyWord: {
    if (node->uParam.ulValue == AI_E_kw_If) {
      auto [cond, after] = AI_BuildNodeExpression(node + 1, end);
      expr = "if " + cond;
      return { expr, after };
    }
    else if (node->uParam.ulValue == AI_E_kw_Then) {
      auto [body, after] = AI_BuildNodeExpression(node + 1, end);
      expr = "then " + body;
      return { expr, after };
    }
    else if (node->uParam.ulValue == AI_E_kw_Else) {
      auto [body, after] = AI_BuildNodeExpression(node + 1, end);
      expr = "else " + body;
      return { expr, after };
    }
    else {
      return { std::string(text), node + 1 };
    }
  }

  default:
    // Leaf
    return { std::string(text), node + 1 };
  }
}


void DR_DLG_AIModel_Draw_Rule(const AI_tdstTreeInterpret& tree)
{
  const AI_tdstNodeInterpret* node = &tree.p_stNodeInterpret[0];
  int stackDepth = 1;
  const AI_tdstNodeInterpret* end = node;
  while (end->eType != AI_E_ti_EndTree) ++end;

  for (;; ++node) {
    if (node->eType == AI_E_ti_EndTree)
      break;

    const unsigned char depth = node->ucDepth;

    if (depth > stackDepth)
      continue;

    while (stackDepth > depth) {
      ImGui::TreePop();
      --stackDepth;
    }

    const AI_tdstNodeInterpret* next = node + 1;
    const bool has_next = (next->eType != AI_E_ti_EndTree);
    const bool has_children = has_next && (next->ucDepth > depth);

    if (!has_children) {
      ImGui::TreeNodeEx((void*)node,
        ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen,
        "%s", AI_GetNodeString(node));
    }
    else {
      bool open = ImGui::TreeNode((void*)node, "%s", AI_GetNodeString(node));
      if (open) {
        ++stackDepth;
      }
      else {
        auto [expr, after] = AI_BuildNodeExpression(node, end);
        if (!expr.empty()) {
          ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
          ImGui::TextDisabled("%s", expr.c_str());
          ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());
        }
      }
    }
  }

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