#include "menu_cheats.hpp"
#include <mod/cheats.h>
#include <imgui.h>
#include <ACP_Ray2.h>

void FlagMenuItem(const char* label, int index)
{
  if (ImGui::MenuItem(label, nullptr, DR_Cheats_GetFlag(index))) {
    DR_Cheats_ToggleFlag(index);
  }
}

void DR_DLG_Cheats_Draw() {
  int a = 0;
  ImGui::SeparatorText("Cheats");
  if (ImGui::MenuItem("Infinite Health", nullptr, g_DR_Cheats_InfiniteHealth)) {
    g_DR_Cheats_InfiniteHealth = !g_DR_Cheats_InfiniteHealth;
  }
  if (ImGui::MenuItem("Megashoot (golden fists)", nullptr, g_DR_Cheats_MegaShoots)) {
    g_DR_Cheats_MegaShoots = !g_DR_Cheats_MegaShoots;
  }
  if (ImGui::MenuItem("Disable starting cutscenes", nullptr, g_DR_Cheats_DisableStartingCutscenes)) {
    g_DR_Cheats_DisableStartingCutscenes = !g_DR_Cheats_DisableStartingCutscenes;
  }
  if (ImGui::MenuItem("Disable death animations", nullptr, g_DR_Cheats_DisableDeathAnimations)) {
    g_DR_Cheats_DisableDeathAnimations = !g_DR_Cheats_DisableDeathAnimations;
  }
  if (ImGui::MenuItem("Activate void")) {
    *AI_g_bInGameMenu = TRUE;
  }
  if (ImGui::MenuItem("Automatic perfect void", nullptr, g_DR_Cheats_AutoVoid)) {
    g_DR_Cheats_AutoVoid = !g_DR_Cheats_AutoVoid;
  }
  if (ImGui::MenuItem("Freeze progress", nullptr, g_DR_Cheats_FreezeProgress)) {
    g_DR_Cheats_FreezeProgress = !g_DR_Cheats_FreezeProgress;
  }
  ImGui::SeparatorText("Flags");
  FlagMenuItem("NoMovies", GB_FLAG_NOMOVIES);
  FlagMenuItem("Hangon (grapple)", GB_FLAG_HANGON);
  FlagMenuItem("Glowfist", GB_FLAG_GLOWFIST);
  FlagMenuItem("Think power", GB_FLAG_THINK);
  FlagMenuItem("Watched woods", 1132); 

  if (ImGui::BeginMenu("Hall of Doors")) {

    if (ImGui::SmallButton("Toggle all")) { 

      bool newState = !DR_Cheats_GetFlag(GB_FLAG_HUB_22_TOMBOFTHEANCIENTS);
        
      DR_Cheats_SetFlag(GB_FLAG_HUB_1_WOODSOFLIGHT, newState);
      DR_Cheats_SetFlag(GB_FLAG_HUB_2_FAIRYGLADE, newState);
      DR_Cheats_SetFlag(GB_FLAG_HUB_3_MARSHES, newState);
      DR_Cheats_SetFlag(GB_FLAG_HUB_4_CAVEOFBADDREAMS, newState);
      DR_Cheats_SetFlag(GB_FLAG_HUB_5_BAYOU, newState);
      DR_Cheats_SetFlag(GB_FLAG_HUB_6_WALKOFLIFE, newState);
      DR_Cheats_SetFlag(GB_FLAG_HUB_7_WATERANDICE, newState);
      DR_Cheats_SetFlag(GB_FLAG_HUB_8_MENHIRHILLS, newState);
      DR_Cheats_SetFlag(GB_FLAG_HUB_9_ECHOINGCAVES, newState);
      DR_Cheats_SetFlag(GB_FLAG_HUB_10_CANOPY, newState);
      DR_Cheats_SetFlag(GB_FLAG_HUB_11_WHALEBAY, newState);
      DR_Cheats_SetFlag(GB_FLAG_HUB_12_STONEANDFIRE, newState);
      DR_Cheats_SetFlag(GB_FLAG_HUB_13_PRECIPICE, newState);
      DR_Cheats_SetFlag(GB_FLAG_HUB_14_TOPOFTHEWORLD, newState);
      DR_Cheats_SetFlag(GB_FLAG_HUB_15_SANCTUARY_ROCKANDLAVA, newState);
      DR_Cheats_SetFlag(GB_FLAG_HUB_16_WALKOFPOWER, newState);
      DR_Cheats_SetFlag(GB_FLAG_HUB_17_BENEATH_SANCTUARY_ROCKLAVA, newState);
      DR_Cheats_SetFlag(GB_FLAG_HUB_18_IRONMOUNTAINS, newState);
      DR_Cheats_SetFlag(GB_FLAG_HUB_19_PRISONSHIP, newState);
      DR_Cheats_SetFlag(GB_FLAG_HUB_20_CROWSNEST, newState);
      DR_Cheats_SetFlag(GB_FLAG_HUB_21_WIPE10_ASTRO20_UNUSED, newState);
      DR_Cheats_SetFlag(GB_FLAG_HUB_22_TOMBOFTHEANCIENTS, newState);
    }

    FlagMenuItem("Woods of Light", GB_FLAG_HUB_1_WOODSOFLIGHT);
    FlagMenuItem("Fairy Glade", GB_FLAG_HUB_2_FAIRYGLADE);
    FlagMenuItem("Marshes of Awakening", GB_FLAG_HUB_3_MARSHES);
    FlagMenuItem("Cave of Bad Dreams", GB_FLAG_HUB_4_CAVEOFBADDREAMS);
    FlagMenuItem("Bayou", GB_FLAG_HUB_5_BAYOU);
    FlagMenuItem("Walk of Life", GB_FLAG_HUB_6_WALKOFLIFE);
    FlagMenuItem("Water and Ice", GB_FLAG_HUB_7_WATERANDICE);
    FlagMenuItem("Menhir Hills", GB_FLAG_HUB_8_MENHIRHILLS);
    FlagMenuItem("Echoing Caves", GB_FLAG_HUB_9_ECHOINGCAVES);
    FlagMenuItem("Canopy", GB_FLAG_HUB_10_CANOPY);
    FlagMenuItem("Whale Bay", GB_FLAG_HUB_11_WHALEBAY);
    FlagMenuItem("Stone and Fire", GB_FLAG_HUB_12_STONEANDFIRE);
    FlagMenuItem("Precipice", GB_FLAG_HUB_13_PRECIPICE);
    FlagMenuItem("Top of the World", GB_FLAG_HUB_14_TOPOFTHEWORLD);
    FlagMenuItem("Sanctuary of Rock and Lava", GB_FLAG_HUB_15_SANCTUARY_ROCKANDLAVA);
    FlagMenuItem("Walk of Power", GB_FLAG_HUB_16_WALKOFPOWER);
    FlagMenuItem("Beneath the Sanctuary of Rock and Lava", GB_FLAG_HUB_17_BENEATH_SANCTUARY_ROCKLAVA);
    FlagMenuItem("Iron Mountains", GB_FLAG_HUB_18_IRONMOUNTAINS);
    FlagMenuItem("Prison Ship", GB_FLAG_HUB_19_PRISONSHIP);
    FlagMenuItem("Crow’s Nest", GB_FLAG_HUB_20_CROWSNEST);
    FlagMenuItem("Wipe_10 / Astro_20 (unused)", GB_FLAG_HUB_21_WIPE10_ASTRO20_UNUSED);
    FlagMenuItem("Tomb of the Ancients", GB_FLAG_HUB_22_TOMBOFTHEANCIENTS);

    ImGui::EndMenu();
  }

}