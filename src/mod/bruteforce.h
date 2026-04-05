#pragma once
#include <ACP_Ray2.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef struct DR_tdstBruteforceSettings {
  BOOL active;
  unsigned int currentAttempt;
  unsigned int maxAttempts;
  unsigned int skipFrames;
  MTH3D_tdstVector targetPosition;
  BOOL ignoreZ;
  float targetDistance;
  float currentBest;
  float analogRandomness;
} DR_tdstBruteforceSettings;


extern DR_tdstBruteforceSettings DR_bruteforceSettings;

void DR_Bruteforce_Start();
void DR_Bruteforce_Update();


#ifdef __cplusplus
    }
#endif
