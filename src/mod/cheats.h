#pragma once
#ifdef __cplusplus
extern "C" {
#endif

// BEWARE: these are 0-indexed offsets while the original scripts use 1-indexed offsets. So subtract 1 from the original script values.
#define GB_FLAG_MEGASHOOT_HIBIT 961
#define GB_FLAG_MEGASHOOT_LOBIT 962
#define GB_FLAG_HANGON 1094
#define GB_FLAG_GLOWFIST 1142
#define GB_FLAG_NOMOVIES 1187
#define GB_FLAG_THINK 1398

#define GB_FLAG_HUB_1_WOODSOFLIGHT                 959
#define GB_FLAG_HUB_2_FAIRYGLADE                   960
#define GB_FLAG_HUB_3_MARSHES                      963
#define GB_FLAG_HUB_4_CAVEOFBADDREAMS              965
#define GB_FLAG_HUB_5_BAYOU                        966
#define GB_FLAG_HUB_6_WALKOFLIFE                   968
#define GB_FLAG_HUB_7_WATERANDICE                  969
#define GB_FLAG_HUB_8_MENHIRHILLS                  971
#define GB_FLAG_HUB_9_ECHOINGCAVES                 974
#define GB_FLAG_HUB_10_CANOPY                      975
#define GB_FLAG_HUB_11_WHALEBAY                    978
#define GB_FLAG_HUB_12_STONEANDFIRE                980
#define GB_FLAG_HUB_13_PRECIPICE                   984
#define GB_FLAG_HUB_14_TOPOFTHEWORLD               987
#define GB_FLAG_HUB_15_SANCTUARY_ROCKANDLAVA       989
#define GB_FLAG_HUB_16_WALKOFPOWER                 991
#define GB_FLAG_HUB_17_BENEATH_SANCTUARY_ROCKLAVA  992
#define GB_FLAG_HUB_18_IRONMOUNTAINS               999
#define GB_FLAG_HUB_19_PRISONSHIP                  1001
#define GB_FLAG_HUB_20_CROWSNEST                   1004
#define GB_FLAG_HUB_21_WIPE10_ASTRO20_UNUSED       1005
#define GB_FLAG_HUB_22_TOMBOFTHEANCIENTS           1006


extern char g_DR_Cheats_InfiniteHealth;
extern char g_DR_Cheats_MegaShoots;
extern char g_DR_Cheats_DisableStartingCutscenes;

void DisableObjectOfModelType();

void DR_Cheats_Apply();
char DR_Cheats_GetFlag(int index);
void DR_Cheats_ToggleFlag(int index);
void DR_Cheats_SetFlag(int index, char state);

#ifdef __cplusplus
    }
#endif