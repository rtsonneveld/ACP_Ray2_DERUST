#include "comportnodegraph.hpp"
#include "mod/ai_strings.h"

ComportNodeGraph::ComportNodeGraph()
{
  m_nodeCounter = 0;
  m_linkCounter = 0;
}

void ComportNodeGraph::Render(AI_tdstTreeInterpret* tree)
{
  m_links.clear();
  m_nodeCounter = 1;
  m_linkCounter = 1;

  int startX = 0;
  int startY = 0;

  ImNodes::BeginNodeEditor();
  buildGraph(tree->p_stNodeInterpret, startX, startY, -1);
  drawLinks();
  ImNodes::MiniMap();
  ImNodes::EndNodeEditor();
}

void ComportNodeGraph::buildGraph(AI_tdstNodeInterpret* nodes,
  int& x, int& y, int parentOutPin)
{
  int i = 0;

  while(true)
  {
    AI_tdstNodeInterpret* node = &nodes[i];

    // Stop the loop when the tree has ended
    if (node->eType == AI_E_ti_EndTree) {
      break;
    }

    int nodeId = m_nodeCounter++;
    int outPin = nodeId * 10 + 1;

    // Draw node
    ImNodes::BeginNode(nodeId);
    ImNodes::BeginNodeTitleBar();

    //ImGui::Text(AI_GetNodeString(&node).c_str());
    ImNodes::EndNodeTitleBar();

    if (isIfOrElse(node)) {
      int truePin = nodeId * 10 + 1;
      int falsePin = nodeId * 10 + 2;

      ImNodes::BeginOutputAttribute(truePin);
      ImGui::Text("True");
      ImNodes::EndOutputAttribute();

      ImNodes::BeginOutputAttribute(falsePin);
      ImGui::Text("False");
      ImNodes::EndOutputAttribute();

      ImNodes::SetNodeEditorSpacePos(nodeId, ImVec2(x, y));
      y += 150; // space branches vertically
    } else if (isFunctionOrProcedure(node)) {
      int numParams = countParameters(node, nodes + i);
      for (int p = 0; p < numParams; ++p) {
        ImNodes::BeginInputAttribute(nodeId * 100 + p);
        ImGui::Text("Param %d", p);
        ImNodes::EndInputAttribute();
      }
      ImNodes::BeginOutputAttribute(outPin);
      ImGui::Text("→");
      ImNodes::EndOutputAttribute();

      ImNodes::SetNodeEditorSpacePos(nodeId, ImVec2(x, y));
    } else {
      ImNodes::BeginOutputAttribute(outPin);
      ImGui::Text("→");
      ImNodes::EndOutputAttribute();
      ImNodes::SetNodeEditorSpacePos(nodeId, ImVec2(x, y));
    }

    ImNodes::EndNode();

    // Connect to parent
    if (parentOutPin != -1) {
      m_links.push_back({ m_linkCounter++, parentOutPin, nodeId * 10 + 1 });
    }

    // Move horizontally
    x += 150;
    i++;
  }
}

void ComportNodeGraph::drawLinks()
{
  for (auto& link : m_links) {
    ImNodes::Link(link.id, link.startAttr, link.endAttr);
  }
}

bool ComportNodeGraph::isIfOrElse(AI_tdstNodeInterpret* node)
{
  if (node->eType == AI_E_ti_KeyWord) {
    auto kw = node->uParam.cValue;
    return (kw == AI_E_kw_If || kw == AI_E_kw_If2 || kw == AI_E_kw_If4 || kw == AI_E_kw_If8 || kw == AI_E_kw_If16 || kw == AI_E_kw_Else);
  }
  return false;
}

bool ComportNodeGraph::isFunctionOrProcedure(AI_tdstNodeInterpret* node)
{
  return node->eType == AI_E_ti_Function || node->eType == AI_E_ti_Procedure;
}

int ComportNodeGraph::countParameters(AI_tdstNodeInterpret* node,
  AI_tdstNodeInterpret* base)
{
  unsigned char funcDepth = node->ucDepth;
  int count = 0;

  // Start after the function node itself
  for (int j = 1; true; ++j) {
    AI_tdstNodeInterpret* param = &base[j];
    if (param->ucDepth <= funcDepth)
      break; // parameters end when depth decreases
    if (param->ucDepth == funcDepth + 1)
      count++;
  }

  return count;
}