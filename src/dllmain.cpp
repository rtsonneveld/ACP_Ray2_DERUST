#include <Windows.h>
#include "cpa_functions.h"
#include "util.h"
#include "state.h"
#include "ui.h"
#include "derust.h"

HIE_tdstSuperObject* g_DR_rayman = nullptr;
HIE_tdstSuperObject* g_DR_selectedObject = nullptr;

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
	
	if (alw_rayman != nullptr) return;
	if (g_DR_rayman == nullptr) return;

	alw_rayman = fn_p_stAllocateAlwaysEngineObject(
		g_DR_rayman->hLinkedObject.p_stActor->hStandardGame->lObjectFamilyType,
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

// DR_RemoveLoadScreens taken from: https://github.com/spitfirex86/LoadScreenRemoveR2/blob/master/dllmain.c

void DR_RemoveLoadScreens() {

	#define C_SegCodePtr	((void*)0x401000)
	#define C_SegCodeSize	0x9b000
	/* disable load screens */

	DWORD op, np = PAGE_EXECUTE_READWRITE;
	VirtualProtect(C_SegCodePtr, C_SegCodeSize, np, &op);

	*(unsigned char*)0x45F7E0 = 0xC3;
	*(unsigned char*)0x45F530 = 0xC3;
	*(unsigned char*)0x45EED0 = 0xC3;
	*(unsigned char*)0x45EDF0 = 0xC3;
	*(unsigned char*)0x45EE10 = 0xC3;

	VirtualProtect(C_SegCodePtr, C_SegCodeSize, op, &np);
}

int timer = 0;

LRESULT MOD_fn_WndProc(HANDLE hWnd, unsigned int uMsg, unsigned int wParam, long lParam) {

	LRESULT lResult = GAM_fn_WndProc(hWnd, uMsg, wParam, lParam);
	switch (uMsg) {
		case WM_SHOWWINDOW:
		if (wParam)
			ShowCursor(TRUE);
		break;

		case WM_ACTIVATE:
		if (wParam > 0)
			ShowCursor(TRUE);
		break;
	}

	return lResult;
}

void MOD_fn_vEngine()
{
	if (DR_UI_Init((HWND)GAM_fn_hGetWindowHandle()) != 0) {
		MessageBox(nullptr, L"IMGUI Failed to initialize", L"Error!", MB_OK | MB_ICONERROR);
		exit(1);
	}
	DR_UI_Update();

	GAM_fn_vEngine();

	if (g_DR_Playback.pause) {

		g_DR_Playback.pause = FALSE;

		GAM_g_stEngineStructure->bEngineIsInPaused = TRUE;
		g_DR_Playback.lastFrame = *HIE_gs_lCurrentFrame;
		GAM_fn_vSaveEngineClock();
	}
	if (g_DR_Playback.unpause) {

		g_DR_Playback.unpause = FALSE;

		GAM_g_stEngineStructure->bEngineIsInPaused = FALSE;
		*HIE_gs_lCurrentFrame = g_DR_Playback.lastFrame;
		GAM_fn_vLoadEngineClock();

		if (g_DR_Playback.framestep) {
			g_DR_Playback.pause = TRUE;
			g_DR_Playback.framestep = FALSE;
		}
	}

	if (g_DR_rayman == NULL)
		g_DR_rayman = HIE_fn_p_stFindObjectByName("rayman");

	CreateAlwaysRaymanObject();

	if (IPT_M_bActionJustValidated(IPT_E_Entry_Action_Nage_Plonger) && FALSE)
	{
		spawned_rayman = CreateObject(&g_DR_rayman->p_stGlobalMatrix->stPos, alwaysRaymanObjectType);
	}

	if (spawned_rayman != NULL) {

		if (SPO_Actor(spawned_rayman)->h3dData != NULL && SPO_Actor(spawned_rayman)->h3dData->h_CurrentState != SPO_Actor(g_DR_rayman)->h3dData->h_CurrentState) {
			PLA_fn_bSetNewState(spawned_rayman, SPO_Actor(g_DR_rayman)->h3dData->h_CurrentState, TRUE, FALSE);

			SPO_SetTransparency(spawned_rayman, 0.5f);
		}
	}
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD dwReason, LPVOID lpReserved )
{
	switch ( dwReason )
	{
		case DLL_PROCESS_ATTACH:

			FHK_fn_lCreateHook((void**)&GAM_fn_WndProc, (void*)MOD_fn_WndProc);
			FHK_fn_lCreateHook((void**)&GAM_fn_vEngine, (void*)MOD_fn_vEngine);

			DR_RemoveLoadScreens();

			SPTXT_vInit();

			break;

		case DLL_PROCESS_DETACH: 
			
			FHK_fn_lDestroyHook((void**)&GAM_fn_vEngine, (void*)MOD_fn_vEngine);
			SPTXT_vDeInit();
			DR_UI_DeInit();

			break;

		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:

			break;
	}
	return TRUE;
}
