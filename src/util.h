#pragma once

#include "cpa_functions.h"
#include "framework.h"

HIE_tdstEngineObject* SPO_Actor(HIE_tdstSuperObject* obj);

#define C_fOneBy255 0.003921568f
#define M_stHexToGEOColor( ulRGB ) (GEO_tdstColor){ (float)(((ulRGB)>>16)&0xFF)*C_fOneBy255, (float)(((ulRGB)>>8)&0xFF)*C_fOneBy255, (float)((ulRGB)&0xFF)*C_fOneBy255 }

void UTIL_vDrawBar(float x, float y, float barWidth, float barHeight, GEO_tdstColor color);

/// <summary>
/// Sets the transparency of a SuperObject
/// </summary>
/// <param name="spo">The SuperObject</param>
/// <param name="alpha">Alpha where 0 is fully transparent and 1 is fully opaque</param>
void SPO_SetTransparency(HIE_tdstSuperObject* spo, float alpha);