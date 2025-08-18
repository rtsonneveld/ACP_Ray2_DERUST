#pragma once
#include "imgui.h"
#include "imnodes.h"
#include <vector>
#include <string>
#include <sstream>

#include <ACP_Ray2.h>

struct VisualLink {
  int id;
  int startAttr;
  int endAttr;
};

class ComportNodeGraph
{
public:
  ComportNodeGraph();

  // Render the graph for a script tree
  void Render(AI_tdstTreeInterpret* tree);

private:
  int m_nodeCounter;
  int m_linkCounter;
  std::vector<VisualLink> m_links;

  void buildGraph(AI_tdstNodeInterpret* nodes, int& x, int& y, int parentOutPin);

  void drawLinks();

  // --- Helpers ---
  bool isIfOrElse(AI_tdstNodeInterpret* node);
  std::string nodeText(AI_tdstNodeInterpret* node);
  bool isFunctionOrProcedure(AI_tdstNodeInterpret* node);
  int countParameters(AI_tdstNodeInterpret* node, AI_tdstNodeInterpret* base);
};
