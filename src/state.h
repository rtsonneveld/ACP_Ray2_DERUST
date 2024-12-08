#pragma once
#include <ACP_Ray2.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  char pause;
  char unpause;
  char framestep;
  long lastFrame;
} DR_State_Playback;

extern DR_State_Playback g_DR_Playback;

#ifdef __cplusplus
}
#endif