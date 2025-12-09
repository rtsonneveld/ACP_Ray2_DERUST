#pragma once
#include <stdlib.h>
#include <ACP_Ray2.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct HIE_tdstSuperObject HIE_tdstSuperObject;
  typedef struct AI_tdstNodeInterpret AI_tdstNodeInterpret;

  typedef struct LogItem {
    unsigned long Index;
    unsigned long Frame;
    HIE_tdstSuperObject* Spo;
    AI_tdstComport* Comport;
    AI_tdstComport* Reflex;
    AI_tdstNodeInterpret* Node;
  } LogItem;

#define DR_AIDUMP_LOG_CAPACITY (60 * 60 * 100) // 1 minute at 60 FPS and 100 nodes every frame

  void DR_AIDUMP_AddNode(HIE_tdstSuperObject* spo,
    AI_tdstComport* Comport,
    AI_tdstComport* Reflex,
    AI_tdstNodeInterpret* node,
    AI_tdstGetSetParam* param);

  BOOL DR_AIDUMP_IsActive();
  unsigned long DR_AIDUMP_GetLogCount();
  LogItem DR_AIDUMP_GetLogItem(unsigned long index);
  void DR_AIDUMP_Start();
  void DR_AIDUMP_Stop();
  void DR_AIDUMP_Clear();

#ifdef __cplusplus
}
#endif