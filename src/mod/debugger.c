#include "debugger.h"
#include "mod/globals.h"
#include "ai_dump.h"
#include "mod/ai_distancechecks.h"
#include "ui/ui_bridge.h"
#include <ACP_Ray2.h>

const void* g_DR_breakpoints[MAX_BREAKPOINTS];
bool g_DR_debuggerEnabled = false;
bool g_DR_debuggerPaused = false;
bool g_DR_debuggerStep = false;
size_t g_DR_breakpoint_count = 0;
int g_DR_debuggerStepOverDepth = 0;
const AI_tdstNodeInterpret* g_DR_debuggerInstructionPtr = NULL;
const HIE_tdstSuperObject* g_DR_debuggerContextSPO = NULL;

extern HANDLE g_hFrameEvent;

void SelectComportInDialog(HIE_tdstSuperObject * spo, AI_tdstNodeInterpret* nodeToCheck, AI_tdstScriptAI* ai, bool isReflex) {
  
  if (ai == NULL) {
    return;
  }
  AI_tdstNodeInterpret* node;

  for (int i = 0;i < ai->ulNbComport;i++) {
    AI_tdstComport comport = ai->a_stComport[i];
    for (int j = 0;j < comport.ucNbRules;j++) {
      node = comport.a_stRules[j].p_stNodeInterpret;
      while (node->eType != AI_E_ti_EndTree) {

        if (node == nodeToCheck) {

          DR_DLG_AiModel_SetSelectedComport(i, isReflex);
          return;
        }

        node++;
      }
    }

    if (comport.p_stSchedule == NULL || comport.p_stSchedule->p_stNodeInterpret == NULL) continue;
    node = comport.p_stSchedule->p_stNodeInterpret;

    while (node->eType != AI_E_ti_EndTree) {

      if (node == nodeToCheck) {

        DR_DLG_AiModel_SetSelectedComport(i, isReflex);
        return;
      }

      node++;
    }
  }


}

void DR_Debugger_SelectObjectAndComport(HIE_tdstSuperObject* spo, AI_tdstNodeInterpret* node) {
  g_DR_selectedObject = spo;

  AI_tdstAIModel* aiModel = spo->hLinkedObject.p_stActor->hBrain->p_stMind->p_stAIModel;
  SelectComportInDialog(spo, node, aiModel->a_stScriptAIIntel, false);
  SelectComportInDialog(spo, node, aiModel->a_stScriptAIReflex, true);
}

AI_tdstNodeInterpret * MOD_fn_p_stEvalTree_Debugger(HIE_tdstSuperObject* spo, AI_tdstNodeInterpret* node, AI_tdstGetSetParam* param) {

  g_DR_debuggerInstructionPtr = node;
  g_DR_debuggerContextSPO = spo;

  if (g_DR_debuggerEnabled) {

    if (DR_Debugger_HasBreakpoint(node)) {
      g_DR_debuggerPaused = true;
    }

    if (g_DR_debuggerStepOverDepth > 0)  {
      if (node->ucDepth <= g_DR_debuggerStepOverDepth) {
        g_DR_debuggerPaused = true;
        g_DR_debuggerStepOverDepth = 0;
      }
    }

    if (g_DR_debuggerPaused) {
      DR_Debugger_SelectObjectAndComport(spo, node);
    }

    while (g_DR_debuggerPaused) {
      
      SetEvent(g_hFrameEvent);

      if (g_DR_debuggerStep) {
        g_DR_debuggerStep = false;
        break;
      }
    }

  }

  if (DR_AIDUMP_IsActive()) {
    AI_tdstMind* mind = spo->hLinkedObject.p_stActor->hBrain->p_stMind;

    if (node->eType == AI_E_ti_Procedure) { 

      DR_AIDUMP_AddNode(spo,
        mind->p_stIntelligence != NULL && mind->bDoingIntel == 1 ? mind->p_stIntelligence->p_stCurrentComport : NULL,
        mind->p_stReflex != NULL && mind->bDoingIntel == 0 ? mind->p_stReflex->p_stCurrentComport : NULL,
        node, param);

    }
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

void DR_Debugger_StepOver()
{
  if (!g_DR_debuggerPaused) {
    return;
  }
  g_DR_debuggerPaused = false;
  if (g_DR_debuggerInstructionPtr != NULL) {
    g_DR_debuggerStepOverDepth = g_DR_debuggerInstructionPtr->ucDepth;
  }
}

void DR_Debugger_StepInto()
{
  if (!g_DR_debuggerPaused) {
    return;
  }
  g_DR_debuggerStep = true;
}

void DR_Debugger_Continue()
{
  if (!g_DR_debuggerPaused) {
    return;
  }
  g_DR_debuggerPaused = false;
}
