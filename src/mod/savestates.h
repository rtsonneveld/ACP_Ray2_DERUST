#pragma once
#include <stdint.h>
#include <stddef.h>
#include <windows.h>
#include <ACP_Ray2.h>

#ifdef __cplusplus
extern "C" {
#endif

// Hooked allocation functions
void* MOD_fn_vGenAlloc(unsigned long Size, unsigned long ChannelId);
void* MOD_fn_vDynAlloc(unsigned long Size);
size_t MOD_fn_ulFRead(void* _p_vBuffer, size_t _ulSize, size_t _ulCount, void* _p_stFile);
void MOD_fn_v_InitMmg(unsigned char ucModuleId, unsigned char ucMaxBlocksNb, unsigned long ulMaxNbDynamicMalloc);
void MOD_fn_vInitSpecificBlock(unsigned char _ucBlockId, unsigned char _ucModuleId,
  tdstBlockInfoPriv* _p_stMyBlocksInfo, unsigned long _ulSize, unsigned long _ulMaxNbStaticMalloc,
  unsigned char _ucAlignment, unsigned char _ucMode);
void setDebugAddress(void* addr);

void SS_SaveState();
void SS_LoadState();


#ifdef __cplusplus
}
#endif
