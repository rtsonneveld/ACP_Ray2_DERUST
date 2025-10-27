#pragma once
#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

int DR_UI_Init(HWND p_r2Window, HMODULE hModule);
void DR_UI_Update();
void DR_UI_OnMapExit();
void DR_UI_DeInit();

#ifdef __cplusplus
}
#endif