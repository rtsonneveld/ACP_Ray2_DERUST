#include "bruteforce.h"
#include "recording.h"
#include "recordingfile.h"
#include "globals.h"
#include "float.h"
#include <stdlib.h>
#include "state.h"
#include <math.h>
#include <windows.h>

#define FILE_BRUTEFORCE_BEST "bruteforcebest.bin"

DR_tdstBruteforceSettings DR_bruteforceSettings = {
  .active = FALSE,
  .currentAttempt = 0,
  .maxAttempts = 100,
  .randomizeStart = 0,
  .randomizeEnd = 0,
  .randomizeRange = 0,
  .targetObject = NULL,
  .targetPosition = {0},
  .ignoreZ = FALSE,
  .dsgVarID = -1,
  .targetScore = 0,
  .currentBestScore = -FLT_MAX,
  .lastAttemptScore = -FLT_MAX,
  .currentScore = -FLT_MAX,
  .currentBestFrame = 0,
  .analogRandomness = 1,
  .analogModChance = 100,
  .inputLengthModChance = 1,
  .disqualifyTouchingWalls = FALSE,
  .disqualifyFalling = FALSE
};

BOOL isBestAttempt = FALSE;
float currentAttemptBestScore = 0.0f;

volatile long signalStop = FALSE;
volatile long signalStart = FALSE;

float RandomFloat(float min, float max) {
  return min + (max-min) * ((float)rand() / (float)RAND_MAX);
}

float RandomizeFloat(float input, float strength) {
  float offset = ((float)rand() / (float)RAND_MAX) * 2.0f * strength - strength;

  float result = input + offset;

  if (result > 100.0f) return 100.0f;
  if (result < -100.0f) return -100.0f;

  return result;
}

BOOL shouldRandomizeFrame(int frame, int rangeStart, int rangeEnd) {
  return frame >= rangeStart && frame < rangeEnd;
}

void DR_Bruteforce_KeepBestAttempt() {
  if (isBestAttempt) {
    DR_RecordingFile_Save(FILE_BRUTEFORCE_BEST, &DR_recording);
    printf("Saving best attempt");
  }
  else {
    DR_RecordingFile_Load(FILE_BRUTEFORCE_BEST, &DR_recording);
    DR_Recording_LoadProgress(); // Load progress from the recording file
    printf("Loading best attempt");
  }

  isBestAttempt = FALSE;
  DR_bruteforceSettings.lastAttemptScore = currentAttemptBestScore;
  currentAttemptBestScore = -FLT_MAX;
}

void DR_Bruteforce_Start() {

  g_DR_Playback.unpause = TRUE;

  DR_bruteforceSettings.currentAttempt = 0;
  DR_bruteforceSettings.active = TRUE;
  DR_bruteforceSettings.currentBestScore = -FLT_MAX;
  DR_bruteforceSettings.lastAttemptScore = -FLT_MAX;
  DR_bruteforceSettings.currentBestFrame = 0;

  isBestAttempt = TRUE;
  DR_Bruteforce_KeepBestAttempt();

  isBestAttempt = FALSE;

  DR_Recording_SeekTo(DR_recording.ulNumFrames, FALSE, TRUE);
}

void DR_Bruteforce_Stop() {

  DR_Bruteforce_KeepBestAttempt();

  DR_bruteforceSettings.active = FALSE;
 
  DR_Recording_StopSeeking(); 

  DR_Recording_SeekTo(DR_bruteforceSettings.currentBestFrame, TRUE, TRUE);
}

