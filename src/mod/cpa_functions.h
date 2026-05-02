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

#define HIE_CONCAT_INNER(a, b) a ## b
#define HIE_CONCAT(a, b) HIE_CONCAT_INNER(a, b)

#define HIE_M_ForEachActor(actorVar) \
    HIE_tdstSuperObject* HIE_CONCAT(_act, __LINE__)[MAX_ACTORS]; \
    int HIE_CONCAT(_idx, __LINE__) = 0; \
    fn_aGetChildObjects(*GAM_g_p_stDynamicWorld, HIE_CONCAT(_act, __LINE__), &HIE_CONCAT(_idx, __LINE__), HIE_C_Type_Actor); \
    fn_aGetChildObjects(*GAM_g_p_stInactiveDynamicWorld, HIE_CONCAT(_act, __LINE__), &HIE_CONCAT(_idx, __LINE__), HIE_C_Type_Actor); \
    for (int HIE_CONCAT(_i, __LINE__) = 0; HIE_CONCAT(_i, __LINE__) < HIE_CONCAT(_idx, __LINE__) && HIE_CONCAT(_act, __LINE__)[HIE_CONCAT(_i, __LINE__)] != NULL; HIE_CONCAT(_i, __LINE__)++) \
        for (HIE_tdstSuperObject* actorVar = HIE_CONCAT(_act, __LINE__)[HIE_CONCAT(_i, __LINE__)]; actorVar; actorVar = NULL)

#define HIE_M_ForEachSector(sectorVar) \
    HIE_tdstSuperObject* HIE_CONCAT(_sec, __LINE__)[MAX_SECTORS]; \
    int HIE_CONCAT(_sIdx, __LINE__) = 0; \
    fn_aGetChildObjects(*GAM_g_p_stFatherSector, HIE_CONCAT(_sec, __LINE__), &HIE_CONCAT(_sIdx, __LINE__), HIE_C_Type_Sector); \
    for (int HIE_CONCAT(_si, __LINE__) = 0; \
         HIE_CONCAT(_si, __LINE__) < HIE_CONCAT(_sIdx, __LINE__) && HIE_CONCAT(_sec, __LINE__)[HIE_CONCAT(_si, __LINE__)] != NULL; \
         HIE_CONCAT(_si, __LINE__)++) \
        for (HIE_tdstSuperObject* sectorVar = HIE_CONCAT(_sec, __LINE__)[HIE_CONCAT(_si, __LINE__)]; \
             sectorVar; \
             sectorVar = NULL)

// Function pointers //

extern SCT_tdstSectInfo* (*fn_vSectInfoAlloc) (HIE_tdstEngineObject* engineObject);
extern void (*fn_v3dDataInitValueSAI) (GAM_tdst3dData* h_3dData);
extern void (*fn_v3dDataInit) (HIE_tdstEngineObject* p_stObject, AI_tdeObjectTreeInit eObjectInit);
extern char* (*fnp_vGameMallocInHLM) (unsigned int size);
extern void (*PLA_fn_vUpdateTransparencyForModules) (HIE_tdstSuperObject* superObject);
extern void (*fn_vKillEngineObjectOrAlwaysByPointer)(HIE_tdstEngineObject* p_stObject);

// End function pointers //

HIE_tdstFamilyList* fn_hFindFamily(tdObjectType otFamilyType);
HIE_tdstSuperObject** fn_aGetChildObjects(HIE_tdstSuperObject* parent, HIE_tdstSuperObject** array, int* index, unsigned long ulTypes);

HIE_tdstEngineObject* fn_p_stAllocateAlwaysEngineObject(tdObjectType otObjectFamilyType, tdObjectType otObjectModelType, tdObjectType otObjectPersonalType);

void fn_vAddAnAlwaysModel(HIE_tdstEngineObject* p_stAlwaysObject);
void fn_vRemoveAnAlwaysModel(HIE_tdstEngineObject* p_stAlwaysObject);

#ifdef __cplusplus
}
#endif