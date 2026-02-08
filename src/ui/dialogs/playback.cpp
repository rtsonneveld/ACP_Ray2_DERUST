#include "playback.hpp"
#include "ui/ui.hpp"
#include "ui/settings.hpp"

// C INCLUDE
#include "mod/state.h"
#include "mod/savestates.h"
#include "mod/recording.h"

#include <ACP_Ray2.h>

void DR_DLG_Playback_Draw() {

  if (!g_DR_settings.dlg_playback) return;

  if (ImGui::Begin("Playback", &g_DR_settings.dlg_playback, ImGuiWindowFlags_NoCollapse)) {

    if (IPT_M_bActionJustValidated(IPT_E_Entry_PulseStep)) { // F6
      g_DR_Playback.unpause = true;
      g_DR_Playback.framestep = true;
    }

    DR_InputRecording_State state = DR_Recording_CurrentState();
    switch(state) {
      case DR_IR_State_Idle:
        
        if (ImGui::Button("Start recording")) {
          DR_Recording_Start();
        }
        ImGui::SameLine();

        break;
      case DR_IR_State_Recording:

        if (ImGui::Button("Stop recording")) {
          DR_Recording_Stop();
        }
        ImGui::SameLine();

        break;
      case DR_IR_State_Playback:

        if (ImGui::Button("Stop playback")) {
          DR_Recording_StopPlayback();
        }
        ImGui::SameLine();

        break;

    }

    if (GAM_g_stEngineStructure->bEngineIsInPaused || g_DR_Playback.pause) {

      if (ImGui::Button("Play (F5)") || IPT_M_bActionJustValidated(IPT_E_Entry_StartStopStep)) { // F5
        g_DR_Playback.unpause = true;
      }
      ImGui::SameLine();
      if (ImGui::Button("Step (F6)")) {
        g_DR_Playback.unpause = true;
        g_DR_Playback.framestep = true;
      }
    } else {
      if (ImGui::Button("Pause (F5)") || IPT_M_bActionJustValidated(IPT_E_Entry_StartStopStep)) {
        g_DR_Playback.pause = true;
      }

    }
  }
  ImGui::End();
}