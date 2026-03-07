// C INCLUDE
#include "imgui.h"
#include <stdio.h>
#include <ACP_Ray2.h>

#include <ui/custominputs.hpp>

void DR_DLG_Inspector_Draw_MS_SectInfo(HIE_tdstEngineObject* actor)
{
  SCT_tdstSectInfo* sectInfo = actor->hSectInfo;

  if (sectInfo == nullptr) {
    ImGui::Text("SectInfo is null");
    return;
  }

  InputSector("hCurrentSector", &sectInfo->hCurrentSector);
  ImGui::DragFloat3("stPreviousPosition", &sectInfo->stPreviousPosition.x);
}