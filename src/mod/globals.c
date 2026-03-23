#include "globals.h"

unsigned long g_DR_totalFrameCount = 0;
unsigned long g_DR_currentMapFrameCount = 0;

MTH3D_tdstVector g_DR_glmDirectionFrom;
MTH3D_tdstVector g_DR_glmDirectionTo;
MTH3D_tdstVector g_DR_glmTeleport;

HIE_tdstSuperObject* g_DR_rayman = NULL;
HIE_tdstSuperObject* g_DR_global = NULL;
HIE_tdstSuperObject* g_DR_selectedObject = NULL;