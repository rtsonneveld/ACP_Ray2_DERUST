#pragma once

#include "framework.h"
#include <assert.h>

#define Std_C_MiscFlag_Always	0x10
#define GLI_C_lIsNotGrided		8
#define MTH3D_M_vNullVector( VectDest)    { (VectDest)->x=(VectDest)->y=(VectDest)->z=0.0f; }

typedef long tdObjectType;
#define C_InvalidObjectType -1
#define C_AlwaysObjectType  0x00010000

HIE_tdstFamilyList* fn_hFindFamily(tdObjectType otFamilyType);

AI_tdstAIModel* fn_p_stAllocAIModel();

struct tdstEngineObject_* fn_p_stAllocateAlwaysEngineObject(tdObjectType otObjectFamilyType, tdObjectType otObjectModelType, tdObjectType otObjectPersonalType);

void fn_vAddAnAlwaysModel(HIE_tdstEngineObject* p_stAlwaysObject);

void* fnp_vGameMallocInHLM(unsigned long ulSize);