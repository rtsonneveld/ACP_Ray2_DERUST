#include "recording.h"
#include "globals.h"

DR_InputRecording DR_recording = { 0 };
DR_InputRecording_State DR_recording_state = DR_IR_State_Idle;

void DR_Recording_ResetInputStructure() {
  
  // Not sure if this is needed anymore
  for (int i = 0;i < IPT_g_stInputStructure->ulNumberOfEntryElement;i++) {

    if (i >= DR_RECORDING_MAX_ENTRIES) {
      break;
    }

    IPT_g_stInputStructure->d_stEntryElementArray[i].lState = 0;
    IPT_g_stInputStructure->d_stEntryElementArray[i].xAnalogicValue = 0;
  }

  // (This is important though:)
  // Reset input related globals to prevent desyncs
  *IPT_g_FieldPadGlobalVectorX = 0;
  *IPT_g_FieldPadGlobalVectorY = 0;
  *IPT_g_FieldPadGlobalVectorZ = 0;
  *IPT_g_FieldPadHorizontalAxis = 0;
  *IPT_g_FieldPadVerticalAxis = 0;
  *IPT_g_FieldPadAnalogForce = 0;
  *IPT_g_FieldPadTrueAnalogForce = 0;
  *IPT_g_FieldPadRotationAngle = 0;
  *IPT_g_FieldPadSector = 0;
}

void DR_Recording_ReloadTheMap() {
  MOD_fn_vAskToChangeLevel(GAM_fn_p_szGetLevelName(), FALSE);
}

void DR_Recording_Cleanup() {

  DR_InputRecordingFrame* frame = DR_recording.pFirstFrame;
  while (frame != NULL) {
    DR_InputRecordingFrame* nextFrame = frame->pNextFrame;
    free(frame);
    frame = nextFrame;
  }

  DR_recording.pFirstFrame = NULL;
  DR_recording.pLastFrame = NULL;
  DR_recording.pCurrentFrame = NULL;
  DR_recording.ulNumFrames = 0;
  DR_recording.ulCurrentFrame = 0;
}

// Hooked functions

void MOD_fn_vComputeRandomTable() {

  if (DR_recording_state != DR_IR_State_Recording &&
      DR_recording_state != DR_IR_State_Playback) {
    return RND_fn_vComputeRandomTable();
  }

  // TODO: programmable random seed?
  srand(0); // Consistent seed when recording/playbacking

  RND_g_stRandomStructure->ulMaxValueInTable = 0;
  for (long i = 0;i < RND_g_stRandomStructure->ulSizeOfTable;i++)
  {
    RND_g_stRandomStructure->p_ulTable[i] = rand();
  }
  RND_fn_vRemapRandomTable();
}

void MOD_fn_vEngineReadInput()
{

  switch(DR_recording_state) {
    case DR_IR_State_Recording:

      if (GAM_g_stEngineStructure->bEngineIsInPaused) {
        return;
      }

      IPT_fn_vEngineReadInput();
      DR_Recording_RecordFrame();
      break;
    case DR_IR_State_Playback:

      if (GAM_g_stEngineStructure->bEngineIsInPaused) {
        return;
      }

      DR_Recording_PlayBackFrame();
      break;
    case DR_IR_State_StartRecording:

      DR_Recording_Cleanup();
      DR_Recording_ResetInputStructure();

      g_DR_Cheats_FreezeProgress = FALSE;
      DR_Cheats_SaveProgress();

      DR_Recording_ReloadTheMap();

      DR_recording_state = DR_IR_State_Recording;

      break;
    case DR_IR_State_StartPlayback:

      DR_Recording_ResetInputStructure();

      DR_recording.ulCurrentFrame = 0;
      DR_recording.pCurrentFrame = DR_recording.pFirstFrame;

      DR_Cheats_LoadProgress();
      DR_Recording_ReloadTheMap();

      DR_recording_state = DR_IR_State_Playback;

      break;
    case DR_IR_State_Idle:

      IPT_fn_vEngineReadInput(); 
      break;
  }
}

DR_InputRecording_State DR_Recording_CurrentState()
{
  return DR_recording_state;
}

void DR_Recording_Start() {

  if (DR_recording_state != DR_IR_State_Idle) {
    return;
  }

  DR_recording_state = DR_IR_State_StartRecording;
}

