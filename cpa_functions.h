#pragma once
#include <apidef.h>
#include <HIE/HIE.h>
#include <POS/POS.h>
#include "_3dDataUtil.h"
#include <assert.h>

#define Std_C_MiscFlag_Always 0x10
#define MTH3D_M_vNullVector( VectDest)    { (VectDest)->x=(VectDest)->y=(VectDest)->z=0.0f; }

//void* (*fnp_vGameMallocInHLM) (unsigned long ulSize) = OFFSET(0x4077E0);

SCT_tdstSectInfo* (*fn_vSectInfoAlloc) (HIE_tdstEngineObject* engineObject) = OFFSET(0x4122C0);
void (*fn_v3dDataInitValueSAI) (GAM_tdst3dData* h_3dData) = OFFSET(0x4186B0);
void (*fn_v3dDataInit) (HIE_tdstEngineObject* p_stObject, AI_tdeObjectTreeInit eObjectInit) = OFFSET(0x418340);
char* (*AI_fn_p_vTrueAlloc) (unsigned int size) = OFFSET(0x466860);

void* fnp_vGameMallocInHLM(unsigned long ulSize) {
	void* ptr = malloc(ulSize);
	memset(ptr, 0, ulSize);

	return ptr;
}

typedef long tdObjectType;
#define C_InvalidObjectType -1
#define C_AlwaysObjectType  0x00010000

HIE_tdstFamilyList* fn_hFindFamily(tdObjectType otFamilyType)
{
	struct HIE_tdstFamilyList* p_stThisFamily;
	long i;

	LST_M_DynamicForEach(&GAM_g_stEngineStructure->hFamilyList, p_stThisFamily, i)
	{
		if (p_stThisFamily->lObjectFamilyType == otFamilyType)
			return(p_stThisFamily);
	}
	return(NULL);
}

long fn_ul3dDataSizeOf() {
	return sizeof(GAM_tdst3dData);
}

GAM_tdst3dData * MOD_GAM_fn_h_3dDataRealAlloc() {

	GAM_tdst3dData* h_3dData;

	h_3dData = (GAM_tdst3dData*)fnp_vGameMallocInHLM(fn_ul3dDataSizeOf());
	POS_fn_vSetIdentityMatrix(fn_p_st3dDataGetMatrix(h_3dData));

	fn_v3dDataSetFlagEndState(h_3dData, FALSE);
	fn_v3dDataSetFlagEndOfAnim(h_3dData, FALSE);
	fn_v3dDataSetFlagModifState(h_3dData, FALSE);
	fn_v3dDataSetForcedFrame(h_3dData, 0);

	(&h_3dData->stFrame3d)->p_stArrayOfElts3d = NULL;
	fn_v3dDataSetSizeOfArrayOfElts3d(h_3dData, 0);
	h_3dData->hMorphChannelList = NULL;

	fn_v3dDataSetStateInLastFrame(h_3dData, NULL);
	fn_v3dDataSetWantedState(h_3dData, NULL);
	fn_v3dDataSetCurrentState(h_3dData, NULL);

	fn_v3dDataSetCurrentObjectsTable(h_3dData, NULL);
	fn_v3dDataSetInitialObjectsTable(h_3dData, NULL);

	h_3dData->ulStartTime = GAM_g_stEngineStructure->stEngineTimer.ulCurrentTimerCount;
	h_3dData->bSkipCurrentFrame = FALSE;
	h_3dData->ulTimeDelay = 0;
	h_3dData->sLastFrame = -1;
	h_3dData->bStateJustModified = FALSE;
	h_3dData->uwNbEngineFrameSinceLastMechEvent = 65535; //LME_INVALID;
	h_3dData->ucFrameRate = 0;

	h_3dData->lDrawMask = GLI_C_lAllIsEnable;
	h_3dData->lDrawMaskInit = GLI_C_lAllIsEnable;

	fn_v3dDataSetShadowScaleX(h_3dData, 1.0f);
	fn_v3dDataSetShadowScaleY(h_3dData, 1.0f);

	fn_v3dDataSetShadowQuality(h_3dData, 2);
	fn_v3dDataSetShadowTexture(h_3dData, NULL);
	fn_v3dDataSetShadowMaterial(h_3dData, NULL);

	MTH3D_M_vNullVector(fn_p_st3dDataGetSHWDeformationVector(h_3dData));
	*(&(h_3dData->xSHWHeight)) = 0.2f;

	h_3dData->ucBrainComputationFrequency = 1;
	h_3dData->cBrainCounter = -1;

	h_3dData->uwBrainMainCounter = 1; // (unsigned short)RND_fn_lGetLongRandomValue(0, 1, (long)17);
	h_3dData->uwBrainMainCounter = 1;
	h_3dData->ucLightComputationFrequency = 1;
	h_3dData->cLightCounter = -1;

	h_3dData->ucTransparency = 255;

	return(h_3dData);
}

