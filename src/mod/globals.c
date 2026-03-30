#include "globals.h"

unsigned long g_DR_totalFrameCount = 0;
unsigned long g_DR_currentMapFrameCount = 0;

GlmRadarData g_DR_glmData[GLMRadar_MaxBookmarks];
MTH3D_tdstVector g_DR_glmBookmarks[GLMRadar_MaxBookmarks];
unsigned int g_DR_glmBookmarkCount = 0;

HIE_tdstSuperObject* g_DR_rayman = NULL;
HIE_tdstSuperObject* g_DR_global = NULL;
HIE_tdstSuperObject* g_DR_selectedObject = NULL;