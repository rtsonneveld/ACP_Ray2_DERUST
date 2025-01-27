#include "playback.hpp"
#include "ui/ui.hpp"

// C INCLUDE
#include "mod/state.h"

#include <ACP_Ray2.h>

bool DR_DLG_Playback_Enabled = FALSE;
int colliderType = 52;

void DR_DLG_Playback_Draw() {

  if (!DR_DLG_Playback_Enabled) return;

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

    int step = 1;
    ImGui::InputScalar("Sudoko type", ImGuiDataType_U8, &colliderType, &step, 0, 0, 0);
    if (ImGui::Button("Sudoku")) {
      HIE_tdstEngineObject* pRayman = HIE_M_hSuperObjectGetActor(HIE_M_hGetMainActor());
      pRayman->hCollSet->stColliderInfo.ucColliderType = colliderType;
      pRayman->hCollSet->stColliderInfo.ucColliderPriority = 255;
      pRayman->hStandardGame->ucHitPoints--;
    }
  }
  ImGui::End();
}