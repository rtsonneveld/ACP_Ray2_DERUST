#include "practicetools.h"
#include <imgui.h>
#include <ACP_Ray2.h>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <map>
#include <string>
#include <vector>
#include <functional>
#include <implot/implot.h>
#include "util.h"
#include <derust.h>

#define MAX_POS_DELTA 50.0f

bool DR_DLG_PracticeTools_Enabled = FALSE;
int historySizeSeconds = 10;

struct PlotData {
  bool enabled;
  int historySize;
  std::vector<float> history;
  float minValue = 0.0f;
  float maxValue = 0.0f;
  float avgValue = 0.0f;

  PlotData() : PlotData(600) {}
  PlotData(int historySize) : historySize(historySize), history(historySize, 0.0f) {}
};

// Global storage for plots
std::map<std::string, PlotData> plots;

// Function pointers to update values for each plot
std::map<std::string, std::function<float()>> valueProviders;

bool isInitialized = false;
MTH3D_tdstVector lastPositionHorizontalSpeed;
MTH3D_tdstVector lastPositionSpeed;

MTH3D_tdstVector lastRaymanPos(0.0, 0.0, 0.0);
MTH3D_tdstVector currentRaymanPos(0.0, 0.0, 0.0);

void ClearHistory() {

  lastPositionHorizontalSpeed = lastPositionSpeed = currentRaymanPos;
  for (auto& [key, data] : valueProviders) {
    PlotData plotData(historySizeSeconds * 60);
    std::fill(plotData.history.begin(), plotData.history.end(), valueProviders[key]());
    plots[key] = plotData;
  }
}

void DR_DLG_PracticeTools_Init() {

  DNM_tdstDynamicsBaseBlock base = g_DR_rayman->hLinkedObject.p_stActor->hDynam->p_stDynamics->stDynamicsBase;
  lastRaymanPos = currentRaymanPos = base.stCurrentMatrix.stPos;

  // Initialize plots and their value providers
  valueProviders["Z"] = []() {
    DNM_tdstDynamicsBaseBlock base = g_DR_rayman->hLinkedObject.p_stActor->hDynam->p_stDynamics->stDynamicsBase;
    return base.stCurrentMatrix.stPos.z;
  };
  valueProviders["Horizontal Speed"] = []() {

    DNM_tdstDynamicsBaseBlock base = g_DR_rayman->hLinkedObject.p_stActor->hDynam->p_stDynamics->stDynamicsBase;
    MTH3D_tdstVector speed;
    MTH3D_M_vSubVector(&speed, &base.stCurrentMatrix.stPos, &lastPositionHorizontalSpeed);
    MTH3D_M_vMulVectorScalar(&speed, &speed, 60.0f); // 60 frames in one second
    lastPositionHorizontalSpeed = base.stCurrentMatrix.stPos;
    return sqrtf(speed.x * speed.x + speed.y * speed.y);
    };
  valueProviders["Speed"] = []() {

    DNM_tdstDynamicsBaseBlock base = g_DR_rayman->hLinkedObject.p_stActor->hDynam->p_stDynamics->stDynamicsBase;
    MTH3D_tdstVector speed;
    MTH3D_M_vSubVector(&speed, &base.stCurrentMatrix.stPos, &lastPositionSpeed);
    MTH3D_M_vMulVectorScalar(&speed, &speed, 60.0f); // 60 frames in one second
    lastPositionSpeed = base.stCurrentMatrix.stPos;
    return sqrtf(speed.x * speed.x + speed.y * speed.y + speed.z * speed.z);
  };

  ClearHistory();
}

void UpdateHistory(const std::string& plotKey) {
  auto& plot = plots[plotKey];
  float newValue = valueProviders[plotKey]();

  // Shift history and add the new value
  std::rotate(plot.history.begin(), plot.history.begin() + 1, plot.history.end());
  plot.history.back() = newValue;

  // Calculate min, max, and average values
  plot.minValue = plot.history[0];
  plot.maxValue = plot.history[0];
  float sum = 0.0f;

  for (float value : plot.history) {
    if (value < plot.minValue) plot.minValue = value;
    if (value > plot.maxValue) plot.maxValue = value;
    sum += value;
  }

  plot.avgValue = sum / plot.historySize;
}

void DrawPlot(const std::string& plotKey) {
  auto& plot = plots[plotKey];

  if (!plot.enabled) return;

  // Draw plot
  const float verticalPadding = 2.5f;
  float minLimit = plot.minValue - verticalPadding;
  float maxLimit = plot.maxValue + verticalPadding;

  ImPlot::SetNextAxesLimits(0, plot.historySize, minLimit, maxLimit, ImPlotCond_Always);
  if (ImPlot::BeginPlot((plotKey+"##Plot").c_str(), ImVec2(-1, 0))) {

    int plotHistorySize = plot.history.size();

    ImPlot::PlotLine(("##" + plotKey).c_str(), plot.history.data(), plotHistorySize);
    ImPlot::Annotation(plotHistorySize, minLimit, ImVec4(1, 1, 1, 1), ImVec2(0, 0), true, "Min: %.2f", plot.minValue);
    ImPlot::Annotation(plotHistorySize, maxLimit, ImVec4(1, 1, 1, 1), ImVec2(0, 0), true, "Max: %.2f", plot.maxValue);
    ImPlot::SetNextFillStyle(ImVec4(1, 0, 0, 0.5f));
    ImPlot::PlotInfLines(("Avg##" + plotKey).c_str(), &plot.avgValue, 1, ImPlotInfLinesFlags_Horizontal);
    ImPlot::Annotation(0, plot.avgValue, ImVec4(1, 0, 0, 0.25f), ImVec2(0, 0), true, "Avg: %.2f", plot.avgValue);

    float curValue = plot.history.back();
    ImPlot::Annotation(0, minLimit, ImVec4(1, 1, 1, 1), ImVec2(0, 0), true, "Cur: %.2f", curValue);

    ImPlot::EndPlot();
  }
}

void DetectTeleports()
{
  DNM_tdstDynamicsBaseBlock base = g_DR_rayman->hLinkedObject.p_stActor->hDynam->p_stDynamics->stDynamicsBase;
  currentRaymanPos = base.stCurrentMatrix.stPos;

  MTH3D_tdstVector diff;
  MTH3D_M_vSubVector(&diff, &lastRaymanPos, &currentRaymanPos);
  if (MTH3D_M_xSqrNormVector(&diff) > MAX_POS_DELTA) {

    ClearHistory();
  }
  lastRaymanPos = currentRaymanPos;
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

  if (ImGui::Begin("Practice Tools", &DR_DLG_PracticeTools_Enabled, ImGuiWindowFlags_NoCollapse)) {

    ImGui::BeginGroup();
    for (auto& [plotKey, plotData] : plots) {

      ImGui::Checkbox(plotKey.c_str(), &plotData.enabled);
      ImGui::SameLine();
    }
    ImGui::EndGroup();

    if (ImGui::SliderInt("History (seconds)", &historySizeSeconds, 1, 20)) {
      ClearHistory();
    }

    if (!GAM_g_stEngineStructure->bEngineIsInPaused) {

      for (auto& [plotKey, plotData] : plots) {
        UpdateHistory(plotKey);
      }

      DetectTeleports();
    }

    for (auto& [plotKey, plotData] : plots) {

      DrawPlot(plotKey);
    }
  }


  ImGui::End();

}
