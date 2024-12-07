#include "practicetools.h"
#include <imgui.h>
#include <ACP_Ray2.h>
#include <algorithm>
#include <sstream>
#include <iomanip>

#define HISTORY_SIZE 600
#define RaymanObject HIE_fn_p_stFindObjectByName("rayman")

bool DR_DLG_PracticeTools_Enabled = FALSE;

float heightHistory[HISTORY_SIZE] = {};
float minHeight = 0.0f, maxHeight = 0.0f;
bool isInitialized = false;

void DR_DLG_PracticeTools_Init() {
  std::fill_n(heightHistory, HISTORY_SIZE, RaymanObject->p_stGlobalMatrix->stPos.z);
}

void UpdateHistory() {

  minHeight = heightHistory[0];
  maxHeight = heightHistory[0];

  // Update history
  for (int i = 0;i < HISTORY_SIZE;i++) {
    float value = heightHistory[i];
    if (value < minHeight) {
      minHeight = value;
    }
    else if (value > maxHeight) {
      maxHeight = value;
    }
    if (i < HISTORY_SIZE - 1) {
      heightHistory[i] = heightHistory[i + 1];
    }
    else {
      heightHistory[i] = RaymanObject->p_stGlobalMatrix->stPos.z;
    }
  }
}

void DR_DLG_PracticeTools_Draw() {

  if (!DR_DLG_PracticeTools_Enabled) return;

  if (!isInitialized) {
    DR_DLG_PracticeTools_Init();
    isInitialized = true;
  }

  ImGuiWindowClass windowClass;
  windowClass.ViewportFlagsOverrideSet = ImGuiViewportFlags_TopMost | ImGuiViewportFlags_NoTaskBarIcon;
  ImGui::SetNextWindowClass(&windowClass);

  ImGui::SetNextWindowPos(ImVec2(200, 200), ImGuiCond_Once);
  ImGui::SetNextWindowSize(ImVec2(200, 500), ImGuiCond_Once);

  if (ImGui::Begin("Practice Tools", &DR_DLG_PracticeTools_Enabled, ImGuiWindowFlags_NoCollapse))
  {
    UpdateHistory();

    std::stringstream sstream;
    sstream << "Rayman.Z, min: ";
    sstream << std::fixed << std::setprecision(2) << minHeight;
    sstream << ", max: ";
    sstream << std::fixed << std::setprecision(2) << maxHeight;
    ImGui::PlotLines("##Rayman.Z", heightHistory, HISTORY_SIZE, 0, sstream.str().c_str(), minHeight, maxHeight, ImVec2(ImGui::GetContentRegionAvail().x, 200.0f));
  }
  ImGui::End();
}