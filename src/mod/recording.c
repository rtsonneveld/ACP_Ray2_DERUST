#include "recording.h"
#include "bruteforce.h"
#include "recordingfile.h"
#include "globals.h"
#include "util.h"
#include "dsgvarnames.h"
#include "randutil.h"

DR_InputRecording DR_recording = { 0 };
DR_InputRecording_State DR_recording_state = DR_IR_State_Idle;
DR_InputRecording_State DR_recording_stateAfterSeek;
//BOOL DR_recording_pauseAfterSeek;
unsigned long DR_recording_seekTarget = 0;
float DR_recording_desync = 0.0f;

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

  *IPT_g_bCancelKeyboardInertia = FALSE;
  *IPT_s_xPrevKeyXValue = 0;
  *IPT_s_xPrevKeyYValue = 0;
}

void FullResetCineInfo(CAM_tdstCineinfo* cineInfo) {

  CAM_tdstInternalStructurCineinfo* hInit = cineInfo->hInit;
  CAM_tdstInternalStructurCineinfo* hCurrent = cineInfo->hCurrent;
  CAM_tdstInternalStructurCineinfo* hVisibility = cineInfo->hVisibility;
  CAM_tdstInternalStructurCineinfo* hWork = cineInfo->hWork;

  CAM_tdeListViewport vp_hInit = hInit->eTypeOfViewport;
  CAM_tdeListViewport vp_hCurrent = hCurrent->eTypeOfViewport;
  CAM_tdeListViewport vp_hVisibility = hVisibility->eTypeOfViewport;
  CAM_tdeListViewport vp_hWork = hWork->eTypeOfViewport;

  HIE_tdstSuperObject* spoTargeted = hInit->hSuperObjectTargeted;

  memcpy(hCurrent, hInit, sizeof(CAM_tdstInternalStructurCineinfo));
  memcpy(hVisibility, hInit, sizeof(CAM_tdstInternalStructurCineinfo));
  memcpy(hWork, hInit, sizeof(CAM_tdstInternalStructurCineinfo)); 

  memset(cineInfo, 0, sizeof(CAM_tdstCineinfo)); 

  cineInfo->hInit = hInit;
  cineInfo->hCurrent = hCurrent;
  cineInfo->hVisibility = hVisibility;
  cineInfo->hWork = hWork;

  memset(CAM_g_stIdeal, 0, sizeof(CAM_tdstInternalStructurCineinfo));

  *CAM_g_cCanTestStatic = 0;
  *CAM_g_cRefAxisIsAlreadyComputed = 0;
  *CAM_g_cNoDynChangeTheta = 0;
  *CAM_g_cJustBetterPos = 0;

  memset(CAM_g_stCameraConstants, 0, sizeof(CAM_tdstCameraConstants));
  memset(CAM_g_stCameraCopyConstants, 0, sizeof(CAM_tdstCameraConstants));
}

void DR_Recording_ReloadTheMap() {

  // Global state reset
  memset(g_stSpeedVector, 0, sizeof(MTH3D_tdstVector)); 

  for (int i = 0;i < COL_C_xMaxNumberOfCollisions;i++)
  {
    memset(&COL_g_stCollisionCase[i], 0, sizeof(COL_tdstCollisionCase));
  }

  HIE_tdstEngineObject* stdCamObj = GAM_g_stEngineStructure->g_hStdCamCharacter->hLinkedObject.p_stCharacter;

  POS_fn_vSetIdentityMatrix(GAM_g_stEngineStructure->g_hStdCamCharacter->p_stGlobalMatrix);
  POS_fn_vSetIdentityMatrix(GAM_g_stEngineStructure->g_hStdCamCharacter->p_stLocalMatrix);

  const char* src = GAM_fn_p_szGetLevelName();
  memcpy(GAM_g_stEngineStructure->szLevelName, DR_recording.firstLevelName, sizeof(GAM_g_stEngineStructure->szLevelName) - 1);

  FullResetCineInfo(stdCamObj->hCineInfo);

  // Clear the data of the Module Allowing the Communication of Datas from the Player or the Intelligence to the Dynamics
  memset(&stdCamObj->hDynam->p_stDynamics->stDynamicsComplex.stExternalDatas, 0, sizeof(DNM_tdstMACDPID));

  GAM_fn_vReinitTheMap(); // Resets a bunch of stuff including the camera
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
  DR_recording.firstLevelName[0] = '\0';
}

