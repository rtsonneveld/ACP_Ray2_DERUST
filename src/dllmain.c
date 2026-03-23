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
#include "mod/ai_disablerandopt.h"
#include "mod/util.h"
#include "mod/state.h"
#include "mod/globals.h"
#include "mod/cheats.h"
#include "mod/debugger.h"
#include "mod/recording.h"
#include <ACP_Ray2.h>
#include "ui/ui_bridge.h"
#include <time.h>
#include <ddraw.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad/glad.h>
#include <mod/dsgvarnames.h>

// Global handles
HMODULE g_hModule;

// Thread
HANDLE g_hUIThread = NULL;
HANDLE g_hInitEvent = NULL;
HANDLE g_hFrameEvent = NULL;
HANDLE g_hAFrameIsWaiting = NULL;
HANDLE g_hFrameDoneCopying = NULL;
volatile BOOL g_bRunning = true;

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

	if (DR_Recording_CurrentState() != DR_IR_State_Seeking) {
		WaitForSingleObject(g_hFrameDoneCopying, 100);
		WaitForSingleObject(g_hFrameEvent, 1); // This fixes random black/incomplete frames?!
	}

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
	
	g_DR_currentMapFrameCount = 0;

	if (DR_Settings_Get_DisableAutoSave()) {
		return GAM_fn_vAskToChangeLevel(szLevelName, FALSE);
	}
	return GAM_fn_vAskToChangeLevel(szLevelName, bSaveGame);
}

void MOD_fn_vChooseTheGoodInit() {

	GAM_tdeEngineMode requestedMode = GAM_g_stEngineStructure->eEngineMode;

	bool wasPaused = GAM_g_stEngineStructure->bEngineIsInPaused;

	// For consistent recording
	if (GAM_g_stEngineStructure->eEngineMode != E_EM_ModeStartingProgram) {
		DR_Recording_HK_fn_vComputeRandomTable();
	}

	GAM_fn_vChooseTheGoodInit();
	GAM_tdeEngineMode currentMode = GAM_g_stEngineStructure->eEngineMode; // The mode is reset after the requested mode has been initialized.

	if (requestedMode == E_EM_ModeDeadLoop) {
		DR_Cheats_LoadPosition();
	}

	GAM_g_stEngineStructure->bEngineIsInPaused = wasPaused;

	if (GAM_g_stEngineStructure->eEngineMode == E_EM_ModePlaying || requestedMode == E_EM_ModeDeadLoop) {
		if (g_DR_rayman == NULL) {
			g_DR_rayman = HIE_fn_p_stFindObjectByName("rayman");
#if _DEBUG
			g_DR_selectedObject = g_DR_rayman;
#endif
		}

		if (g_DR_global == NULL) {
			g_DR_global = HIE_fn_p_stFindObjectByName("global");
		}

		GLMRadar_Init();
	}
}

