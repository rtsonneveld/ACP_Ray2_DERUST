#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include <ACP_Ray2.h>

#define MAX_BREAKPOINTS 1024

typedef struct tdstBreakpoint {
  void* address;
  HIE_tdstSuperObject* spo;
} tdstBreakpoint;

// Global variables
extern tdstBreakpoint g_DR_breakpoints[];
extern size_t g_DR_breakpoint_count;
extern bool g_DR_debuggerEnableBreakpoints;
extern bool g_DR_debuggerDisableBreakpointedNodes;
extern bool g_DR_debuggerPaused;
extern bool g_DR_debuggerStep;
extern const AI_tdstNodeInterpret* g_DR_debuggerInstructionPtr;
extern const HIE_tdstSuperObject* g_DR_debuggerContextSPO;

// Global breakpoint
extern bool g_DR_debugger_globalBreakpointEnabled;
extern AI_tdeTypeInterpret g_DR_debugger_globalBreakpointNodeType;
extern unsigned long g_DR_debugger_globalBreakpointNodeParam;
extern GAM_tdxObjectType g_DR_debugger_globalBreakpointModelTypeFilter;
extern bool g_DR_debugger_globalBreakpointEnableModelTypeFilter;

// Functions
AI_tdstNodeInterpret* MOD_fn_p_stEvalTree_Debugger(HIE_tdstSuperObject* spo, AI_tdstNodeInterpret* node, AI_tdstGetSetParam* param);

bool DR_Debugger_HasBreakpoint(HIE_tdstSuperObject* spo, AI_tdstNodeInterpret* node);
void DR_Debugger_SetBreakpoint(const void* address, HIE_tdstSuperObject* spo);
void DR_Debugger_UnsetBreakpoint(const void* address, HIE_tdstSuperObject* spo);
void DR_Debugger_SelectObjectAndComport(HIE_tdstSuperObject* spo, AI_tdstNodeInterpret* node);
void DR_Debugger_StepOver();
void DR_Debugger_StepInto();
void DR_Debugger_Continue();

#ifdef __cplusplus
}
#endif