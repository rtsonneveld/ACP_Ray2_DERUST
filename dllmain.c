#include "framework.h"
#include "cpa_functions.h"
#define VERSION "0.0.1"

HIE_tdstSuperObject* spo_rayman = NULL;

void CALLBACK MyTextCallback(SPTXT_tdstTextInfo* p_stString)
{
	p_stString->X = 30;
	p_stString->Y = 30;
	p_stString->xSize = 10.0f;
	p_stString->ucAlpha = 255;
	p_stString->bFrame = TRUE;

	SPTXT_vPrintFmtLine("DERUST v" VERSION);
}

void CreateObject(MTH3D_tdstVector* position, tdObjectType modelType)
{

	POS_tdstCompletePosition stMatrix;
	POS_fn_vSetIdentityMatrix(&stMatrix);
	POS_fn_vSetTranslationVector(&stMatrix, position);

	HIE_tdstSuperObject* p_SuperObjPersoGenerated = fn_p_stAllocateAlways(
		modelType, 
		*GAM_g_p_stDynamicWorld, 
		HIE_M_hGetMainActor(), 
		0,
		&stMatrix);
}

HIE_tdstEngineObject* myOwnSpecialEngineObject;
long myOwnSpecialObjectType = 69420;

void MOD_fn_vEngine()
{
	GAM_fn_vEngine();

	if (spo_rayman == NULL)
		spo_rayman = HIE_fn_p_stFindObjectByName("rayman");

	if (IPT_M_bActionJustValidated(IPT_E_Entry_Action_TransePolochus))
	{
		myOwnSpecialEngineObject = fn_p_stAllocateAlwaysEngineObject(
			spo_rayman->hLinkedObject.p_stActor->hStandardGame->lObjectFamilyType, 
			myOwnSpecialObjectType,
			C_AlwaysObjectType);

		myOwnSpecialEngineObject->hBrain->p_stMind->p_stAIModel = fn_p_stAllocAIModel();
		fn_vSectInfoAlloc(myOwnSpecialEngineObject);

		// Set the object table to the family's default

		myOwnSpecialEngineObject->h3dData->h_CurrentObjectsTable =
			myOwnSpecialEngineObject->h3dData->h_InitialObjectsTable = myOwnSpecialEngineObject->h3dData->h_Family->hDefaultObjectsTable;
		myOwnSpecialEngineObject->h3dData->h_InitialState = myOwnSpecialEngineObject->h3dData->h_Family->stForStateArray.hFirstElementSta;
		myOwnSpecialEngineObject->h3dData->ulNumberOfChannels = myOwnSpecialEngineObject->h3dData->h_Family->ulNumberOfChannels;
	}

	if (IPT_M_bActionJustValidated(IPT_E_Entry_Action_Nage_Plonger))
	{
		fn_vAddAnAlwaysModel(myOwnSpecialEngineObject);
		//fn_vAddAnAlwaysModel(spo_rayman->hLinkedObject.p_stActor);
	}

	if (IPT_M_bActionJustValidated(IPT_E_Entry_Action_Affiche_Jauge))
	{
		CreateObject(&spo_rayman->p_stGlobalMatrix->stPos, myOwnSpecialObjectType);
		//CreateObject(&spo_rayman->p_stGlobalMatrix->stPos, spo_rayman->hLinkedObject.p_stActor->hStandardGame->lObjectPersonalType);
	}
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD dwReason, LPVOID lpReserved )
{
	switch ( dwReason )
	{
		case DLL_PROCESS_ATTACH: /* create function hooks here */
			FHK_fn_lCreateHook((void**)&GAM_fn_vEngine, (void*)MOD_fn_vEngine);

			SPTXT_vInit();
			SPTXT_vAddTextCallback(MyTextCallback);

			break;

		case DLL_PROCESS_DETACH: /* destroy function hooks here*/
			
			FHK_fn_lDestroyHook((void**)&GAM_fn_vEngine, (void*)MOD_fn_vEngine);
			SPTXT_vDeInit();

			break;

		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
	}
	return TRUE;
}