void DR_Recording_SaveProgress() {

  // Prevent progress being frozen by cheat
  g_DR_Cheats_FreezeProgress = FALSE;
  tdstDsgVarArray* array = ACT_DsgVarPtr(g_DR_global->hLinkedObject.p_stActor, DV_GLOBAL_GLOBAL_Bits);

  if (array) {

    for (int i = 0;i < GLOBAL_BITS_ARRAYSIZE;i++) {

      if (i >= array->ucMaxSize) break;

      DR_recording.savedProgress[i] = array->d_ArrayElement[i].ulValue;
    }
  }

  DR_recording.hitPoints = g_DR_rayman->hLinkedObject.p_stActor->hStandardGame->ucHitPoints;
  DR_recording.hitPointsMax = g_DR_rayman->hLinkedObject.p_stActor->hStandardGame->ucHitPointsMax;
  DR_recording.hitPointsMaxMax = g_DR_rayman->hLinkedObject.p_stActor->hStandardGame->ucHitPointsMaxMax;
}

void DR_Recording_LoadProgress() {
  
  // Prevent progress being frozen by cheat
  g_DR_Cheats_FreezeProgress = FALSE;

  tdstDsgVarArray* array = ACT_DsgVarPtr(g_DR_global->hLinkedObject.p_stActor, DV_GLOBAL_GLOBAL_Bits);

  if (array) {

    for (int i = 0;i < GLOBAL_BITS_ARRAYSIZE;i++) {

      if (i >= array->ucMaxSize) break;

      array->d_ArrayElement[i].ulValue = DR_recording.savedProgress[i];
    }
  }

  g_DR_rayman->hLinkedObject.p_stActor->hStandardGame->ucHitPoints = DR_recording.hitPoints;
  g_DR_rayman->hLinkedObject.p_stActor->hStandardGame->ucHitPointsMax = DR_recording.hitPointsMax;
  g_DR_rayman->hLinkedObject.p_stActor->hStandardGame->ucHitPointsMaxMax = DR_recording.hitPointsMaxMax;
}

