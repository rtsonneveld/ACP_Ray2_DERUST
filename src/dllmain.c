// All files inside mod are C
// All files inside UI are C++
#define WIN32_LEAN_AND_MEAN

#define NOSERVICE
#define NOMCX
#define NOIME
#define NOHELP
#define NOCOMM
#define NOKANJI
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOSYSCOMMANDS
#define NORASTEROPS
#define NOICONS
#define NOKEYSTATES
#define NOSHOWWINDOW
#define NOCLIPBOARD

#include <Windows.h>
#include <stdio.h>
#include "mod/cpa_functions.h"
#include "mod/ai_distancechecks.h"
#include "mod/util.h"
#include "mod/state.h"
#include "mod/savestates.h"
#include "mod/globals.h"
#include "mod/cheats.h"
#include "mod/debugger.h"
#include <ACP_Ray2.h>
#include "ui/ui_bridge.h"
#include <time.h>
#include <ddraw.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad/glad.h>

// Global handles
HMODULE g_hModule;

// Thread
HANDLE g_hUIThread = NULL;
HANDLE g_hInitEvent = NULL;
HANDLE g_hFrameEvent = NULL;
HANDLE g_hAFrameIsWaiting = NULL;
HANDLE g_hFrameDoneCopying = NULL;
volatile BOOL g_bRunning = true;

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

HWND g_hParent = NULL;
LRESULT CALLBACK MOD_fn_WndProc(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_SHOWWINDOW:
			if (wParam)
				ShowCursor(TRUE);
			break;

		case WM_ACTIVATE:
			if (wParam > 0)
				ShowCursor(TRUE);
			break;
	}

	return GAM_fn_WndProc(hWnd, uMsg, wParam, lParam);
}

void MOD_fn_vDisplayAll(void) {
	(*GLI_DRV_xSendListToViewport_)(&GAM_g_stEngineStructure->stViewportAttr);
	if ( !GLD_bWriteToViewportFinished(GAM_g_stEngineStructure->hGLDDevice, GAM_g_stEngineStructure->hGLDViewport) )
		return;
	GAM_fn_vDisplayFix();
	//ReleaseSemaphore((HANDLE)GAM_g_stEngineStructure->hDrawSem, 1, 0);
	(*GLI_DRV_bEndScene_)();

	ReleaseSemaphore(g_hAFrameIsWaiting, 1, NULL);
	WaitForSingleObject(g_hFrameDoneCopying, 100);

	ReleaseSemaphore(GAM_g_stEngineStructure->hDrawSem, 1, NULL);
}

BOOL MOD_fn_bCreateMainDisplayScreen(void) {
	SetEvent(g_hInitEvent);
    return GAM_fn_bCreateMainDisplayScreen();
}

short MOD_INO_fn_wInit(HINSTANCE hInstance, HWND hWnd) {
	return INO_fn_wInit(hInstance, hWindow);
}

void MOD_fn_vInitGameLoop(void) {
	PostMessage(hWindow, WM_DR_INITWND, 0, 0);
	GAM_fn_vInitGameLoop();
}

void MOD_fn_vAskToChangeLevel(char const* szLevelName, ACP_tdxBool bSaveGame) {

	if (DR_Settings_Get_DisableAutoSave()) {
		return GAM_fn_vAskToChangeLevel(szLevelName, FALSE);
	}
	return GAM_fn_vAskToChangeLevel(szLevelName, bSaveGame);
}

void MOD_fn_vChooseTheGoodInit() {

	bool deadLoop = FALSE;

	if (GAM_g_stEngineStructure->eEngineMode == E_EM_ModeDeadLoop) {
		deadLoop = TRUE;
	}

	GAM_fn_vChooseTheGoodInit();

	if (deadLoop) {
		DR_Cheats_LoadPosition();
	}
}

