#include "framework.h"
#include "cpa_functions.h"
#include "util.h"
#define VERSION "0.0.1"

HIE_tdstSuperObject* rayman_spo = NULL;

void CALLBACK MyTextCallback(SPTXT_tdstTextInfo* p_stString)
{
	p_stString->X = 30;
	p_stString->Y = 30;
	p_stString->xSize = 10.0f;
	p_stString->ucAlpha = 255;
	p_stString->bFrame = TRUE;

	SPTXT_vPrintFmtLine("DERUST v" VERSION);
}

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

HIE_tdstEngineObject* always_rayman_obj;
long alwaysRaymanObjectType = 1000;
HIE_tdstSuperObject* spawned_rayman;

void CreateAlwaysRaymanObject() {
	
	if (always_rayman_obj != NULL) return;
	if (rayman_spo == NULL) return;

	always_rayman_obj = fn_p_stAllocateAlwaysEngineObject(
		rayman_spo->hLinkedObject.p_stActor->hStandardGame->lObjectFamilyType,
		alwaysRaymanObjectType,
		C_AlwaysObjectType);

	always_rayman_obj->hBrain->p_stMind->p_stAIModel = fn_p_stAllocAIModel();
	fn_vSectInfoAlloc(always_rayman_obj);

	// Set the object table to the family's default

	always_rayman_obj->h3dData->h_CurrentObjectsTable =
		always_rayman_obj->h3dData->h_InitialObjectsTable = always_rayman_obj->h3dData->h_Family->hDefaultObjectsTable;
	always_rayman_obj->h3dData->h_InitialState = always_rayman_obj->h3dData->h_Family->stForStateArray.hFirstElementSta;
	always_rayman_obj->h3dData->ulNumberOfChannels = always_rayman_obj->h3dData->h_Family->ulNumberOfChannels;

	fn_vAddAnAlwaysModel(always_rayman_obj);
}

void MOD_fn_vEngine()
{
	GAM_fn_vEngine();

	if (rayman_spo == NULL)
		rayman_spo = HIE_fn_p_stFindObjectByName("rayman");

	CreateAlwaysRaymanObject();

	if (IPT_M_bActionJustValidated(IPT_E_Entry_Action_Nage_Plonger))
	{
		spawned_rayman = CreateObject(&rayman_spo->p_stGlobalMatrix->stPos, alwaysRaymanObjectType);
	}

	if (spawned_rayman != NULL) {
		if (SPO_Actor(spawned_rayman)->h3dData->h_CurrentState != rayman_spo->hLinkedObject.p_stActor->h3dData->h_CurrentState) {
			PLA_fn_bSetNewState(spawned_rayman, rayman_spo->hLinkedObject.p_stActor->h3dData->h_CurrentState, TRUE, FALSE);

			SPO_SetTransparency(spawned_rayman, 0.5f);
		}
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
