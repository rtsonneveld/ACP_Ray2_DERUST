#include "dialogs.hpp"
#include <Windows.h>

void DR_DLG_Init(HWND window_r2) {
  DR_DLG_Utils_Init();
}

void DR_DLG_Draw(HWND window_r2) {
  
  DR_DLG_Menu_Draw();
  DR_DLG_Hierarchy_Draw();
  DR_DLG_Playback_Draw();
  DR_DLG_Stats_Draw();
  DR_DLG_Inspector_Draw();
  DR_DLG_AIModel_Draw();
  DR_DLG_DebugWindow_Draw();
  DR_DLG_Utils_Draw();
  DR_DLG_EngineGlobals_Draw();
  DR_DLG_Options_Draw();
}