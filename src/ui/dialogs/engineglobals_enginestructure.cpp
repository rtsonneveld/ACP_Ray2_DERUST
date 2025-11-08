#include <imgui.h>
#include "ui/custominputs.hpp"
#include <ACP_Ray2.h>
#include <stdio.h>

// Helper to show pointer as read-only hex
template<typename T>
void ShowPointerHex(const char* label, T* ptr)
{
  ImGui::Text("%s: 0x%p", label, (void*)ptr);
}


void Show_TMR_tdstEngineTimerStructure(const TMR_tdstEngineTimerStructure& timer)
{
  ImGui::Text("Frame Number: %lu", timer.ulFrameNumber);
  ImGui::Text("Timer Handle: %u", timer.wTimerHandle);
  ImGui::Text("Current Timer Count: %lu", timer.ulCurrentTimerCount);
  ImGui::Text("Delta Timer Count: %lu", timer.ulDeltaTimerCount);
  ImGui::Text("Useful Delta Time: %lu", timer.ulUsefulDeltaTime);
  ImGui::Text("Pause Time: %lu", timer.ulPauseTime);
  ImGui::Text("Tick Per Ms: %lu", timer.ulTickPerMs);
}

void Show_GLD_tdstViewportAttributes(const GLD_tdstViewportAttributes& attr)
{
  ImGui::Text("Width: %lu", attr.dwWidth);
  ImGui::Text("Height: %lu", attr.dwHeight);
  ImGui::Text("TopInPix: %lu", attr.dwTopInPix);
  ImGui::Text("LeftInPix: %lu", attr.dwLeftInPix);
  ImGui::Text("OffsetPosX: %ld", attr.lOffsetPosX);
  ImGui::Text("OffsetPosY: %ld", attr.lOffsetPosY);
  ShowPointerHex("p_cVirtualScreen", attr.p_cVirtualScreen);
  ShowPointerHex("p_vSpecificToXD", attr.p_vSpecificToXD);
  ImGui::Text("hDevice: %d", attr.hDevice);
  ImGui::Text("hViewport: %d", attr.hViewport);
}

