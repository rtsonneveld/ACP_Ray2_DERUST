#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include <ACP_Ray2.h>

#define MAX_BREAKPOINTS 1024

// Global variables
extern const void* g_DR_breakpoints[];
extern size_t g_DR_breakpoint_count;
extern bool g_DR_debuggerEnabled;
extern bool g_DR_debuggerPaused;
extern const AI_tdstNodeInterpret* g_DR_debuggerInstructionPtr;
extern const HIE_tdstSuperObject* g_DR_debuggerContextSPO;

AI_tdstNodeInterpret* MOD_fn_p_stEvalTree_Debugger(HIE_tdstSuperObject* spo, AI_tdstNodeInterpret* node, AI_tdstGetSetParam* param);

// Functions
bool DR_Debugger_HasBreakpoint(const void* address);
void DR_Debugger_SetBreakpoint(const void* address);
void DR_Debugger_UnsetBreakpoint(const void* address);


#ifdef __cplusplus
}
#endif