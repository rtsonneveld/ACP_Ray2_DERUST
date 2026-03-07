#pragma once

#include "derust.h"
#include <assert.h>
#include <ACP_Ray2.h>

#ifdef __cplusplus
extern "C" {
#endif

#define Std_C_MiscFlag_Always	0x10
#define GLI_C_lIsNotGrided		8
#define MTH3D_M_vNullVector( VectDest)    { (VectDest)->x=(VectDest)->y=(VectDest)->z=0.0f; }

typedef long tdObjectType;
#define C_InvalidObjectType -1
#define C_AlwaysObjectType  0x00010000

#define MAX_ACTORS 1024
#define MAX_SECTORS 128

#define HIE_M_ForEachActor(actorVar) \
    HIE_tdstSuperObject* actors[MAX_ACTORS]; \
    int index = 0; \
    fn_aGetChildObjects(*GAM_g_p_stDynamicWorld, actors, &index, HIE_C_Type_Actor); \
    fn_aGetChildObjects(*GAM_g_p_stInactiveDynamicWorld, actors, &index, HIE_C_Type_Actor); \
    for (int i = 0; i < index && actors[i] != NULL; i++) \
        for (HIE_tdstSuperObject* actorVar = actors[i]; actorVar; actorVar = NULL)

#define HIE_M_ForEachSector(sectorVar) \
    HIE_tdstSuperObject* sectors[MAX_SECTORS]; \
    int index = 0; \
    fn_aGetChildObjects(*GAM_g_p_stFatherSector, sectors, &index, HIE_C_Type_Sector); \
    for (int i = 0; i < index && sectors[i] != NULL; i++) \
        for (HIE_tdstSuperObject* sectorVar = sectors[i]; sectorVar; sectorVar = NULL)

// Function pointers //

extern SCT_tdstSectInfo* (*fn_vSectInfoAlloc) (HIE_tdstEngineObject* engineObject);
extern void (*fn_v3dDataInitValueSAI) (GAM_tdst3dData* h_3dData);
extern void (*fn_v3dDataInit) (HIE_tdstEngineObject* p_stObject, AI_tdeObjectTreeInit eObjectInit);
extern char* (*AI_fn_p_vTrueAlloc) (unsigned int size);
extern char* (*fnp_vGameMallocInHLM) (unsigned int size);
extern void (*PLA_fn_vUpdateTransparencyForModules) (HIE_tdstSuperObject* superObject);
extern HIE_tdstSuperObject* (*fn_p_stAllocateAlways) (long otObjectModelType,	HIE_tdstSuperObject* p_stFatherSuperObject,	HIE_tdstSuperObject* _hGenerator,unsigned short uwAction, POS_tdstCompletePosition* p_stMatrix);
extern void (*fn_vKillEngineObjectOrAlwaysByPointer)(HIE_tdstEngineObject* p_stObject);

// End function pointers //

HIE_tdstFamilyList* fn_hFindFamily(tdObjectType otFamilyType);
HIE_tdstSuperObject** fn_aGetChildObjects(HIE_tdstSuperObject* parent, HIE_tdstSuperObject** array, int* index, unsigned long ulTypes);

AI_tdstAIModel* fn_p_stAllocAIModel();

HIE_tdstEngineObject* fn_p_stAllocateAlwaysEngineObject(tdObjectType otObjectFamilyType, tdObjectType otObjectModelType, tdObjectType otObjectPersonalType);

void fn_vAddAnAlwaysModel(HIE_tdstEngineObject* p_stAlwaysObject);

#ifdef __cplusplus
}
#endif