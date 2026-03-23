#include "cpa_functions.h"

SCT_tdstSectInfo* (*SCT_fn_vSectInfoAlloc) (HIE_tdstEngineObject* engineObject) = OFFSET(0x4122C0);
void (*fn_v3dDataInitValueSAI) (GAM_tdst3dData* h_3dData) = OFFSET(0x4186B0);
void (*fn_v3dDataInit) (HIE_tdstEngineObject* p_stObject, AI_tdeObjectTreeInit eObjectInit) = OFFSET(0x418340);
char* (*fnp_vGameMallocInHLM) (unsigned int size) = OFFSET(0x4077E0);
void (*PLA_fn_vUpdateTransparencyForModules) (HIE_tdstSuperObject* superObject) = OFFSET(0x40F260);
void (*fn_vKillEngineObjectOrAlwaysByPointer)(HIE_tdstEngineObject* p_stObject) = OFFSET(0x4062a0);


/*void* fnp_vGameMallocInHLM(unsigned long ulSize) {
	void* ptr = malloc(ulSize);
	memset(ptr, 0, ulSize);

	return ptr;
}*/

HIE_tdstSuperObject** fn_aGetChildObjects(HIE_tdstSuperObject* parent, HIE_tdstSuperObject** array, int* index, unsigned long ulType) {
	if (!parent || !array) return array;

	HIE_tdstSuperObject* child;
	LST_M_DynamicForEach(parent, child) {
		if (child->ulType == ulType) {
			array[(*index)++] = child;
		}
		fn_aGetChildObjects(child, array, index, ulType);
	}

	return array;
}

HIE_tdstFamilyList* fn_hFindFamily(tdObjectType otFamilyType)
{
	struct HIE_tdstFamilyList* p_stThisFamily;
	long i;

	LST_M_DynamicForEachIndex(&GAM_g_stEngineStructure->hFamilyList, p_stThisFamily, i)
	{
		if (p_stThisFamily->lObjectFamilyType == otFamilyType)
			return(p_stThisFamily);
	}
	return(NULL);
}

HIE_tdstEngineObject * fn_p_stAllocateAlwaysEngineObject(tdObjectType otObjectFamilyType, tdObjectType otObjectModelType, tdObjectType otObjectPersonalType)
{
	HIE_tdstEngineObject* p_stTempObject;
	p_stTempObject = (HIE_tdstEngineObject*)fnp_vGameMallocInHLM(sizeof(HIE_tdstEngineObject));

	GAM_fn_vStdGameAlloc(p_stTempObject);
	GAM_fn_v3dDataAlloc(p_stTempObject);
	//AI_fn_vBrainAlloc(p_stTempObject); // Because the game does not allocate a lot of space for always minds/dsgmem by default, we allocate our own space
  p_stTempObject->hBrain = (AI_tdstBrain*)fnp_vGameMallocInHLM(sizeof(AI_tdstBrain));
  p_stTempObject->hBrain->p_stMind = (AI_tdstMind*)fnp_vGameMallocInHLM(sizeof(AI_tdstMind));
  p_stTempObject->hBrain->p_stMind->p_stDsgMem = (AI_tdstMind*)fnp_vGameMallocInHLM(sizeof(AI_tdstDsgMem));
	p_stTempObject->hBrain->p_stMind->p_stDsgMem->p_cDsgMemBuffer = fnp_vGameMallocInHLM(32 * 1024); // 32 kB should be plenty
  p_stTempObject->hBrain->p_stMind->p_stDsgMem->p_cDsgMemBufferInit = fnp_vGameMallocInHLM(32 * 1024); // 32 kB should be plenty
  AI_tdstDsgVar* dsgVar = (AI_tdstDsgVar*)fnp_vGameMallocInHLM(sizeof(AI_tdstDsgVar));
  AI_tdstDsgVar** dsgVarArray = (AI_tdstDsgVar**)fnp_vGameMallocInHLM(sizeof(AI_tdstDsgVar*));
	dsgVarArray[0] = dsgVar;
	p_stTempObject->hBrain->p_stMind->p_stDsgMem->pp_stDsgVar = dsgVarArray;

	COL_fn_vCollSetAlloc(p_stTempObject);
	SCT_fn_vSectInfoAlloc(p_stTempObject);
	DNM_fn_vDynamAlloc(p_stTempObject);

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

void fn_vRemoveAnAlwaysModel(HIE_tdstEngineObject* p_stAlwaysObject)
{
	if (p_stAlwaysObject == NULL || ALW_g_stAlways == NULL) return;

	ALW_tdstAlwaysModelList* h_Current;
	ALW_tdstAlwaysModelList* h_Next;

	h_Current = (ALW_tdstAlwaysModelList*)LST_M_DynamicGetFirstElement(&(ALW_g_stAlways->hLstAlwaysModel));

	while (h_Current != NULL)
	{
		// Save next pointer because LST_M_DynamicIsolate/Free will invalidate current
		h_Next = (ALW_tdstAlwaysModelList*)LST_M_DynamicGetNextBrother(h_Current);

		if (h_Current->p_stAlwaysObject == p_stAlwaysObject)
		{
			LST_M_DynamicIsolate(h_Current);

			if (p_stAlwaysObject->hStandardGame)
			{
				p_stAlwaysObject->hStandardGame->ucMiscFlags &= ~Std_C_MiscFlag_Always;
			}

			// TODO: free memory

			return;
		}

		h_Current = h_Next;
	}
}