void DR_Recording_SeekTo(unsigned long frameNum) {

  if (DR_recording_state == DR_IR_State_Idle) {
    DR_recording_state = DR_IR_State_Playback;
  } else if (DR_recording_state != DR_IR_State_Playback && DR_recording_state != DR_IR_State_Recording) {
    return;
  }

  if (frameNum < 0) {
    frameNum = 0;
  }
  if (frameNum >= DR_recording.ulNumFrames) {
    frameNum = DR_recording.ulNumFrames - 1;
  }

  DR_recording_seekTarget = frameNum;
  DR_recording_stateAfterSeek = DR_recording_state;
  DR_recording_state = DR_IR_State_StartSeeking;
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

// Called whenever playback or seeking is started
void DR_Recording_StartFromBeginning() {

  DR_Recording_ResetInputStructure();

  *IPT_g_cJoystickXcenter = DR_recording.cJoystickXcenter;
  *IPT_g_cJoystickYcenter = DR_recording.cJoystickYcenter;
  *IPT_g_cJoystickXmin = DR_recording.cJoystickXmin;
  *IPT_g_cJoystickYmin = DR_recording.cJoystickYmin;
  *IPT_g_cJoystickXmax = DR_recording.cJoystickXmax;
  *IPT_g_cJoystickYmax = DR_recording.cJoystickYmax;

  DR_recording.ulCurrentFrame = 0;
  DR_recording.pCurrentFrame = DR_recording.pFirstFrame;

  DR_Recording_LoadProgress();
  DR_Recording_ReloadTheMap();
}

// Attempts to playback the current frame, returns false if there are no more frames to play or if not in the correct state
BOOL DR_Recording_PlayBackFrame() {

  if (DR_recording_state != DR_IR_State_Playback && DR_recording_state != DR_IR_State_Seeking) {
    return FALSE;
  }

  GAM_g_stEngineStructure->stEngineTimer = DR_recording.pCurrentFrame->stEngineTimer;
  GAM_g_stEngineStructure->stEngineTimer.ulPauseTime = GAM_g_stEngineStructure->stEngineTimer.ulCurrentTimerCount;
  GAM_g_stEngineStructure->stEngineTimer.stPauseTime.ulHighPart = GAM_g_stEngineStructure->stEngineTimer.stRealTimeCount.ulHighPart;
  GAM_g_stEngineStructure->stEngineTimer.stPauseTime.ulLowPart = GAM_g_stEngineStructure->stEngineTimer.stRealTimeCount.ulLowPart;

  // Load engine clock
  GAM_fn_vLoadEngineClock();

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

  float dist = (diffX * diffX + diffY * diffY + diffZ * diffZ);

  DR_recording_desync = dist;
  
  //printf("Playback frame %lu/%lu, desync=%f, frame: %lu, rayframe: %lu\n", DR_recording.ulCurrentFrame, DR_recording.ulNumFrames, DR_recording_desync, GAM_g_stEngineStructure->stEngineTimer.ulFrameNumber, g_DR_rayman->hLinkedObject.p_stActor->hStandardGame->ulLastTrame);

  if (DR_recording.pCurrentFrame->pNextFrame != NULL) {
    DR_recording.ulCurrentFrame++;
    DR_recording.pCurrentFrame = DR_recording.pCurrentFrame->pNextFrame;
    return TRUE;
  }

  return FALSE; // This is the end
}

void DR_Recording_RecordFrame() {

  if (DR_recording_state != DR_IR_State_Recording) {
    return;
  }

  // Save engine clock each frame
  GAM_fn_vSaveEngineClock();

  DR_InputRecordingFrame* newFrame = malloc(sizeof(DR_InputRecordingFrame));
  newFrame->pNextFrame = NULL;
  newFrame->stRaymanPosition = g_DR_rayman->p_stGlobalMatrix->stPos;
  newFrame->stEngineTimer = GAM_g_stEngineStructure->stEngineTimer;

  for (int i = 0;i < IPT_g_stInputStructure->ulNumberOfEntryElement;i++) {

    if (i >= DR_RECORDING_MAX_ENTRIES) {
      break;
    }

    newFrame->a_stEntries[i].lState = IPT_g_stInputStructure->d_stEntryElementArray[i].lState;
    newFrame->a_stEntries[i].xAnalogicValue = IPT_g_stInputStructure->d_stEntryElementArray[i].xAnalogicValue;

  }
  
  //printf("Recording frame %i: x:%f, y:%f\n", DR_recording.ulCurrentFrame, newFrame->a_stEntries[0].xAnalogicValue, newFrame->a_stEntries[1].xAnalogicValue);

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

void DR_Recording_StartPlayback() {

  DR_recording_state = DR_IR_State_StartPlayback;
}

void DR_Recording_Save() {
  DR_RecordingFile_Save("recording.bin", &DR_recording);
}

void DR_Recording_Load() {
  DR_RecordingFile_Load("recording.bin", &DR_recording);
  DR_Recording_LoadProgress(); // Load progress from the recording file
}

// Hooked (HK) functions

void DR_Recording_HK_fn_vComputeRandomTable() {

  if (DR_recording_state != DR_IR_State_Recording &&
    DR_recording_state != DR_IR_State_Playback &&
    DR_recording_state != DR_IR_State_Seeking) {

    printf("Regular random table computation\n");

    RND_fn_vComputeRandomTable();
    return;
  }

  printf("Predictable random table computation\n");

  // Consistent seed when recording/playbacking
  my_srand(DR_recording.ulSeed);

  RND_g_stRandomStructure->ulMaxValueInTable = 0;
  for (long i = 0;i < RND_g_stRandomStructure->ulSizeOfTable;i++)
  {
    RND_g_stRandomStructure->p_ulTable[i] = my_rand() % 0x7FFF; // Random table uses 15-bit entries
  }
  RND_fn_vRemapRandomTable();
}

void DR_Recording_HK_fn_vActualizeEngineClock() {

  // When playbacking or seeking, we override the engine clock
  // (Yes, StartPlayback and StartRecording are also important here!)
  if (DR_recording_state != DR_IR_State_Idle && DR_recording_state != DR_IR_State_Recording) {
    return;
  }

  GAM_fn_vActualizeEngineClock();

  return;
}

BOOL DR_Recording_HK_bFlipDeviceWithSynchro() {
  if (DR_recording_state == DR_IR_State_Seeking) {
    // When seeking, skip this synchronisation
    return TRUE;
  }

  return GLD_bFlipDeviceWithSynchro();
}

void DR_Recording_HK_fn_vSendStaticWorldToViewport(GLD_tdstViewportAttributes* _p_stVpt, HIE_tdstSuperObject* _hSprObjSector, long _lDrawMask, long _lCullingResult) {

  if (DR_recording_state == DR_IR_State_Seeking) {
    return;
  }
  return HIE_fn_vSendStaticWorldToViewport(_p_stVpt, _hSprObjSector, _lDrawMask, _lCullingResult);
}

void DR_Recording_HK_fn_vSendCharacterModulesToViewPort(GLD_tdstViewportAttributes* _hVpt, HIE_tdstSuperObject* _hSprObj, long _DrawMask) {

  if (DR_recording_state == DR_IR_State_Seeking) {
    // When seeking, don't bother sending character graphics to the viewport
    return;
  }
  return HIE_fn_vSendCharacterModulesToViewPort(_hVpt, _hSprObj, _DrawMask);
}

long DR_Recording_HK_fn_lSendRequestSound(long lIndice, long lType, SND_tduRefEvt uEvt, long lPrio, SND_td_pfn_vSoundCallback pfnProc) {
  
  if (DR_recording_state == DR_IR_State_Seeking) {
    // When seeking, don't bother playing sounds
    return -2;
  }

  return SND_fn_lSendRequestSound(lIndice, lType, uEvt, lPrio, pfnProc);
}

void DR_Recording_HK_fn_vSynchroSound() {

  if (DR_recording_state == DR_IR_State_Seeking) {
    // When seeking, don't bother playing sounds
    return;
  }

  return SND_fn_vSynchroSound();
}

void DR_Recording_HK_fn_vReadInput()
{
  // Main function for recording and replaying
  DR_Bruteforce_Update();

  switch (DR_recording_state) {
  case DR_IR_State_Recording:

    if (GAM_g_stEngineStructure->bEngineIsInPaused) {
      return;
    }

    if (DR_recording.ulCurrentFrame == 0) {
      DR_Recording_SaveProgress();
    }

    IPT_fn_vReadInput();
    DR_Recording_RecordFrame();
    break;
  case DR_IR_State_Seeking:

    if (!DR_Recording_PlayBackFrame() || DR_recording.ulCurrentFrame >= DR_recording_seekTarget) {
      DR_recording_state = DR_recording_stateAfterSeek;
      //GAM_g_stEngineStructure->bEngineIsInPaused = DR_recording_pauseAfterSeek;
    }

    break;
  case DR_IR_State_StartSeeking:

    /* If recording, truncate tail so new length == frameNum */
    if (DR_recording_stateAfterSeek == DR_IR_State_Recording) {

      /* If keeping everything, nothing to truncate */
      if (DR_recording_seekTarget < DR_recording.ulNumFrames) {

        DR_InputRecordingFrame* cur = DR_recording.pFirstFrame;
        DR_InputRecordingFrame* prev = NULL;
        unsigned long i = 0;

        /* Walk to first frame that should be removed */
        while (cur && i < DR_recording_seekTarget) {
          prev = cur;
          cur = cur->pNextFrame;
          i++;
        }

        /* Detach list */
        if (prev) {
          prev->pNextFrame = NULL;
        }
        else {
          /* Removing entire list */
          DR_recording.pFirstFrame = NULL;
        }

        /* Free removed tail */
        while (cur) {
          DR_InputRecordingFrame* next = cur->pNextFrame;
          free(cur);
          cur = next;
        }

        /* Update metadata */
        DR_recording.ulNumFrames = DR_recording_seekTarget;
        DR_recording.pLastFrame = prev;
        DR_recording.pCurrentFrame = prev;
      }
    }

    if (DR_recording_seekTarget <= DR_recording.ulCurrentFrame) {
      DR_Recording_StartFromBeginning();
    }

    DR_recording_state = DR_IR_State_Seeking;

    break;
  case DR_IR_State_Playback:

    if (GAM_g_stEngineStructure->bEngineIsInPaused) {
      return;
    }

    // Attempt to playback a frame, if there are no more frames to playback then stop
    if (!DR_Recording_PlayBackFrame()) {
      DR_recording_state = DR_IR_State_Idle;
    }

    break;
  case DR_IR_State_StartRecording:

    DR_Recording_Cleanup();
    DR_Recording_ResetInputStructure();

    g_DR_Cheats_FreezeProgress = FALSE;

    memcpy(DR_recording.firstLevelName, GAM_g_stEngineStructure->szLevelName, sizeof(DR_recording.firstLevelName)-1);

    DR_recording.cJoystickXcenter = *IPT_g_cJoystickXcenter;
    DR_recording.cJoystickYcenter = *IPT_g_cJoystickYcenter;
    DR_recording.cJoystickXmin = *IPT_g_cJoystickXmin;
    DR_recording.cJoystickYmin = *IPT_g_cJoystickYmin;
    DR_recording.cJoystickXmax = *IPT_g_cJoystickXmax;
    DR_recording.cJoystickYmax = *IPT_g_cJoystickYmax;
    
    DR_Recording_ReloadTheMap();

    DR_recording_state = DR_IR_State_Recording;

    break;
  case DR_IR_State_StartPlayback:

    DR_Recording_StartFromBeginning();

    DR_recording_state = DR_IR_State_Playback;

    break;
  case DR_IR_State_Idle:

    IPT_fn_vReadInput();
    break;
  }
}