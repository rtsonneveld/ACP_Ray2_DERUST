#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <ACP_Ray2.h>

#define MAX_DISTANCE_CHECKS 128
#define DR_DISTANCECHECKS_MAXLIFETIME 16

  // Create the enum as requested
  typedef enum {
    CheckType_DistanceCheck
  } CheckType;

  // Structure to hold one distance check entry
  typedef struct {
    HIE_tdstSuperObject* spo;
    AI_tdstNodeInterpret* node;
    float x;
    float y;
    float z;
    float radius;
    BOOL active;
    CheckType type;
    int lifetime;
  } DistanceCheckEntry;

  extern DistanceCheckEntry distanceChecks[MAX_DISTANCE_CHECKS];
  extern int DR_DistanceCheckCount;

  AI_tdstNodeInterpret* MOD_fn_p_stEvalCondition_DistanceCheck(HIE_tdstSuperObject* spo, AI_tdstNodeInterpret* node, AI_tdstGetSetParam* param);

  void DR_DistanceChecks_Update();
  void DR_DistanceChecks_Reset();
  void DR_DistanceChecks_CheckScript(HIE_tdstSuperObject* spo, AI_tdstNodeInterpret* node, AI_tdstGetSetParam* param);
  void DR_DistanceChecks_Add(HIE_tdstSuperObject* spo, AI_tdstNodeInterpret* node, float x, float y, float z, float radius, BOOL active, CheckType type);

#ifdef __cplusplus
}
#endif