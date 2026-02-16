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
  DR_IR_State_StartSeeking,
  DR_IR_State_Seeking,
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
  unsigned char hitPoints;
  unsigned char hitPointsMax;
  unsigned char hitPointsMaxMax;
  DR_InputRecordingFrame* pCurrentFrame;
} DR_InputRecording;

extern DR_InputRecording DR_recording;
extern float DR_recording_desync;

DR_InputRecording_State DR_Recording_CurrentState();
void DR_Recording_Start();
void DR_Recording_RecordFrame();
void DR_Recording_StopPlayback();
void DR_Recording_PlayBackFrame();
void DR_Recording_SeekTo(unsigned long frameNum);
void DR_Recording_Stop();
void DR_Recording_StartPlayback();

// Hooked functions
void DR_Recording_HK_fn_vComputeRandomTable();
BOOL DR_Recording_HK_bFlipDeviceWithSynchro();
long DR_Recording_HK_fn_lSendSectorToViewportStatic(MTH3D_tdstVector* _p_stAbsolutePositionOfCamera, GLD_tdstViewportAttributes* _p_stVpt, HIE_tdstSuperObject* _hSprObjSector, long _lDrawMask);
void DR_Recording_HK_fn_vSendCharacterModulesToViewPort(GLD_tdstViewportAttributes* _hVpt, HIE_tdstSuperObject* _hSprObj, long _DrawMask);
long DR_Recording_HK_fn_lSendRequestSound(long lIndice, long lType, SND_tduRefEvt uEvt, long lPrio, SND_td_pfn_vSoundCallback pfnProc);
void DR_Recording_HK_fn_vSynchroSound();
void DR_Recording_HK_fn_vEngineReadInput();

#ifdef __cplusplus
    }
#endif