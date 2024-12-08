#include "dialogs.h"
#include "ACP_Ray2.h"
#include "derust.h"
#include "dsgvarnames.h"
#include <sstream>
#include "imgui.h"

void DR_DLG_Draw(HWND window_r2) {
  
  DR_DLG_Menu_Draw();
  DR_DLG_Hierarchy_Draw();
  DR_DLG_Playback_Draw();
  DR_DLG_PracticeTools_Draw();

  if (g_DR_rayman == NULL) return;
  char* state;
  int rule = AI_fn_bGetDsgVar(g_DR_rayman, DV_RAY_RAY_Etat, NULL, (void**)& state);
  std::stringstream ss;
  ss << "State: ";
  ss << (int)(*state);
  ImGui::Text(ss.str().c_str());
}