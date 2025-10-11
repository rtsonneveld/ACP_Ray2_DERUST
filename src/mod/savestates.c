#include "savestates.h"
#include <ACP_Ray2.h>
#include "globals.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>

const char* modules[] = {
    "ERM",
    "MMG",
    "GMT",
    "SCR",
    "GAM",
    "GEO",
    "IPT",
    "RND",
    "CMP",
    "SAI",
    "TMP",
    "FIL",
    "VIG",
    "AI",
    "FON",
    "GLD",
    "TMR",
    "SND",
};

void MOD_fn_v_InitMmg(unsigned char ucModuleId, unsigned char ucMaxBlocksNb, unsigned long ulMaxNbDynamicMalloc) {

  Mmg_fn_v_InitMmg(ucModuleId, ucMaxBlocksNb, ulMaxNbDynamicMalloc);
}

void MOD_fn_vInitSpecificBlock(unsigned char _ucBlockId, unsigned char _ucModuleId,
  tdstBlockInfoPriv* _p_stMyBlocksInfo, unsigned long _ulSize, unsigned long _ulMaxNbStaticMalloc,
  unsigned char _ucAlignment, unsigned char _ucMode) {
  Mmg_fn_vInitSpecificBlock(_ucBlockId, _ucModuleId, _p_stMyBlocksInfo, _ulSize, _ulMaxNbStaticMalloc, _ucAlignment, _ucMode);
}

void* MOD_fn_vGenAlloc(unsigned long Size, unsigned long ChannelId) {

  void* p = fn_p_vGenAlloc(Size, ChannelId);
  return p;
}

void* MOD_fn_vDynAlloc(unsigned long Size) {

  void* p = fn_p_vDynAlloc(Size);
  return p;
}

size_t MOD_fn_ulFRead(void* _p_vBuffer, size_t _ulSize, size_t _ulCount, void* _p_stFile) {
  size_t* p = SNA_fn_ulFRead(_p_vBuffer, _ulSize, _ulCount, _p_stFile);
  return p;
}

// --- Snapshot save/load ---
void SS_SaveState() {
}

void SS_LoadState() {
}