void RandomizeFrame(DR_InputRecordingFrame* frame, DR_InputRecordingFrame* previousFrame, DR_InputRecordingFrame* nextFrame, int i)
{

  if (RandomFloat(0.0f, 1.0f) < DR_bruteforceSettings.analogModChance / 100.0f) {
    frame->a_stEntries[0].xAnalogicValue = RandomizeFloat(frame->a_stEntries[0].xAnalogicValue, DR_bruteforceSettings.analogRandomness);
    frame->a_stEntries[1].xAnalogicValue = RandomizeFloat(frame->a_stEntries[1].xAnalogicValue, DR_bruteforceSettings.analogRandomness);
  }

  // Only randomize inputs after the first frame and before the last frame
  if (i > 1 && i < DR_recording.ulNumFrames - 1) {

    for (int entry = 2; entry < DR_RECORDING_MAX_ENTRIES; entry++) {
      
      if (RandomFloat(0.0f, 1.0f) > DR_bruteforceSettings.inputLengthModChance / 100.0f) continue;

      if (frame->a_stEntries[entry].lState == 1) { // Just pressed
        if (rand() % 2 == 0) { // Delay press by one frame
          frame->a_stEntries[entry].lState = previousFrame->a_stEntries[entry].lState - 1;
          nextFrame->a_stEntries[entry].lState = 1;
        } else { // Make press happen one frame earlier
          frame->a_stEntries[entry].lState = 2;
          previousFrame->a_stEntries[entry].lState = 1;
        }
      } else if (frame->a_stEntries[entry].lState == -11) { // Just released
        if (rand() % 2 == 0) { // Delay release by one frame
          frame->a_stEntries[entry].lState = previousFrame->a_stEntries[entry].lState + 1;
          nextFrame->a_stEntries[entry].lState = -1;
        } else { // Make release happen one frame earlier
          frame->a_stEntries[entry].lState = -2;
          previousFrame->a_stEntries[entry].lState = -1;
        }
      }

    }
  }
}

void DR_Bruteforce_Signal_Start() {
  InterlockedExchange(&signalStart, 1);
}

void DR_Bruteforce_Signal_Stop() {
  InterlockedExchange(&signalStop, 1);
}

float DR_Bruteforce_Heuristic() {

  HIE_tdstSuperObject* targetObject = DR_bruteforceSettings.targetObject != NULL ? DR_bruteforceSettings.targetObject : GAM_g_stEngineStructure->g_hMainActor;
  if (targetObject == NULL || targetObject->hLinkedObject.p_stActor->hStandardGame == NULL) {
    return -FLT_MAX;
  }
  MTH3D_tdstVector currentPos = targetObject->p_stGlobalMatrix->stPos;
  if (DR_bruteforceSettings.dsgVarID >= 0) {

    void* dsgVarPtr = ACT_DsgVarPtr(targetObject->hLinkedObject.p_stActor, DR_bruteforceSettings.dsgVarID);
    if (dsgVarPtr == NULL) {
      return -FLT_MAX;
    }
    currentPos = *(MTH3D_tdstVector*)dsgVarPtr;
  }

  MTH3D_tdstVector targetPos = DR_bruteforceSettings.targetPosition;
  float xDiff = (currentPos.x - targetPos.x);
  float yDiff = (currentPos.y - targetPos.y);
  float zDiff = DR_bruteforceSettings.ignoreZ ? 0 : (currentPos.z - targetPos.z);
  float distSq = (xDiff * xDiff + yDiff * yDiff + zDiff * zDiff);
  float result = -sqrtf(distSq);

  if (DR_bruteforceSettings.invertScore) {
    return -result;
  }
  return result;
}

BOOL DR_Bruteforce_CheckDisqualification() {

  if (DR_recording.ulCurrentFrame < DR_bruteforceSettings.randomizeStart) {
    return FALSE;
  }

  if (DR_bruteforceSettings.disqualifyTouchingWalls) {
    if ((g_DR_rayman->hLinkedObject.p_stActor->hDynam->p_stDynamics->stDynamicsBase.p_stReport->ulCurrSurfaceState & MEC_C_WOT_ulWall) != 0) {
      return TRUE;
    }
  }

  if (DR_bruteforceSettings.disqualifyFalling) {
    if ((g_DR_rayman->hLinkedObject.p_stActor->hDynam->p_stDynamics->stDynamicsBase.p_stReport->ulCurrSurfaceState & MEC_C_WOT_ulGround) == 0) {
      return TRUE;
    }
  }

  return FALSE;
}

