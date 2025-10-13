#pragma once

#include "cpa_functions.h"

#ifdef __cplusplus
extern "C" {
#endif

HIE_tdstEngineObject* SPO_Actor(HIE_tdstSuperObject* obj);

#define C_fOneBy255 0.003921568f
#define M_stHexToGEOColor( ulRGB ) (GEO_tdstColor){ (float)(((ulRGB)>>16)&0xFF)*C_fOneBy255, (float)(((ulRGB)>>8)&0xFF)*C_fOneBy255, (float)((ulRGB)&0xFF)*C_fOneBy255 }

typedef struct tdstDsgVarArray
{
  enum tdeDsgVarTypeId_ eType;
  unsigned char ucMaxSize;
  AI_tduGetSetParamForNode d_ArrayElement[255];
} tdstDsgVarArray;

void UTIL_vDrawBar(float x, float y, float barWidth, float barHeight, GEO_tdstColor color);
/// <summary>
/// Retrieves a pointer to a dsgvar's current value for a given actor.
/// </summary>
/// <param name="actor">The actor.</param>
/// <param name="dsgVarIndex">The index of the dsgvar.</param>
/// <returns>A pointer to the current value of the variable in the brain's memory.</returns>
void* ACT_DsgVarPtr(HIE_tdstEngineObject* actor, int dsgVarIndex);

void ACT_SetBooleanInArray(HIE_tdstEngineObject* engineObj, int dsgVarIndex, unsigned long bitIndex, char state);
char ACT_GetBooleanInArray(HIE_tdstEngineObject* engineObj, int dsgVarIndex, unsigned long bitIndex);
unsigned long ACT_GetNumberOfBooleanInArray(HIE_tdstEngineObject* engineObj, int dsgVarIndex, unsigned long firstIndex, unsigned long lastIndex);

/// <summary>
/// Sets the transparency of a SuperObject
/// </summary>
/// <param name="spo">The SuperObject</param>
/// <param name="alpha">Alpha where 0 is fully transparent and 1 is fully opaque</param>
void SPO_SetTransparency(HIE_tdstSuperObject* spo, float alpha);

#ifdef __cplusplus
    }
#endif