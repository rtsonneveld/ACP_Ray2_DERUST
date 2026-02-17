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
        
        if (ImGui::Button("Start Recording")) {
          DR_Recording_Start();
        }
        ImGui::SameLine();
        if (ImGui::Button("Start Playback")) {
          DR_Recording_StartPlayback();
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

      ImGui::SameLine();
    } else {
      if (ImGui::Button("Pause (F5)") || IPT_M_bActionJustValidated(IPT_E_Entry_StartStopStep)) {
        g_DR_Playback.pause = true;
      }

      ImGui::SameLine();
    }

    if (ImGui::Button("Save Recording")) {
      DR_Recording_Save();
    }

    if (ImGui::Button("Load Recording")) {
      DR_Recording_Load();
    }

    switch (DR_Recording_CurrentState()) {
      case DR_IR_State_Idle:           ImGui::Text("Idle"); break;
      case DR_IR_State_Playback:       ImGui::Text("Playback"); break;
      case DR_IR_State_Recording:      ImGui::Text("Recording"); break;
      case DR_IR_State_Seeking:        ImGui::Text("Seeking"); break;
      case DR_IR_State_StartPlayback:  ImGui::Text("Start Playback"); break;
      case DR_IR_State_StartRecording: ImGui::Text("Start Recording"); break;
      case DR_IR_State_StartSeeking:   ImGui::Text("Start Seeking"); break;
    }
    
    ImGui::SameLine();

    ImGui::Text("Frame %u/%u", DR_recording.ulCurrentFrame, DR_recording.ulNumFrames);
    ImGui::SameLine();

    // Desync warning indicator
    if (DR_recording_desync != 0.0f) {
      ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));

      ImGui::Text("Desync: %f", DR_recording_desync);
    }
    else {
      ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));

      ImGui::Text("No desync", DR_recording_desync);
    }
    ImGui::PopStyleColor();

    unsigned long targetFrame = DR_recording.ulCurrentFrame;
    unsigned long minFrame = 0;


    if (DR_Recording_CurrentState() == DR_IR_State_Recording && IPT_M_bActionIsValidated(IPT_E_Entry_Action_Affiche_Jauge) && IPT_M_bActionIsValidated(IPT_E_Entry_Action_Strafe)) { // F6
      DR_Recording_SeekTo(targetFrame - 60);
    }

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

    if (ImGui::SliderScalar("##PlaybackSeek", ImGuiDataType_U32, &targetFrame, &minFrame, &DR_recording.ulNumFrames, "", ImGuiSliderFlags_AlwaysClamp)) {
      DR_Recording_SeekTo(targetFrame);
    }
  }
  ImGui::End();
}