void DR_Recording_PlayBackFrame() {

  if (DR_recording_state != DR_IR_State_Playback) {
    return;
  }

  GAM_g_stEngineStructure->stEngineTimer = DR_recording.pCurrentFrame->stEngineTimer;
  *GAM_g_stEngineStructure->g_hStdCamCharacter->p_stGlobalMatrix = DR_recording.pCurrentFrame->stCameraPos;

  for(int i=0;i< IPT_g_stInputStructure->ulNumberOfEntryElement;i++) {

    if (i >= DR_RECORDING_MAX_ENTRIES) {
      break;
    }

    IPT_g_stInputStructure->d_stEntryElementArray[i].lState = DR_recording.pCurrentFrame->a_stEntries[ i ].lState;
    IPT_g_stInputStructure->d_stEntryElementArray[i].xAnalogicValue = DR_recording.pCurrentFrame->a_stEntries[ i ].xAnalogicValue;

  }

  float diffX = (DR_recording.pCurrentFrame->stRaymanPosition.x - g_DR_rayman->p_stGlobalMatrix->stPos.x);
  float diffY = (DR_recording.pCurrentFrame->stRaymanPosition.y - g_DR_rayman->p_stGlobalMatrix->stPos.y);
  float diffZ = (DR_recording.pCurrentFrame->stRaymanPosition.z - g_DR_rayman->p_stGlobalMatrix->stPos.z);
  float desync = diffX * diffX + diffY * diffY + diffZ * diffZ;

  printf("Playback frame %i: x:%f, y:%f\n", DR_recording.ulCurrentFrame, DR_recording.pCurrentFrame->a_stEntries[0].xAnalogicValue, DR_recording.pCurrentFrame->a_stEntries[1].xAnalogicValue);

  printf("Playback frame %lu/%lu, desync=%f\n", DR_recording.ulCurrentFrame, DR_recording.ulNumFrames, desync);

  DR_recording.ulCurrentFrame++;
  DR_recording.pCurrentFrame = DR_recording.pCurrentFrame->pNextFrame;

  // Stop playback when reaching the end
  if (DR_recording.pCurrentFrame == NULL) {

    DR_recording.ulCurrentFrame = 0;
    DR_recording.pCurrentFrame = DR_recording.pFirstFrame;
    DR_recording_state = DR_IR_State_Idle;
  }
}

void DR_Recording_RecordFrame() {

  if (DR_recording_state != DR_IR_State_Recording) {
    return;
  }

  DR_InputRecordingFrame* newFrame = malloc(sizeof(DR_InputRecordingFrame));
  newFrame->pNextFrame = NULL;
  newFrame->stRaymanPosition = g_DR_rayman->p_stGlobalMatrix->stPos;
  newFrame->stCameraPos = *GAM_g_stEngineStructure->g_hStdCamCharacter->p_stGlobalMatrix;
  newFrame->stEngineTimer = GAM_g_stEngineStructure->stEngineTimer;

  if (DR_recording.ulCurrentFrame == 0) {
    DR_recording.startingCameraPosition = GAM_g_stEngineStructure->stMainCameraPosition;
  }

  for (int i = 0;i < IPT_g_stInputStructure->ulNumberOfEntryElement;i++) {

    if (i >= DR_RECORDING_MAX_ENTRIES) {
      break;
    }

    newFrame->a_stEntries[i].lState = IPT_g_stInputStructure->d_stEntryElementArray[i].lState;
    newFrame->a_stEntries[i].xAnalogicValue = IPT_g_stInputStructure->d_stEntryElementArray[i].xAnalogicValue;

  }
  
  printf("Recording frame %i: x:%f, y:%f\n", DR_recording.ulCurrentFrame, newFrame->a_stEntries[0].xAnalogicValue, newFrame->a_stEntries[1].xAnalogicValue);

  if (DR_recording.pFirstFrame == NULL) {
    DR_recording.pFirstFrame = newFrame;
    DR_recording.pLastFrame = newFrame;
  } else {
    DR_recording.pLastFrame->pNextFrame = newFrame;
    DR_recording.pLastFrame = newFrame;
  }

  DR_recording.ulNumFrames++;
  DR_recording.ulCurrentFrame++;
}

void DR_Recording_StopPlayback() {

  if (DR_recording_state != DR_IR_State_Playback) {
    return;
  }

  DR_recording_state = DR_IR_State_Idle;
}

void DR_Recording_Stop() {

  if (DR_recording_state != DR_IR_State_Recording) {
    return;
  }

  // Start playback immediately after stopping recording
  DR_recording_state = DR_IR_State_StartPlayback;
}