// TODO: figure out why this function doesn't work when hooked
void MOD_GAM_fn_v3dDataAlloc(HIE_tdstEngineObject* p_stObject)
{
	assert(p_stObject != NULL);
	assert(p_stObject->h3dData == NULL);

	p_stObject->h3dData = MOD_GAM_fn_h_3dDataRealAlloc();
	fn_v3dDataInitValueSAI(p_stObject->h3dData);
}


AI_tdstAIModel* fn_p_stAllocAIModel()
{
	AI_tdstAIModel* p_stAIModel = NULL;
	p_stAIModel = (AI_tdstAIModel*)AI_fn_p_vTrueAlloc(sizeof(AI_tdstAIModel));
	p_stAIModel->a_stScriptAIIntel = NULL;
	p_stAIModel->a_stScriptAIReflex = NULL;
	p_stAIModel->p_stDsgVar = NULL;
	p_stAIModel->p_stListOfMacro = NULL;
	p_stAIModel->ucSecondPassDone = 0;

	return(p_stAIModel);
}

struct tdstEngineObject_* fn_p_stAllocateAlwaysEngineObject(tdObjectType otObjectFamilyType, tdObjectType otObjectModelType, tdObjectType otObjectPersonalType)
{
	HIE_tdstEngineObject* p_stTempObject;
	p_stTempObject = (HIE_tdstEngineObject*)fnp_vGameMallocInHLM(sizeof(HIE_tdstEngineObject));

	GAM_fn_vStdGameAlloc(p_stTempObject);
	MOD_GAM_fn_v3dDataAlloc(p_stTempObject);
	AI_fn_vBrainAlloc(p_stTempObject);

	p_stTempObject->hStandardGame->lObjectFamilyType = otObjectFamilyType;
	p_stTempObject->hStandardGame->lObjectModelType = otObjectModelType;
	p_stTempObject->hStandardGame->lObjectPersonalType = otObjectPersonalType;

	if (otObjectFamilyType != C_InvalidObjectType) {
		p_stTempObject->h3dData->h_Family = fn_hFindFamily(otObjectFamilyType);
	}

	return p_stTempObject;
}

void fn_vAddAnAlwaysModel(HIE_tdstEngineObject* p_stAlwaysObject)
{
	if (p_stAlwaysObject != NULL)
	{
		ALW_tdstAlwaysModelList* h_NewAlwaysModel;
		h_NewAlwaysModel = (ALW_tdstAlwaysModelList*)fnp_vGameMallocInHLM(sizeof(ALW_tdstAlwaysModelList));
		h_NewAlwaysModel->p_stAlwaysObject = p_stAlwaysObject;
		h_NewAlwaysModel->lObjectModelType = p_stAlwaysObject->hStandardGame->lObjectModelType;
		LST_M_DynamicAddTail(&(ALW_g_stAlways->hLstAlwaysModel), h_NewAlwaysModel);

		if (p_stAlwaysObject->hStandardGame)
			p_stAlwaysObject->hStandardGame->ucMiscFlags |= Std_C_MiscFlag_Always;
	}
}

HIE_tdstSuperObject* (*fn_p_stAllocateAlways) (long otObjectModelType,
                                               HIE_tdstSuperObject* p_stFatherSuperObject,
                                               HIE_tdstSuperObject* _hGenerator,
                                               unsigned short uwAction,
                                               POS_tdstCompletePosition* p_stMatrix) = OFFSET(0x40BCC0);
