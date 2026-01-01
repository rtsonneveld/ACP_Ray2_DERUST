#ifndef AUDIO_SYSTEM_H
#define AUDIO_SYSTEM_H

#include "miniaudio.h"
#include <windows.h>
#include <unordered_map>
#include <string>

// -----------------------------------------------------------------------------
//  Simple wrapper for global audio engine + resource-backed WAV loading
// -----------------------------------------------------------------------------
namespace AudioSystem
{
  // Call once at startup
  bool Initialize(HINSTANCE hInstance);

  // Call once at shutdown
  void Shutdown();

  // Access the global engine (read-only)
  ma_engine* GetEngine();

  // Load a WAV from a resource ID (returns nullptr on failure)
  // Each resource must have type "WAVE"
  ma_sound* LoadSoundFromResource(UINT resourceID, bool startImmediately = false);

  // Stop & free all sounds and internal storage
  void FreeAllSounds();
}

#endif // AUDIO_SYSTEM_H
