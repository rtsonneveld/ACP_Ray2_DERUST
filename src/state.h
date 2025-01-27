#pragma once

typedef struct {
  char pause;
  char unpause;
  char framestep;
  long lastFrame;
} DR_State_Playback;

extern DR_State_Playback g_DR_Playback;