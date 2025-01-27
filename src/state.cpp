#include "state.h"

DR_State_Playback g_DR_Playback = {
  .pause = false,
  .unpause = false,
  .framestep = false,
  .lastFrame = 0,
};