void MOD_fn_vChooseTheGoodDesInit() {

	GLMRadar_DeInit();

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
		if (DR_Recording_CurrentState() == DR_IR_State_Seeking) {

			ReleaseSemaphore(g_hFrameDoneCopying, 1, NULL);
			continue;
		}
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

	GLMRadar_Before_fn_vEngine();

	// Main Engine Loop
	{
		BOOL wasPaused = GAM_g_stEngineStructure->bEngineIsInPaused;
		
		// When seeking, the engine is not paused
		if (DR_Recording_CurrentState() == DR_IR_State_Seeking) {
			GAM_g_stEngineStructure->bEngineIsInPaused = FALSE;
		}

		if (DR_Settings_Get_TryCatchExceptions() && FALSE) {
			__try {
				GAM_fn_vEngine();
			}
			__except (LogExceptionFilter(GetExceptionInformation())) {
				__debugbreak();

				g_DR_Playback.pause = TRUE;
				g_DR_debuggerEnableBreakpoints = TRUE;
				g_DR_debuggerPaused = TRUE;

				DR_Debugger_SelectObjectAndComport(g_DR_debuggerContextSPO, g_DR_debuggerInstructionPtr);
			}
		}
		else {
			GAM_fn_vEngine();
		}

		GAM_g_stEngineStructure->bEngineIsInPaused = wasPaused; 
	}

	if (g_DR_Playback.pause) {

		g_DR_Playback.pause = FALSE;

		GAM_g_stEngineStructure->bEngineIsInPaused = TRUE;

		if (DR_Recording_CurrentState == DR_IR_State_Idle) {
			g_DR_Playback.lastFrame = *HIE_gs_lCurrentFrame;
			GAM_fn_vSaveEngineClock();
		}
	}
	if (g_DR_Playback.unpause) {

		g_DR_Playback.unpause = FALSE;

		GAM_g_stEngineStructure->bEngineIsInPaused = FALSE;

		if (DR_Recording_CurrentState == DR_IR_State_Idle) {
			*HIE_gs_lCurrentFrame = g_DR_Playback.lastFrame;
			GAM_fn_vLoadEngineClock();
		}

		if (g_DR_Playback.framestep) {
			g_DR_Playback.pause = TRUE;
			g_DR_Playback.framestep = FALSE;
		}
	}

	SetEvent(g_hFrameEvent);

	if (!GAM_g_stEngineStructure->bEngineIsInPaused) {
		g_DR_totalFrameCount++;
		g_DR_currentMapFrameCount++;
	}
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

void MOD_fn_vFillDynamicHierarchy(HIE_tdstSuperObject* _hSOCurrentSector, ACP_tdxBool _bInit) {

	if (!g_DR_Cheats_Simulate256FramesBetweenSectorReloads) {

		GAM_fn_vFillDynamicHierarchy(_hSOCurrentSector, _bInit);
		return;
	}

	char oldCounter = _hSOCurrentSector->hLinkedObject.p_stSector->cCounter;
	char timerAsChar = (char)GAM_g_stEngineStructure->stEngineTimer.ulFrameNumber;
	
	unsigned long oldFrameNumber = GAM_g_stEngineStructure->stEngineTimer.ulFrameNumber;
	GAM_g_stEngineStructure->stEngineTimer.ulFrameNumber = 0;
	HIE_M_ForEachSector(sector) {
		sector->hLinkedObject.p_stSector->cCounter = 0;
	}

	GAM_fn_vFillDynamicHierarchy(_hSOCurrentSector, _bInit);

	GAM_g_stEngineStructure->stEngineTimer.ulFrameNumber = oldFrameNumber;
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
			FHK_fn_lCreateHook((void**)&AI_fn_p_stEvalKeyWord, (void*)MOD_fn_p_stEvalKeyWord_DisableMicroRNG);
			FHK_fn_lCreateHook((void**)&GAM_fn_b3dDataCanComputeBrain, (void*)MOD_fn_b3dDataCanComputeBrain);
			FHK_fn_lCreateHook((void**)&GAM_fn_vAskToChangeLevel, (void*)MOD_fn_vAskToChangeLevel);
			FHK_fn_lCreateHook((void**)&GAM_fn_vChooseTheGoodDesInit, (void*)MOD_fn_vChooseTheGoodDesInit);
			FHK_fn_lCreateHook((void**)&GAM_fn_vChooseTheGoodInit, (void*)MOD_fn_vChooseTheGoodInit);
			FHK_fn_lCreateHook((void**)&GAM_fn_bCreateMainDisplayScreen, (void*)MOD_fn_bCreateMainDisplayScreen);
			FHK_fn_lCreateHook((void**)&GAM_fn_vDisplayAll, (void*)MOD_fn_vDisplayAll);
			FHK_fn_lCreateHook((void**)&INO_fn_wInit, (void*)MOD_INO_fn_wInit);
			FHK_fn_lCreateHook((void**)&GAM_fn_vInitGameLoop, (void*)MOD_fn_vInitGameLoop);
			FHK_fn_lCreateHook((void**)&GAM_fn_vFillDynamicHierarchy, (void*)MOD_fn_vFillDynamicHierarchy);

			// Recording
			FHK_fn_lCreateHook((void**)&GLD_bFlipDeviceWithSynchro, (void*)DR_Recording_HK_bFlipDeviceWithSynchro);
			FHK_fn_lCreateHook((void**)&IPT_fn_vReadInput, (void*)DR_Recording_HK_fn_vReadInput);
			FHK_fn_lCreateHook((void**)&HIE_fn_vSendStaticWorldToViewport, (void*)DR_Recording_HK_fn_vSendStaticWorldToViewport);
			FHK_fn_lCreateHook((void**)&HIE_fn_vSendCharacterModulesToViewPort, (void*)DR_Recording_HK_fn_vSendCharacterModulesToViewPort);
			FHK_fn_lCreateHook((void**)&RND_fn_vComputeRandomTable, (void*)DR_Recording_HK_fn_vComputeRandomTable);
			FHK_fn_lCreateHook((void**)&SND_fn_vSynchroSound, (void*)DR_Recording_HK_fn_vSynchroSound);
			FHK_fn_lCreateHook((void**)&SND_fn_lSendRequestSound, (void*)DR_Recording_HK_fn_lSendRequestSound);
			FHK_fn_lCreateHook((void**)&RND_fn_vComputeRandomTable, (void*)DR_Recording_HK_fn_vComputeRandomTable);
			FHK_fn_lCreateHook((void**)&GAM_fn_vActualizeEngineClock, (void*)DR_Recording_HK_fn_vActualizeEngineClock);

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
			FHK_fn_lDestroyHook((void**)&AI_fn_p_stEvalKeyWord, (void*)MOD_fn_p_stEvalKeyWord_DisableMicroRNG);
			FHK_fn_lDestroyHook((void**)&GAM_fn_b3dDataCanComputeBrain, (void*)MOD_fn_b3dDataCanComputeBrain);
			FHK_fn_lDestroyHook((void**)&GAM_fn_vAskToChangeLevel, (void*)MOD_fn_vAskToChangeLevel);
			FHK_fn_lDestroyHook((void**)&GAM_fn_vChooseTheGoodDesInit, (void*)MOD_fn_vChooseTheGoodDesInit);
			FHK_fn_lDestroyHook((void**)&GAM_fn_vChooseTheGoodInit, (void*)MOD_fn_vChooseTheGoodInit);

			/*
			FHK_fn_lDestroyHook((void**)&fn_p_vDynAlloc, (void*)MOD_fn_vDynAlloc);
			FHK_fn_lDestroyHook((void**)&fn_p_vGenAlloc, (void*)MOD_fn_vGenAlloc);
			FHK_fn_lDestroyHook((void**)&Mmg_fn_vInitSpecificBlock, (void*)MOD_fn_vInitSpecificBlock);
			FHK_fn_lDestroyHook((void**)&Mmg_fn_v_InitMmg, (void*)MOD_fn_v_InitMmg);
			FHK_fn_lDestroyHook((void**)&SNA_fn_ulFRead, (void*)MOD_fn_ulFRead);
			*/

			FHK_fn_lDestroyHook((void**)&GAM_fn_bCreateMainDisplayScreen, (void*)MOD_fn_bCreateMainDisplayScreen);
			FHK_fn_lDestroyHook((void**)&GAM_fn_vDisplayAll, (void*)MOD_fn_vDisplayAll);
			FHK_fn_lDestroyHook((void**)&INO_fn_wInit, (void*)MOD_INO_fn_wInit);
			FHK_fn_lDestroyHook((void**)&GAM_fn_vInitGameLoop, (void*)MOD_fn_vInitGameLoop);

			SPTXT_vDeInit();

			g_bRunning = false;

			break;

		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:

			break;
	}
	return TRUE;
}
