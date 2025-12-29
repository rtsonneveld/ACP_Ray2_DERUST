#pragma once
#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WM_DR_INITWND WM_APP+1

extern HWND hWindow;
extern HWND hWndR2;

int DR_UI_Init(HWND p_r2Window, HMODULE hModule);
void DR_UI_Update();
void DR_UI_OnMapExit();
void DR_UI_DeInit();

bool DR_UI_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void DR_DLG_AiModel_SetSelectedComport_Intelligence(int comportIndex);
void DR_DLG_AiModel_SetSelectedComport_Reflex(int comportIndex);
void DR_DLG_AiModel_SetSelectedComport_Macro(int comportIndex);
bool DR_Settings_IsCatchExceptionsEnabled();

#ifdef __cplusplus
}
#endif