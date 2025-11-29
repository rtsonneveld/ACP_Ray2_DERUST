#include "ai_distancechecks.h"
#include <assert.h>

DistanceCheckEntry distanceChecks[MAX_DISTANCE_CHECKS];
int DR_DistanceCheckCount = 0;

void DR_DistanceChecks_Add(HIE_tdstSuperObject * spo, AI_tdstNodeInterpret* node, float x, float y, float z, float radius, BOOL active, CheckType type) {

  int index = 0;

  for (index = 0;index < DR_DistanceCheckCount;index++) {
    if (distanceChecks[index].spo == spo && distanceChecks[index].node == node) {
      break; // Re-use existing slot if it's the same superobject + AI node
    }
  }

  if (DR_DistanceCheckCount >= MAX_DISTANCE_CHECKS) {
    return; // Just ignore for now
  }

  distanceChecks[index].spo = spo;
  distanceChecks[index].node = node;
  distanceChecks[index].x = x;
  distanceChecks[index].y = y;
  distanceChecks[index].z = z;
  distanceChecks[index].radius = radius;
  distanceChecks[index].active = active;
  distanceChecks[index].type = type;
  distanceChecks[index].lifetime = 0;

  if (index == DR_DistanceCheckCount) {
    DR_DistanceCheckCount++;
  }
}

BOOL IsDead(HIE_tdstSuperObject* spo) {
  if (spo == NULL) return TRUE;
  HIE_tdstEngineObject* actor = spo->hLinkedObject.p_stActor;
  if (actor == NULL || actor->hStandardGame == NULL) return TRUE;
  return (actor->hStandardGame->ucMiscFlags & Std_C_MiscFlag_DesactivateAtAll);
}

void DR_DistanceChecks_Update() {
  for (int i = 0; i < DR_DistanceCheckCount; ) {
    distanceChecks[i].lifetime++;

    if (distanceChecks[i].lifetime > DR_DISTANCECHECKS_MAXLIFETIME || IsDead(distanceChecks[i].spo)) {
      // Remove by overwriting with last element
      DR_DistanceCheckCount--;
      distanceChecks[i] = distanceChecks[DR_DistanceCheckCount];
    }
    else {
      i++; // Only advance if we did not remove the current slot
    }
  }
}

void DR_DistanceChecks_Reset() {
  DR_DistanceCheckCount = 0;
}



AI_tdstNodeInterpret* MOD_fn_p_stEvalCondition_DistanceCheck(spo, node, param) {
  DR_DistanceChecks_CheckScript(spo, node, param);
  return AI_fn_p_stEvalCondition(spo, node, param);
}

void DR_DistanceChecks_CheckScript(HIE_tdstSuperObject* spo, AI_tdstNodeInterpret* node, AI_tdstGetSetParam* param)
{
  if (node->uParam.cValue != AI_E_cond_Lesser && node->uParam.cValue != AI_E_cond_LesserOrEqual &&
    node->uParam.cValue != AI_E_cond_Greater && node->uParam.cValue != AI_E_cond_GreaterOrEqual) return;

  AI_tdstNodeInterpret* nextNode = node + 1;

  if (nextNode->eType == AI_E_ti_Operator && nextNode->uParam.cValue == AI_E_op_Ultra) {
    nextNode = nextNode + 2;
  }

  if (nextNode->eType != AI_E_ti_Function) return;
  
  int checkType = nextNode->uParam.cValue;
  if (checkType < AI_E_func_DistanceToPerso || checkType > AI_E_func_DistanceToWP) return;

  MTH3D_tdstVector pos = spo->p_stGlobalMatrix->stPos;
  AI_tdstGetSetParam result;

  if (checkType == AI_E_func_DistanceToWP) { // In case of distance to waypoint check, evaluate the next node to find out which waypoint is being checked against

    AI_fn_p_stEvalTree(spo, nextNode + 1, &result);
    assert(result.ulType == AI_E_vt_WayPoint);
    WP_tdstWayPoint* waypoint = result.uParam.pvValue;
    pos = waypoint->m_stVertex;
  }

  int depth = nextNode->ucDepth;

  // Find the next node with the same depth, this is the value that the distance will be compared against (the radius of our distance check)
  do {
    nextNode = nextNode + 1;
  } while (nextNode->ucDepth > depth);

  AI_fn_p_stEvalTree(spo, nextNode, &result);
  
  float size = result.ulType == AI_E_vt_Integer ? result.uParam.cValue : result.uParam.xValue;

  if (checkType == AI_E_func_DistanceToPerso || checkType == AI_E_func_DistanceToPersoCenter || TRUE) {
    DR_DistanceChecks_Add(spo, node, pos.x, pos.y, pos.z, size, TRUE, CheckType_DistanceCheck);
  }
  
}