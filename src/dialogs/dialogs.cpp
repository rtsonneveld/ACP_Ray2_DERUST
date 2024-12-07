#include "dialogs.h"

void DR_DLG_Draw(HWND window_r2) {
  DR_DLG_Menu_Draw(window_r2);
  DR_DLG_Hierarchy_Draw();
  DR_DLG_Playback_Draw();
}