#pragma once
#include <ACP_Ray2.h>
#include "cheats.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DR_RECORDING_MAX_ENTRIES 138

typedef enum DR_InputRecording_State {
  DR_IR_State_Idle,
  DR_IR_State_StartRecording,
  DR_IR_State_Recording,
  DR_IR_State_StartPlayback,
  DR_IR_State_Playback,
} DR_InputRecording_State;

typedef struct DR_InputRecordingEntry {
  signed long lState;
  MTH_tdxReal xAnalogicValue;
} DR_InputRecordingEntry;

typedef struct DR_InputRecordingFrame {
  DR_InputRecordingEntry a_stEntries[DR_RECORDING_MAX_ENTRIES];
  struct DR_InputRecordingFrame* pNextFrame;
  MTH3D_tdstVector stRaymanPosition;
  POS_tdstCompletePosition stCameraPos;
  TMR_tdstEngineTimerStructure stEngineTimer;
} DR_InputRecordingFrame;

typedef struct DR_InputRecording {
  DR_InputRecordingFrame* pFirstFrame;
  DR_InputRecordingFrame* pLastFrame;
  unsigned long ulNumFrames;
  unsigned long ulCurrentFrame;
  unsigned int savedProgress[GLOBAL_BITS_ARRAYSIZE];
  POS_tdstCompletePosition startingCameraPosition;
  DR_InputRecordingFrame* pCurrentFrame;
} DR_InputRecording;

DR_InputRecording_State DR_Recording_CurrentState();
void DR_Recording_Start();
void DR_Recording_RecordFrame();
void DR_Recording_StopPlayback();
void DR_Recording_PlayBackFrame();
void DR_Recording_Stop();

// Hooked functions
void MOD_fn_vEngineReadInput();
void MOD_fn_vComputeRandomTable();

#ifdef __cplusplus
    }
#endif