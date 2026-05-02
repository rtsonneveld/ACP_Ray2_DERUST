#include "playback.hpp"
#include "ui/ui.hpp"
#include "ui/settings.hpp"
#include "ui/custominputs.hpp"
#include "rendering/scene.hpp"

// C INCLUDE
#include "mod/state.h"
#include "mod/recording.h"
#include "mod/bruteforce.h"
#include "mod/globals.h"

#include <ACP_Ray2.h>

unsigned int rewindFrames = 60;

void DR_DLG_Playback_TabRecording()
{
  if (IPT_M_bActionJustValidated(IPT_E_Entry_PulseStep)) { // F6
    g_DR_Playback.unpause = true;
    g_DR_Playback.framestep = true;
  }

  DR_InputRecording_State state = DR_Recording_CurrentState();

  switch (state) {
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
    if (ImGui::Button("Resume recording")) {
      DR_Recording_ResumeRecording();
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
  }
  else {
    if (ImGui::Button("Pause (F5)") || IPT_M_bActionJustValidated(IPT_E_Entry_StartStopStep)) {
      g_DR_Playback.pause = true;
    }

    ImGui::SameLine();
  }

  if (ImGui::Button("Save Recording")) {
    DR_Recording_Save("recording.bin");
  }

  ImGui::SameLine();

  if (ImGui::Button("Load Recording")) {
    DR_Recording_Load("recording.bin");
  }

  ImGui::SameLine();

  ImGui::InputInt("Rewind amount", (int*)&rewindFrames);

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

    if (DR_recording.pCurrentFrame != NULL) {
      MTH3D_tdstVector pos = DR_recording.pCurrentFrame->stRaymanPosition;
      ImGui::Text("Desync: %f (%f, %f, %f)", DR_recording_desync, pos.x, pos.y, pos.z);
    }
  }
  else {
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));

    ImGui::Text("No desync", DR_recording_desync);
  }
  ImGui::PopStyleColor();

  unsigned long targetFrame = DR_recording.ulCurrentFrame;
  unsigned long minFrame = 0;

  if (DR_Recording_CurrentState() == DR_IR_State_Recording && IPT_M_bActionIsValidated(IPT_E_Entry_Action_Affiche_Jauge) && IPT_M_bActionIsValidated(IPT_E_Entry_Action_Strafe)) { // F6
    DR_Recording_SeekTo(targetFrame - rewindFrames, FALSE, FALSE);
  }

  if (ImGui::Button("<")) {
    DR_Recording_SeekTo(targetFrame - 1, FALSE, FALSE);
  }
  ImGui::SameLine();
  if (ImGui::Button(">")) {
    DR_Recording_SeekTo(targetFrame + 1, FALSE, FALSE);
  }
  ImGui::SameLine();

  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

  if (ImGui::SliderScalar("##PlaybackSeek", ImGuiDataType_U32, &targetFrame, &minFrame, &DR_recording.ulNumFrames, "", ImGuiSliderFlags_AlwaysClamp)) {
    DR_Recording_SeekTo(targetFrame, FALSE, FALSE);
  }
}

void DR_DLG_Playback_TabBruteForce() {
  ImGui::InputInt("Max attempts", (int*)&DR_bruteforceSettings.maxAttempts);
  ImGui::InputInt("Start frame", (int*)&DR_bruteforceSettings.randomizeStart);
  ImGui::InputInt("End frame (0 for all)", (int*)&DR_bruteforceSettings.randomizeEnd);
  ImGui::Checkbox("Randomize frame range", (bool*)&DR_bruteforceSettings.randomizeRange);
  InputPerso("Target object (default is MainActor)", &DR_bruteforceSettings.targetObject);
  ImGui::InputInt("DsgVar ID (-1 for none)", (int*)&DR_bruteforceSettings.dsgVarID);
  ImGui::InputFloat3("Target position", (float*)&DR_bruteforceSettings.targetPosition.x);
  ImGui::Checkbox("Invert score", (bool*)&DR_bruteforceSettings.invertScore);
  if (ImGui::Button("Copy from Selection")) {
    DR_bruteforceSettings.targetPosition = (g_DR_selectedObject != NULL ? g_DR_selectedObject:g_DR_rayman)->p_stGlobalMatrix->stPos;
  }
  if (ImGui::Button("Copy from Viewport position")) {
    DR_bruteforceSettings.targetPosition = FromGLMVec(scene.mouseLook.position);
  }
  ImGui::InputFloat("Target score", (float*)&DR_bruteforceSettings.targetScore);
  ImGui::Text("Current score: %f", DR_bruteforceSettings.currentScore);
  ImGui::Checkbox("Ignore Z", (bool*) & DR_bruteforceSettings.ignoreZ);
  ImGui::DragFloat("Analog modification chance", (float*)&DR_bruteforceSettings.analogModChance, 1.0f, 0.0f, 100.0f);
  ImGui::DragFloat("Analog randomness", (float*)&DR_bruteforceSettings.analogRandomness, 1.0f, 0.0f, 100.0f);
  ImGui::DragFloat("Input length modification chance", (float*)&DR_bruteforceSettings.inputLengthModChance, 1.0f, 0.0f, 100.0f);
  ImGui::Checkbox("Disqualify touching walls", (bool*)&DR_bruteforceSettings.disqualifyTouchingWalls);
  ImGui::Checkbox("Disqualify falling", (bool*)&DR_bruteforceSettings.disqualifyFalling);
  ImGui::Checkbox("Stop on map change", (bool*)&DR_bruteforceSettings.stopOnMapChange);

  if (!DR_bruteforceSettings.active) {
    if (ImGui::Button("Start bruteforce")) {
      DR_Bruteforce_Signal_Start();
    }
  } else {
    if (ImGui::Button("Stop bruteforce")) {
      DR_Bruteforce_Signal_Stop();
    }

    ImGui::Text("Attempt %u/%u", DR_bruteforceSettings.currentAttempt, DR_bruteforceSettings.maxAttempts); 
    ImGui::Text("Current best %.4f", DR_bruteforceSettings.currentBestScore); 
    ImGui::Text("Last attempt %.4f", DR_bruteforceSettings.lastAttemptScore); 
  }

  ImGui::SameLine();

  if (ImGui::Button("Load Best Attempt")) {
    DR_Recording_Load("bruteforcebest.bin");
  }
}

void DR_DLG_Playback_Draw() {

  if (!g_DR_settings.dlg_playback) return;

  if (ImGui::Begin("Playback", &g_DR_settings.dlg_playback, ImGuiWindowFlags_NoCollapse)) {

    ImGui::BeginTabBar("##PlaybackTabs", ImGuiTabBarFlags_None);

    if (ImGui::BeginTabItem("Recording")) {
      DR_DLG_Playback_TabRecording();
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Brute Force")) {
      DR_DLG_Playback_TabBruteForce();
      ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
  }
  ImGui::End();
}