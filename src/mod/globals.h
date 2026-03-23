#pragma once
#include <ACP_Ray2.h>

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned long g_DR_totalFrameCount;
extern unsigned long g_DR_currentMapFrameCount;

extern MTH3D_tdstVector g_DR_glmDirectionFrom;
extern MTH3D_tdstVector g_DR_glmDirectionTo;
extern MTH3D_tdstVector g_DR_glmTeleport;

extern HIE_tdstSuperObject* g_DR_rayman;
extern HIE_tdstSuperObject* g_DR_global;
extern HIE_tdstSuperObject* g_DR_selectedObject;

#ifdef __cplusplus
    }
#endif