void DR_Bruteforce_OnMapChange() {

  if (!DR_bruteforceSettings.active) {
    return;
  }

  if (!DR_bruteforceSettings.stopOnMapChange) {
    return;
  }

  DR_bruteforceSettings.currentBestScore = FLT_MAX;
  DR_bruteforceSettings.lastAttemptScore = FLT_MAX;
  DR_bruteforceSettings.currentBestFrame = DR_recording.ulCurrentFrame-1;
  currentAttemptBestScore = FLT_MAX;
  isBestAttempt = TRUE;

  DR_Bruteforce_Stop();
  DR_Bruteforce_KeepBestAttempt();
}

void DR_Bruteforce_Update() {
  if (InterlockedExchange(&signalStop, 0) == 1) {
    DR_Bruteforce_Stop();
  }
  if (InterlockedExchange(&signalStart, 0) == 1) {
    DR_Bruteforce_Start();
  }

  float score = DR_Bruteforce_Heuristic();

  if (score > DR_bruteforceSettings.targetScore) {
    score = 100000.0f - DR_recording.ulCurrentFrame; // If we beat the target score, prioritize attempts that do it faster
  }

  BOOL disqualified = DR_Bruteforce_CheckDisqualification();
  if (disqualified) {
    score = -FLT_MAX;
    isBestAttempt = FALSE;
    DR_Recording_StopSeeking(); // Stop seeking immediately if disqualified
  }

  DR_bruteforceSettings.currentScore = score;

  if (!DR_bruteforceSettings.active) {
    return;
  }
  if (DR_bruteforceSettings.currentAttempt >= DR_bruteforceSettings.maxAttempts) {
    DR_Bruteforce_Stop();

    return;
  }
  
  if (DR_Recording_CurrentState() == DR_IR_State_Seeking && DR_recording.ulCurrentFrame >= DR_bruteforceSettings.randomizeStart) {

    if (score > DR_bruteforceSettings.currentBestScore) {
      DR_bruteforceSettings.currentBestScore = score;
      DR_bruteforceSettings.currentBestFrame = DR_recording.ulCurrentFrame;
      isBestAttempt = TRUE;
    }
    if (score > currentAttemptBestScore) {
      currentAttemptBestScore = score;
    }
  }

  if (DR_Recording_CurrentState() != DR_IR_State_Idle) {
    return;
  }

  DR_Bruteforce_KeepBestAttempt();

  // Start modifying the replay after the first attempt to get a baseline
  if (DR_bruteforceSettings.currentAttempt > 1) {
    DR_InputRecordingFrame* frame = DR_recording.pFirstFrame;
    DR_InputRecordingFrame* nextFrame = DR_recording.pFirstFrame->pNextFrame;
    DR_InputRecordingFrame* previousFrame = NULL;

    int originalStart = DR_bruteforceSettings.randomizeStart;
    int originalEnd = (DR_bruteforceSettings.randomizeEnd == 0) ? DR_recording.ulNumFrames : DR_bruteforceSettings.randomizeEnd;

    int rangeStart = originalStart;
    int rangeEnd = originalEnd;

    if (DR_bruteforceSettings.randomizeRange && (originalEnd > originalStart)) {
      int length = originalEnd - originalStart;

      int r1 = originalStart + (rand() % (length + 1));
      int r2 = originalStart + (rand() % (length + 1));

      rangeStart = min(r1, r2);
      rangeEnd = max(r1, r2);
    }

    for (int i = 0;i < DR_recording.ulNumFrames;i++) {

      if (shouldRandomizeFrame(i, rangeStart, rangeEnd)) {
        RandomizeFrame(frame, previousFrame, nextFrame, i);
      }

      previousFrame = frame;
      frame = frame->pNextFrame;
      if (frame != NULL && frame->pNextFrame != NULL) {
        nextFrame = frame->pNextFrame;
      }
    }
  }

  DR_Recording_SeekTo(DR_recording.ulNumFrames, FALSE, TRUE);

  DR_bruteforceSettings.currentAttempt++;
}
