#include "framework.h"
#include "cpa_functions.h"
#include "util.h"
#include "features/features.h"
#include "derust.h"

HIE_tdstSuperObject* rayman = NULL;

HIE_tdstSuperObject* CreateObject(MTH3D_tdstVector* position, tdObjectType modelType)
{

	POS_tdstCompletePosition stMatrix;
	POS_fn_vSetIdentityMatrix(&stMatrix);
	POS_fn_vSetTranslationVector(&stMatrix, position);

	return fn_p_stAllocateAlways(
		modelType, 
		*GAM_g_p_stDynamicWorld, 
		HIE_M_hGetMainActor(), 
		0,
		&stMatrix);
}

HIE_tdstEngineObject* alw_rayman;
long alwaysRaymanObjectType = 1000;
HIE_tdstSuperObject* spawned_rayman;

void CreateAlwaysRaymanObject() {
	
	if (alw_rayman != NULL) return;
	if (rayman == NULL) return;

	alw_rayman = fn_p_stAllocateAlwaysEngineObject(
		rayman->hLinkedObject.p_stActor->hStandardGame->lObjectFamilyType,
		alwaysRaymanObjectType,
		C_AlwaysObjectType);

	alw_rayman->hBrain->p_stMind->p_stAIModel = fn_p_stAllocAIModel();
	
	fn_vSectInfoAlloc(alw_rayman);
	
	alw_rayman->h3dData->h_CurrentObjectsTable =
		alw_rayman->h3dData->h_InitialObjectsTable = alw_rayman->h3dData->h_Family->hDefaultObjectsTable;
	alw_rayman->h3dData->h_InitialState = alw_rayman->h3dData->h_Family->stForStateArray.hFirstElementSta;
	alw_rayman->h3dData->ulNumberOfChannels = alw_rayman->h3dData->h_Family->ulNumberOfChannels;

	fn_vAddAnAlwaysModel(alw_rayman);
}

void MOD_fn_vEngine()
{
	GAM_fn_vEngine();

	if (rayman == NULL)
		rayman = HIE_fn_p_stFindObjectByName("rayman");

	CreateAlwaysRaymanObject();

	if (IPT_M_bActionJustValidated(IPT_E_Entry_Action_Nage_Plonger) && FALSE)
	{
		spawned_rayman = CreateObject(&rayman->p_stGlobalMatrix->stPos, alwaysRaymanObjectType);
	}

	if (spawned_rayman != NULL) {

		if (SPO_Actor(spawned_rayman)->h3dData != NULL && SPO_Actor(spawned_rayman)->h3dData->h_CurrentState != SPO_Actor(rayman)->h3dData->h_CurrentState) {
			PLA_fn_bSetNewState(spawned_rayman, SPO_Actor(rayman)->h3dData->h_CurrentState, TRUE, FALSE);

			SPO_SetTransparency(spawned_rayman, 0.5f);
		}
	}

	DR_Features_Update();
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD dwReason, LPVOID lpReserved )
{
	switch ( dwReason )
	{
		case DLL_PROCESS_ATTACH: /* create function hooks here */
			FHK_fn_lCreateHook((void**)&GAM_fn_vEngine, (void*)MOD_fn_vEngine);

			SPTXT_vInit();

			DR_Features_Init();

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
