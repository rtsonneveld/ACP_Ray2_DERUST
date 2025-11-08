#include "debugger.h"
#include <ACP_Ray2.h>

const void* g_DR_breakpoints[MAX_BREAKPOINTS];
bool g_DR_debuggerEnabled = false;
bool g_DR_debuggerPaused = false;
size_t g_DR_breakpoint_count = 0;
const AI_tdstNodeInterpret* g_DR_debuggerInstructionPtr = NULL;
const HIE_tdstSuperObject* g_DR_debuggerContextSPO = NULL;

AI_tdstNodeInterpret * MOD_fn_p_stEvalTree_Debugger(HIE_tdstSuperObject* spo, AI_tdstNodeInterpret* node, AI_tdstGetSetParam* param) {

  while (DR_Debugger_HasBreakpoint(node)) {
    DR_UI_Update();
  }

  return AI_fn_p_stEvalTree(spo, node, param);
}

bool DR_Debugger_HasBreakpoint(const void* address)
{
  if (!g_DR_debuggerEnabled) {
    return false;
  }

  for (size_t i = 0; i < g_DR_breakpoint_count; ++i)
    if (g_DR_breakpoints[i] == address)
      return true;
  return false;
}

void DR_Debugger_SetBreakpoint(const void* address)
{
  if (address == NULL || DR_Debugger_HasBreakpoint(address))
    return;

  if (g_DR_breakpoint_count < MAX_BREAKPOINTS)
    g_DR_breakpoints[g_DR_breakpoint_count++] = address;
}

void DR_Debugger_UnsetBreakpoint(const void* address)
{
  for (size_t i = 0; i < g_DR_breakpoint_count; ++i)
  {
    if (g_DR_breakpoints[i] == address)
    {
      // move the last element to this slot to fill the gap
      g_DR_breakpoints[i] = g_DR_breakpoints[--g_DR_breakpoint_count];
      return;
    }
  }
}