#include "playback.h"
#include <imgui.h>
#include "state.h"
#include <windows.h>

bool DR_DLG_Playback_Enabled = FALSE;

void DR_DLG_Playback_Draw() {

  if (!DR_DLG_Playback_Enabled) return;

  ImGuiWindowClass windowClass;
  windowClass.ViewportFlagsOverrideSet = ImGuiViewportFlags_TopMost | ImGuiViewportFlags_NoTaskBarIcon;
  ImGui::SetNextWindowClass(&windowClass);

  bool open = true;
  if (ImGui::Begin("Playback", &DR_DLG_Playback_Enabled, ImGuiWindowFlags_NoCollapse)) {

    if (IPT_M_bActionJustValidated(IPT_E_Entry_PulseStep)) { // F6
      g_DR_Playback.unpause = true;
      g_DR_Playback.framestep = true;
    }

    if (GAM_g_stEngineStructure->bEngineIsInPaused || g_DR_Playback.pause) {
      if (ImGui::Button("Play") || IPT_M_bActionJustValidated(IPT_E_Entry_StartStopStep)) { // F5
        g_DR_Playback.unpause = true;
      }
      if (ImGui::Button("Step")) {
        g_DR_Playback.unpause = true;
        g_DR_Playback.framestep = true;
      }
    } else {
      if (ImGui::Button("Pause") || IPT_M_bActionJustValidated(IPT_E_Entry_StartStopStep)) {
        g_DR_Playback.pause = true;
      }
    }
  }
  ImGui::End();
}