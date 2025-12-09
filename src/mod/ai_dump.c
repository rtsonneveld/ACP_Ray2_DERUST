#include "ai_dump.h"

BOOL aiDumpActive = FALSE;
LogItem logBuffer[DR_AIDUMP_LOG_CAPACITY];
unsigned long logCount = 0;

void DR_AIDUMP_AddNode(HIE_tdstSuperObject* spo,
  AI_tdstComport* comport,
  AI_tdstComport* reflex,
  AI_tdstNodeInterpret* node,
  AI_tdstGetSetParam* param)
{
  if (logCount >= DR_AIDUMP_LOG_CAPACITY)
    return;

  logBuffer[logCount].Index = logCount;
  logBuffer[logCount].Frame = GAM_g_stEngineStructure->stEngineTimer.ulFrameNumber;
  logBuffer[logCount].Spo = spo;
  logBuffer[logCount].Comport = comport;
  logBuffer[logCount].Reflex = reflex;
  logBuffer[logCount].Node = node;

  logCount++;
}

BOOL DR_AIDUMP_IsActive() {
  return aiDumpActive;
}

unsigned long DR_AIDUMP_GetLogCount() {
  return logCount;
}

LogItem DR_AIDUMP_GetLogItem(unsigned long index) {
  if (index < 0 || index >= logCount) {
    LogItem empty = {0};
    empty.Spo = NULL;
    empty.Node = NULL;
    return empty;
  }
  return logBuffer[index];
}

void DR_AIDUMP_Start() {
  if (!aiDumpActive) {
    DR_AIDUMP_Clear();
    aiDumpActive = TRUE;
  }
}

void DR_AIDUMP_Stop() {
  aiDumpActive = FALSE;
}

void DR_AIDUMP_Clear()
{
  /* Ensure we clear the actual logCount used by this module. */
  logCount = 0;
}
