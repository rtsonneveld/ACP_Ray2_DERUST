#include "cpa_functions.h"

SCT_tdstSectInfo* (*fn_vSectInfoAlloc) (HIE_tdstEngineObject* engineObject) = OFFSET(0x4122C0);
void (*fn_v3dDataInitValueSAI) (GAM_tdst3dData* h_3dData) = OFFSET(0x4186B0);
void (*fn_v3dDataInit) (HIE_tdstEngineObject* p_stObject, AI_tdeObjectTreeInit eObjectInit) = OFFSET(0x418340);
char* (*AI_fn_p_vTrueAlloc) (unsigned int size) = OFFSET(0x466860);
char* (*fnp_vGameMallocInHLM) (unsigned int size) = OFFSET(0x4077E0);
void (*PLA_fn_vUpdateTransparencyForModules) (HIE_tdstSuperObject* superObject) = OFFSET(0x40F260);
BOOL(*PLA_fn_bSetNewState)(HIE_tdstSuperObject* p_stSuperObject, HIE_tdstState* h_WantedState, BOOL _bForce, BOOL _bHandleSkippedEventsIfRelevant) = OFFSET(0x40FAA0);

HIE_tdstSuperObject* (*fn_p_stAllocateAlways) (long otObjectModelType,
	HIE_tdstSuperObject* p_stFatherSuperObject,
	HIE_tdstSuperObject* _hGenerator,
	unsigned short uwAction,
	POS_tdstCompletePosition* p_stMatrix) = OFFSET(0x40BCC0);


/*void* fnp_vGameMallocInHLM(unsigned long ulSize) {
	void* ptr = malloc(ulSize);
	memset(ptr, 0, ulSize);

	return ptr;
}*/

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
	GAM_fn_v3dDataAlloc(p_stTempObject);
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