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

  DR_Raycast raycast = {
    .paramPoint = *AI_g_stCollideParameterPoint,
    .paramVector = *AI_g_stCollideParameterVector,
    .resultPoint = *AI_g_stCollideResultPoint,
    .resultVector = *AI_g_stCollideResultVector,
    .hitSuperObject = *AI_g_hSuperObjectHit,
    .age = 1,
  };

  DR_AddRaycast(raycast);
}