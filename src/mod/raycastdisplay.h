#pragma once
#include <ACP_Ray2.h> 

#define DR_Raycast_Max 1000

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct DR_Raycast {
    MTH3D_tdstVector paramPoint;
    MTH3D_tdstVector paramVector;
    MTH3D_tdstVector resultPoint;
    MTH3D_tdstVector resultVector;
    HIE_tdstSuperObject* hitSuperObject;
    unsigned long age;
  } DR_Raycast;

  extern DR_Raycast DR_raycasts[DR_Raycast_Max];

  void DR_AddRaycast(DR_Raycast raycast);
  void DR_UpdateRaycasts();

  void MOD_fn_vComputeCollideResult_RayCastDisplay(HIE_tdstSuperObject* sender);

#ifdef __cplusplus
}
#endif