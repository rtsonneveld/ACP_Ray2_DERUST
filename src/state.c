#include "state.h"

DR_State_Playback g_DR_Playback = {
  .pause = FALSE,
  .unpause = FALSE,
  .framestep = FALSE,
  .lastFrame = 0,
};