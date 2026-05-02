#pragma once
#include <ACP_Ray2.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef struct DR_tdstBruteforceSettings {
  BOOL active;
  unsigned int currentAttempt;
  unsigned int maxAttempts;
  unsigned int randomizeStart;
  unsigned int randomizeEnd;
  BOOL randomizeRange;
  HIE_tdstSuperObject* targetObject;
  MTH3D_tdstVector targetPosition;
  BOOL ignoreZ;
  int dsgVarID;
  BOOL invertScore;
  float targetScore;
  float currentBestScore;
  float lastAttemptScore;
  float currentScore;
  int currentBestFrame;
  float analogRandomness;
  float analogModChance;
  float inputLengthModChance;
  BOOL disqualifyTouchingWalls;
  BOOL disqualifyFalling;
  BOOL stopOnMapChange;

} DR_tdstBruteforceSettings;


extern DR_tdstBruteforceSettings DR_bruteforceSettings;

// Called from UI
void DR_Bruteforce_Signal_Start();
void DR_Bruteforce_Signal_Stop();

void DR_Bruteforce_Update();
void DR_Bruteforce_OnMapChange();


#ifdef __cplusplus
    }
#endif
