#pragma once
#include <ACP_Ray2.h>
#include "mod/glmradar.h"

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned long g_DR_totalFrameCount;
extern unsigned long g_DR_currentMapFrameCount;

typedef struct GlmRadarData {
  MTH3D_tdstVector g_DR_glmDirectionFrom;
  MTH3D_tdstVector g_DR_glmDirectionTo;
  MTH3D_tdstVector g_DR_glmTeleport;
  MTH3D_tdstVector g_DR_glmCoordinateList[GLMRadar_NumChecks];
} GlmRadarData;

extern GlmRadarData g_DR_glmData[GLMRadar_MaxBookmarks];
extern MTH3D_tdstVector g_DR_glmBookmarks[GLMRadar_MaxBookmarks];
extern unsigned int g_DR_glmBookmarkCount;

extern HIE_tdstSuperObject* g_DR_rayman;
extern HIE_tdstSuperObject* g_DR_global;
extern HIE_tdstSuperObject* g_DR_selectedObject;

#ifdef __cplusplus
    }
#endif