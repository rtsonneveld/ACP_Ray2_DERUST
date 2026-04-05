#include "bruteforce.h"
#include "recording.h"
#include "recordingfile.h"
#include "globals.h"
#include <stdlib.h>
#include "tryblock.h"

#define FILE_BRUTEFORCE_BEST "bruteforcebest.bin"

DR_tdstBruteforceSettings DR_bruteforceSettings = { 0 };
BOOL isBestAttempt = FALSE;

float RandomizeFloat(float input, float strength) {
  float offset = ((float)rand() / (float)RAND_MAX) * 2.0f * strength - strength;

  float result = input + offset;

  if (result > 100.0f) return 100.0f;
  if (result < -100.0f) return -100.0f;

  return result;
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
}

void DR_Bruteforce_Start() {

  DR_bruteforceSettings.currentAttempt = 0;
  DR_bruteforceSettings.active = TRUE;
  DR_bruteforceSettings.currentBest = 0;

  isBestAttempt = TRUE;
  DR_Bruteforce_KeepBestAttempt();

  isBestAttempt = FALSE;
}

void DR_Bruteforce_Update() {

  if (!DR_bruteforceSettings.active) {
    return;
  }
  if (DR_bruteforceSettings.currentAttempt >= DR_bruteforceSettings.maxAttempts) {
    DR_bruteforceSettings.active = FALSE;
    printf("Bruteforce complete, reached max attempts (%u)\n", DR_bruteforceSettings.maxAttempts);

    DR_Bruteforce_KeepBestAttempt();

    return;
  }
  
  if (DR_Recording_CurrentState() == DR_IR_State_Seeking && DR_recording.ulCurrentFrame > DR_bruteforceSettings.skipFrames) {
    
    MTH3D_tdstVector rayPos = g_DR_rayman->p_stGlobalMatrix->stPos;
    MTH3D_tdstVector targetPos = DR_bruteforceSettings.targetPosition;
    float xDiff = (rayPos.x - targetPos.x);
    float yDiff = (rayPos.y - targetPos.y);
    float zDiff = DR_bruteforceSettings.ignoreZ ? 0 : (rayPos.z - targetPos.z);

    float distSq = (xDiff * xDiff + yDiff * yDiff + zDiff * zDiff);
    if (DR_bruteforceSettings.currentBest == 0 || distSq < DR_bruteforceSettings.currentBest) {
      DR_bruteforceSettings.currentBest = distSq;
      isBestAttempt = TRUE;
    }

    if (distSq < DR_bruteforceSettings.targetDistance * DR_bruteforceSettings.targetDistance) {
        DR_bruteforceSettings.active = FALSE;
        printf("Bruteforce complete, target reached in attempt %u (distance %.2f)\n", DR_bruteforceSettings.currentAttempt, sqrtf(distSq));
        DR_Bruteforce_KeepBestAttempt();
        return;
    }
  }

  if (DR_Recording_CurrentState() != DR_IR_State_Idle) {
    return;
  }

  DR_Bruteforce_KeepBestAttempt();

  // Start modifying the replay after the first attempt to get a baseline
  if (DR_bruteforceSettings.currentAttempt > 1) {
    DR_InputRecordingFrame* frame = DR_recording.pFirstFrame;
    for (int i = 0;i < DR_recording.ulNumFrames;i++) {

      if (i < DR_bruteforceSettings.skipFrames) {
        frame = frame->pNextFrame;
        continue;
      }

      frame->a_stEntries[0].xAnalogicValue = RandomizeFloat(frame->a_stEntries[0].xAnalogicValue, DR_bruteforceSettings.analogRandomness);
      frame->a_stEntries[1].xAnalogicValue = RandomizeFloat(frame->a_stEntries[1].xAnalogicValue, DR_bruteforceSettings.analogRandomness);

      frame = frame->pNextFrame;
    }
  }

  DR_Recording_SeekTo(DR_recording.ulNumFrames);

  DR_bruteforceSettings.currentAttempt++;
}