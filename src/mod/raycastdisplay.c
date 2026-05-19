#include "raycastdisplay.h"

DR_Raycast DR_raycasts[DR_Raycast_Max] = { 0 };

void DR_AddRaycast(DR_Raycast raycast) {
  for(int i=0; i < DR_Raycast_Max; i++) {
    if (DR_raycasts[i].age == 0) {
      DR_raycasts[i] = raycast;
      return;
    }
  }
}

void DR_UpdateRaycasts() {

  int lifetime = DR_Settings_Get_RaycastLifetime();

  for (int i = 0; i < DR_Raycast_Max; i++) {
    if (DR_raycasts[i].age > 0) {
      DR_raycasts[i].age++;
      if (DR_raycasts[i].age > lifetime && lifetime > 0) {
        DR_raycasts[i].age = 0;
      }
    }
  }
}

void MOD_fn_vComputeCollideResult_RayCastDisplay(HIE_tdstSuperObject* sender) {

  AI_fn_vComputeCollideResult(sender);
  
  if (!DR_Settings_Get_DrawRaycasts()) return;

  DR_Raycast raycast = {
    .eType = DR_RaycastType_CollideResult,
      .collideResult = {
        .paramPoint = *AI_g_stCollideParameterPoint,
        .paramVector = *AI_g_stCollideParameterVector,
        .resultPoint = *AI_g_stCollideResultPoint,
        .resultVector = *AI_g_stCollideResultVector,
        .hitSuperObject = *AI_g_hSuperObjectHit
      },
    .age = 1,
  };

  DR_AddRaycast(raycast);
}

ACP_tdxBool MOD_fn_bDetectIntersectSegmentWithTriangle_RayCastDisplay(MTH3D_tdstVector* p_stVertexA, MTH3D_tdstVector* p_stVectAB, MTH3D_tdstVector* p_stVertex1, MTH3D_tdstVector* p_stVertex2, MTH3D_tdstVector* p_stVertex3, MTH3D_tdstVector* p_stNormal, MTH_tdxReal xDPlan) {
  BOOL result = INT_fn_bDetectIntersectSegmentWithTriangle(p_stVertexA, p_stVectAB, p_stVertex1, p_stVertex2, p_stVertex3, p_stNormal, xDPlan);


  if (!DR_Settings_Get_DrawRaycasts()) return result;

  DR_Raycast raycast = {
  .eType = DR_RaycastType_IntersectSegmentTriangle,
    .intersectSegmentTriangle = {
      .paramPoint = *p_stVertexA,
      .paramVector = *p_stVectAB,
      .vertex1 = *p_stVertex1,
      .vertex2 = *p_stVertex2,
      .vertex3 = *p_stVertex3,
      .isHit = result,
    },
  .age = 1,
  };

  DR_AddRaycast(raycast);

  return result;
}