void MOD_fn_vChooseTheGoodDesInit() {
	if (GAM_g_stEngineStructure->eEngineMode == E_EM_ModeChangeLevel) {
		g_DR_selectedObject = NULL;
		// Level changed?
		if (_stricmp(GAM_g_stEngineStructure->szLevelName, GAM_g_stEngineStructure->szNextLevelName) != 0) {
			DR_Cheats_ResetSavedPosition();
		}

		if (g_DR_Cheats_FreezeProgress) {
			DR_Cheats_LoadProgress();
		}

		DR_UI_OnMapExit();
	}
	DR_DistanceChecks_Reset();

	if (GAM_g_stEngineStructure->eEngineMode == E_EM_ModeStoppingProgram) {
		g_bRunning = FALSE;

		printf("Stopping program, wait for UI thread...\n");
		WaitForSingleObject(g_hUIThread, 1000);
		printf("Stopping program, close UI thread...\n");
		CloseHandle(g_hUIThread);
		printf("Stopping program, now choose the good desinit\n");
	}

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


DWORD WINAPI DR_UI_ThreadMain(LPVOID p)
{
	WaitForSingleObject(g_hInitEvent, INFINITE);

	if (DR_UI_Init(GAM_fn_hGetWindowHandle(), g_hModule) != 0) {
		MessageBox(NULL, L"IMGUI Failed to initialize", L"Error!", MB_OK | MB_ICONERROR);
		exit(1);
	}

	while (g_bRunning)
	{
		// Wait until the game loop signals a new frame
		WaitForSingleObject(g_hFrameEvent, 500);

		// Now process the UI update
		DR_UI_Update();
	}

	DR_UI_DeInit();

	return 0;
}

void MOD_fn_vEngine()
{
	DR_Cheats_Apply();
	if (!GAM_g_stEngineStructure->bEngineIsInPaused && !GAM_g_stEngineStructure->bEngineFrozen) {
		DR_DistanceChecks_Update();
	}
	
	int oldTickPerMs = GAM_g_stEngineStructure->stEngineTimer.ulTickPerMs;
	if (IPT_M_bActionIsValidated(IPT_E_Entry_DisplayRaster)) { // F10

		GAM_g_stEngineStructure->stEngineTimer.ulUsefulDeltaTime *= 4;
		GAM_g_stEngineStructure->stEngineTimer.ulTickPerMs /= 4;
	}

	if (DR_Settings_Get_TryCatchExceptions()) {
		__try {
			GAM_fn_vEngine();
		}
		__except (LogExceptionFilter(GetExceptionInformation())) {
			__debugbreak();

			g_DR_Playback.pause = TRUE;
			g_DR_debuggerEnabled = TRUE;
			g_DR_debuggerPaused = TRUE;

			DR_Debugger_SelectObjectAndComport(g_DR_debuggerContextSPO, g_DR_debuggerInstructionPtr);
		}
	}	else {
		GAM_fn_vEngine();
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

	GAM_g_stEngineStructure->stEngineTimer.ulTickPerMs = oldTickPerMs;

	SetEvent(g_hFrameEvent);
}

void CALLBACK VersionDisplay(SPTXT_tdstTextInfo* p_stString) {
	SPTXT_vPrintFmtLine("DERUST %s", DERUST_VERSION);
}

typedef BOOL(WINAPI* PFN_SetWindowPos)(
	HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);

PFN_SetWindowPos True_SetWindowPos = NULL;

BOOL WINAPI Hook_SetWindowPos(
	HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	if (hWnd == hWndR2)
	{
		RECT rc;
		GetClientRect(hWndR2, &rc);
		int width = rc.right - rc.left;
		int height = rc.bottom - rc.top;

		// Hide the Rayman 2 window by setting a 1x1 pixel region in the center
		int cx = (rc.right - rc.left) / 2;
		int cy = (rc.bottom - rc.top) / 2;

		HRGN rgn = CreateRectRgn(width/2, height/2, width/2 + 1, height/2 + 1); // 16x16 for test

		SetWindowRgn(hWndR2, rgn, TRUE);

		X = (-width / 2);
		Y = (-height / 2);

		uFlags &= ~SWP_NOMOVE;  // ensure move is applied
	}

	return True_SetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
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

			g_hInitEvent = CreateEvent(NULL, FALSE, FALSE, L"DRInitEvt");
			g_hFrameEvent = CreateEvent(NULL, FALSE, FALSE, L"DRFrameEvt");
			g_hAFrameIsWaiting = CreateSemaphoreA(NULL, 0, 1, "FRAMEWAITING");
			g_hFrameDoneCopying = CreateSemaphoreA(NULL, 0, 1, "FRAMECOPIED");

			g_hUIThread = CreateThread(
				NULL, 0,
				(LPTHREAD_START_ROUTINE)DR_UI_ThreadMain,
				NULL, 0, NULL
			);

			FHK_fn_lCreateHook((void**)&GAM_fn_WndProc, (void*)MOD_fn_WndProc);
			FHK_fn_lCreateHook((void**)&GAM_fn_vEngine, (void*)MOD_fn_vEngine);
			FHK_fn_lCreateHook((void**)&AI_fn_p_stEvalTree, (void*)MOD_fn_p_stEvalTree_Debugger);
			FHK_fn_lCreateHook((void**)&AI_fn_p_stEvalCondition, (void*)MOD_fn_p_stEvalCondition_DistanceCheck);
			FHK_fn_lCreateHook((void**)&GAM_fn_vAskToChangeLevel, (void*)MOD_fn_vAskToChangeLevel);
			FHK_fn_lCreateHook((void**)&GAM_fn_vChooseTheGoodDesInit, (void*)MOD_fn_vChooseTheGoodDesInit);
			FHK_fn_lCreateHook((void**)&GAM_fn_vChooseTheGoodInit, (void*)MOD_fn_vChooseTheGoodInit);
			FHK_fn_lCreateHook((void**)&fn_p_vDynAlloc, (void*)MOD_fn_vDynAlloc);
			FHK_fn_lCreateHook((void**)&fn_p_vGenAlloc, (void*)MOD_fn_vGenAlloc);
			FHK_fn_lCreateHook((void**)&Mmg_fn_vInitSpecificBlock, (void*)MOD_fn_vInitSpecificBlock);
			FHK_fn_lCreateHook((void**)&Mmg_fn_v_InitMmg, (void*)MOD_fn_v_InitMmg);
			FHK_fn_lCreateHook((void**)&SNA_fn_ulFRead, (void*)MOD_fn_ulFRead);
			FHK_fn_lCreateHook((void**)&GAM_fn_bCreateMainDisplayScreen, (void*)MOD_fn_bCreateMainDisplayScreen);
			FHK_fn_lCreateHook((void**)&GAM_fn_vDisplayAll, (void*)MOD_fn_vDisplayAll);
			FHK_fn_lCreateHook((void**)&INO_fn_wInit, (void*)MOD_INO_fn_wInit);
			FHK_fn_lCreateHook((void**)&GAM_fn_vInitGameLoop, (void*)MOD_fn_vInitGameLoop);

			HMODULE user32 = GetModuleHandleW(L"user32.dll");
			if (!user32) return;

			True_SetWindowPos = (PFN_SetWindowPos)GetProcAddress(user32, "SetWindowPos");

			FHK_fn_lCreateHook((PVOID*)&True_SetWindowPos, Hook_SetWindowPos);

			DR_RemoveLoadScreens();

			SPTXT_vInit();
			SPTXT_vAddTextCallback(VersionDisplay);

			break;

		case DLL_PROCESS_DETACH: 

			FHK_fn_lDestroyHook((void**)&GAM_fn_WndProc, (void*)MOD_fn_WndProc);
			FHK_fn_lDestroyHook((void**)&GAM_fn_vEngine, (void*)MOD_fn_vEngine);
			FHK_fn_lDestroyHook((void**)&AI_fn_p_stEvalTree, (void*)MOD_fn_p_stEvalTree_Debugger);
			FHK_fn_lDestroyHook((void**)&AI_fn_p_stEvalCondition, (void*)MOD_fn_p_stEvalCondition_DistanceCheck);
			FHK_fn_lDestroyHook((void**)&GAM_fn_vAskToChangeLevel, (void*)MOD_fn_vAskToChangeLevel);
			FHK_fn_lDestroyHook((void**)&GAM_fn_vChooseTheGoodDesInit, (void*)MOD_fn_vChooseTheGoodDesInit);
			FHK_fn_lDestroyHook((void**)&GAM_fn_vChooseTheGoodInit, (void*)MOD_fn_vChooseTheGoodInit);
			FHK_fn_lDestroyHook((void**)&fn_p_vDynAlloc, (void*)MOD_fn_vDynAlloc);
			FHK_fn_lDestroyHook((void**)&fn_p_vGenAlloc, (void*)MOD_fn_vGenAlloc);
			FHK_fn_lDestroyHook((void**)&Mmg_fn_vInitSpecificBlock, (void*)MOD_fn_vInitSpecificBlock);
			FHK_fn_lDestroyHook((void**)&Mmg_fn_v_InitMmg, (void*)MOD_fn_v_InitMmg);
			FHK_fn_lDestroyHook((void**)&SNA_fn_ulFRead, (void*)MOD_fn_ulFRead);
			FHK_fn_lDestroyHook((void**)&GAM_fn_bCreateMainDisplayScreen, (void*)MOD_fn_bCreateMainDisplayScreen);
			FHK_fn_lDestroyHook((void**)&GAM_fn_vDisplayAll, (void*)MOD_fn_vDisplayAll);
			FHK_fn_lDestroyHook((void**)&INO_fn_wInit, (void*)MOD_INO_fn_wInit);
			FHK_fn_lDestroyHook((void**)&GAM_fn_vInitGameLoop, (void*)MOD_fn_vInitGameLoop);

			SPTXT_vDeInit();

			g_bRunning = false;

			CloseHandle(g_hUIThread);
			CloseHandle(g_hFrameEvent);
			CloseHandle(g_hAFrameIsWaiting);
			CloseHandle(g_hFrameDoneCopying);

			break;

		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:

			break;
	}
	return TRUE;
}
