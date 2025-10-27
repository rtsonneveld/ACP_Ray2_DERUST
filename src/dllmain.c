// All files inside mod are C
// All files inside UI are C++

#include <Windows.h>
#include <stdio.h>
#include "mod/cpa_functions.h"
#include "mod/util.h"
#include "mod/state.h"
#include "mod/savestates.h"
#include "mod/globals.h"
#include "mod/cheats.h"
#include <ACP_Ray2.h>
#include "ui/ui_bridge.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Global module handle
HMODULE g_hModule;

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
	if (g_DR_rayman == NULL) return;

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


void MOD_fn_vChooseTheGoodDesInit() {
	if (GAM_g_stEngineStructure->eEngineMode == E_EM_ModeChangeLevel) {
		g_DR_selectedObject = NULL;
	}

	DR_UI_OnMapExit();

	GAM_fn_vChooseTheGoodDesInit();
}

LONG LogExceptionFilter(PEXCEPTION_POINTERS ep) {
	if (!ep || !ep->ExceptionRecord) {
		OutputDebugStringA("Filter: missing ep\n");
		return EXCEPTION_EXECUTE_HANDLER;
	}

	char buf[256];
	_snprintf_s(buf, sizeof(buf), _TRUNCATE,
		"Filter: code=0x%08X addr=%p params=%u\n",
		(unsigned)ep->ExceptionRecord->ExceptionCode,
		ep->ExceptionRecord->ExceptionAddress,
		(unsigned)ep->ExceptionRecord->NumberParameters);

	OutputDebugStringA(buf);
	return EXCEPTION_EXECUTE_HANDLER;
}


void MOD_fn_vEngine()
{
	if (DR_UI_Init((HWND)GAM_fn_hGetWindowHandle(), g_hModule) != 0) {
		MessageBox(NULL, L"IMGUI Failed to initialize", L"Error!", MB_OK | MB_ICONERROR);
		exit(1);
	}

	DR_UI_Update();
	DR_Cheats_Apply();

	__try {
		GAM_fn_vEngine();
	}
	__except (LogExceptionFilter(GetExceptionInformation())) {
		__debugbreak();

		g_DR_Playback.pause = TRUE;
	}

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

	if (g_DR_rayman == NULL) {
		g_DR_rayman = HIE_fn_p_stFindObjectByName("rayman");
#if _DEBUG
		g_DR_selectedObject = g_DR_rayman;
#endif
	}

	if (g_DR_global == NULL) {
		g_DR_global = HIE_fn_p_stFindObjectByName("global");
	}

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

void CALLBACK VersionDisplay(SPTXT_tdstTextInfo* p_stString) {
	SPTXT_vPrintFmtLine("DERUST %s - Press TAB", DERUST_VERSION);
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD dwReason, LPVOID lpReserved )
{
	switch ( dwReason )
	{
		case DLL_PROCESS_ATTACH:

			g_hModule = hModule;

			AllocConsole();
			FILE* f;
			freopen_s(&f, "CONOUT$", "w", stdout);
			freopen_s(&f, "CONOUT$", "w", stderr);

			FHK_fn_lCreateHook((void**)&GAM_fn_WndProc, (void*)MOD_fn_WndProc);
			FHK_fn_lCreateHook((void**)&GAM_fn_vEngine, (void*)MOD_fn_vEngine);
			FHK_fn_lCreateHook((void**)&GAM_fn_vChooseTheGoodDesInit, (void*)MOD_fn_vChooseTheGoodDesInit);
			FHK_fn_lCreateHook((void**)&fn_p_vDynAlloc, (void*)MOD_fn_vDynAlloc);
			FHK_fn_lCreateHook((void**)&fn_p_vGenAlloc, (void*)MOD_fn_vGenAlloc);
			FHK_fn_lCreateHook((void**)&Mmg_fn_vInitSpecificBlock, (void*)MOD_fn_vInitSpecificBlock);
			FHK_fn_lCreateHook((void**)&Mmg_fn_v_InitMmg, (void*)MOD_fn_v_InitMmg);
			FHK_fn_lCreateHook((size_t**)&SNA_fn_ulFRead, (size_t)MOD_fn_ulFRead);

			DR_RemoveLoadScreens();

			SPTXT_vInit();
			SPTXT_vAddTextCallback(VersionDisplay);

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