void DR_DLG_EngineGlobals_Draw_EngineStructure()
{
  GAM_tdstEngineStructure* eng = GAM_g_stEngineStructure;

  ImGui::SeparatorText("Engine Mode");// --- Enums as dropdowns ---
  const char* engineModes[] = {
      "Invalid", "StartingProgram", "StoppingProgram", "EnterGame",
      "QuitGame", "EnterLevel", "ChangeLevel", "DeadLoop",
      "PlayerDead", "Playing"
  };
  int engineMode = eng->eEngineMode;
  if (ImGui::Combo("Engine Mode", &engineMode, engineModes, IM_ARRAYSIZE(engineModes))) {
    eng->eEngineMode = (GAM_tdeEngineMode)engineMode;
  }

  const char* inputModes[] = { "Normal", "Commands" };
  int inputMode = eng->eInputMode;
  if (ImGui::Combo("Input Mode", &inputMode, inputModes, IM_ARRAYSIZE(inputModes))) {
    eng->eInputMode = (GAM_tdeInputMode)inputMode;
  }
  const char* displayModes[] = {
        "DisplayAll (0xFF)", "DisplayHitPoints (0x01)",
        "DisplayGameSave (0x02)", "DisplayNothing (0x00)"
  };
  int displayModeIndex = 0;
  switch (eng->eDisplayFixMode)
  {
    case E_DFM_DisplayAll: displayModeIndex = 0; break;
    case E_DFM_DisplayHitPoints: displayModeIndex = 1; break;
    case E_DFM_DisplayGameSave: displayModeIndex = 2; break;
    case E_DFM_DisplayNothing: displayModeIndex = 3; break;
  }
  if (ImGui::Combo("Display Fix Mode", &displayModeIndex, displayModes, IM_ARRAYSIZE(displayModes)))
  {
    static const GAM_tdeDisplayFixMode modeMap[] = {
        E_DFM_DisplayAll, E_DFM_DisplayHitPoints,
        E_DFM_DisplayGameSave, E_DFM_DisplayNothing
    };
    eng->eDisplayFixMode = modeMap[displayModeIndex];
  }

  ImGui::Text("Display Mode: %lu", eng->ulDisplayMode);

  ImGui::SeparatorText("Level Names");
  ImGui::InputText("Current Level", eng->szLevelName, IM_ARRAYSIZE(eng->szLevelName));
  ImGui::InputText("Next Level", eng->szNextLevelName, IM_ARRAYSIZE(eng->szNextLevelName));
  ImGui::InputText("First Level", eng->szFirstLevelName, IM_ARRAYSIZE(eng->szFirstLevelName));

  ImGui::SeparatorText("Timer");
  if (ImGui::TreeNode("stEngineTimer"))
  {
    Show_TMR_tdstEngineTimerStructure(eng->stEngineTimer);
    ImGui::TreePop();
  }

  ImGui::SeparatorText("Viewport");
  ImGui::Text("hGLDDevice: %d", eng->hGLDDevice);
  ImGui::Text("hGLDViewport: %d", eng->hGLDViewport);

  if (ImGui::TreeNode("stViewportAttr"))
  {
    Show_GLD_tdstViewportAttributes(eng->stViewportAttr);
    ImGui::TreePop();
  }

  ShowPointerHex("p_stGameViewportCamera", eng->p_stGameViewportCamera);
  ShowPointerHex("hDrawSem", eng->hDrawSem);

  if (ImGui::TreeNode("stFixViewportAttr"))
  {
    Show_GLD_tdstViewportAttributes(eng->stFixViewportAttr);
    ImGui::TreePop();
  }

  ShowPointerHex("p_stFixCamera", eng->p_stFixCamera);
  ShowPointerHex("a_hViewportArray", eng->a_hViewportArray);
  //ShowPointerHex("hCameraList", eng.hCameraList); TODO
  //ShowPointerHex("hFamilyList", eng.hFamilyList); TODO
  //ShowPointerHex("hAlwaysActiveCharactersList", eng.hAlwaysActiveCharactersList); TODO
  ShowPointerHex("g_hMainActor", eng->g_hMainActor);
  ShowPointerHex("g_hNextMainActor", eng->g_hNextMainActor);
  ShowPointerHex("g_hStdCamCharacter", eng->g_hStdCamCharacter);
  ShowPointerHex("p_stLanguageTable", eng->p_stLanguageTable);
  ShowPointerHex("hLevelNameList", eng->hLevelNameList);
  // --- Continue from previous code block ---

  ImGui::SeparatorText("Positions");
  InputCompletePosition("Main Character Position", &eng->stMainCharacterPosition);
  InputCompletePosition("Main Camera Position", &eng->stMainCameraPosition);

  ImGui::SeparatorText("Map / Pause State");
  ImGui::Text("SubMap Number: %ld", eng->lSubMapNumber);

  ImGui::Checkbox("Engine Is Paused", (bool*)&eng->bEngineIsInPaused);
  ImGui::Checkbox("Engine Has Paused", (bool*)&eng->bEngineHasInPaused);

  ImGui::SeparatorText("Levels");
  ImGui::Text("Number of Levels: %u", eng->ucNumberOfLevels);
  ImGui::Text("Current Level: %u", eng->ucCurrentLevel);
  ImGui::Text("Previous Level: %u", eng->ucPreviousLevel);
  ImGui::Text("Exit ID to Quit Prev Level: %u", eng->ucExitIdToQuitPrevLevel);
  ImGui::Text("Level Global Counter: %u", eng->ucLevelGlobalCounter);

  // Level names (read-only display, since editing 80×MAX_NAME_LEVEL could be messy)
  if (ImGui::TreeNode("a_szLevelName"))
  {
    for (int i = 0; i < eng->ucNumberOfLevels && i < 80; ++i)
    {
      char label[64];
      snprintf(label, sizeof(label), "Level[%02d]", i);
      ImGui::Text("%s: %s", label, eng->a_szLevelName[i]);
    }
    ImGui::TreePop();
  }

  ImGui::SeparatorText("Demo / Language / State Flags");
  ImGui::Checkbox("Demo Mode", (bool*)&eng->xDemoMode);
  ImGui::Checkbox("Engine Frozen", (bool*)&eng->bEngineFrozen);
  ImGui::Checkbox("Resurrection", (bool*)&eng->bResurrection);

  ImGui::Text("Current Language: %u", eng->ucCurrentLanguage);
  ImGui::Text("Number of Languages: %u", eng->ucNbLanguages);

  ImGui::SeparatorText("Camera");
  ImGui::Text("Camera Mode: %d", eng->cCameraMode);

}