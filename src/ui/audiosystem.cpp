#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "AudioSystem.hpp"
#include <vector>

namespace AudioSystem
{
  static ma_engine gEngine{};
  static bool gEngineInit = false;

  static HINSTANCE gInstance = nullptr;

  // Keep ownership of sounds created from resources
  static std::vector<ma_sound*> gSounds;

  bool Initialize(HINSTANCE hInstance)
  {
    if (gEngineInit)
      return true;

    gInstance = hInstance;

    ma_result result = ma_engine_init(nullptr, &gEngine);
    if (result != MA_SUCCESS)
      return false;

    gEngineInit = true;
    return true;
  }

  void Shutdown()
  {
    FreeAllSounds();

    if (gEngineInit)
    {
      ma_engine_uninit(&gEngine);
      gEngineInit = false;
    }
  }

  ma_engine* GetEngine()
  {
    return gEngineInit ? &gEngine : nullptr;
  }

  ma_sound* LoadSoundFromResource(UINT resourceID, bool startImmediately)
  {
    if (!gEngineInit || gInstance == nullptr)
      return nullptr;

    HRSRC res = FindResource(gInstance, MAKEINTRESOURCE(resourceID), L"WAVE");
    if (!res) return nullptr;

    HGLOBAL resHandle = LoadResource(gInstance, res);
    if (!resHandle) return nullptr;

    DWORD resSize = SizeofResource(gInstance, res);
    void* resData = LockResource(resHandle);
    if (!resData || resSize == 0) return nullptr;

    ma_decoder_config config = ma_decoder_config_init(ma_format_unknown, 0, 0);
    ma_decoder* decoder = new ma_decoder();

    if (ma_decoder_init_memory(resData, resSize, &config, decoder) != MA_SUCCESS)
    {
      delete decoder;
      return nullptr;
    }

    ma_sound* sound = new ma_sound();
    if (ma_sound_init_from_data_source(&gEngine, decoder, 0, nullptr, sound) != MA_SUCCESS)
    {
      ma_decoder_uninit(decoder);
      delete decoder;
      delete sound;
      return nullptr;
    }

    // Store so we can clean later
    gSounds.push_back(sound);

    if (startImmediately)
      ma_sound_start(sound);

    return sound;
  }

  void FreeAllSounds()
  {
    for (auto* sound : gSounds)
    {
      if (sound)
      {
        ma_decoder* decoder = (ma_decoder*)sound->pDataSource;
        ma_sound_uninit(sound);

        if (decoder)
          ma_decoder_uninit(decoder);

        delete decoder;
        delete sound;
      }
    }

    gSounds.clear();
  }
}
