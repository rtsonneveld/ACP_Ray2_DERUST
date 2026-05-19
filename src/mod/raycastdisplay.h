#pragma once
#include <ACP_Ray2.h> 

#define DR_Raycast_Max 10000

#ifdef __cplusplus
extern "C" {
#endif

  typedef enum DR_RaycastType {
    DR_RaycastType_CollideResult = 0,
    DR_RaycastType_IntersectSegmentTriangle = 1,
  } DR_RaycastType;

  typedef struct DR_Raycast {

    DR_RaycastType eType;
    union {
      struct {
        MTH3D_tdstVector paramPoint;
        MTH3D_tdstVector paramVector;
        MTH3D_tdstVector resultPoint;
        MTH3D_tdstVector resultVector;
        HIE_tdstSuperObject* hitSuperObject;
      } collideResult;
      struct {
        MTH3D_tdstVector paramPoint;
        MTH3D_tdstVector paramVector;
        MTH3D_tdstVector vertex1;
        MTH3D_tdstVector vertex2;
        MTH3D_tdstVector vertex3;
        ACP_tdxBool isHit;
      } intersectSegmentTriangle;
    };
    unsigned long age;
  } DR_Raycast;

  extern DR_Raycast DR_raycasts[DR_Raycast_Max];

  void DR_AddRaycast(DR_Raycast raycast);
  void DR_UpdateRaycasts();

  void MOD_fn_vComputeCollideResult_RayCastDisplay(HIE_tdstSuperObject* sender);
  ACP_tdxBool MOD_fn_bDetectIntersectSegmentWithTriangle_RayCastDisplay(MTH3D_tdstVector* p_stVertexA, MTH3D_tdstVector* p_stVectAB,
    MTH3D_tdstVector* p_stVertex1, MTH3D_tdstVector* p_stVertex2, MTH3D_tdstVector* p_stVertex3, MTH3D_tdstVector* p_stNormal, MTH_tdxReal xDPlan);

#ifdef __cplusplus
}
#endif