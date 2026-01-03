#include "menu.hpp"

#include "dialogs.hpp"

#include "hierarchy.hpp"
#include "ui/ui.hpp"
#include "ui/settings.hpp"
#include <imgui_internal.h>

#include <ACP_Ray2.h>

void DR_DLG_Menu_DrawLevelSelection() {

  #define CREATE_MENU(menu_title, ...) \
  if (ImGui::BeginMenu(menu_title)) { \
      const char* items[] = { __VA_ARGS__ }; \
      for (int i = 0; i < sizeof(items) / sizeof(items[0]); i += 2) { \
          if (ImGui::MenuItem(items[i])) { \
              GAM_fn_vAskToChangeLevel(items[i + 1], false); \
          } \
      } \
      ImGui::EndMenu(); \
  }

  CREATE_MENU("Menu",
    "Main Menu", "menu",
    "The Hall of Doors", "mapmonde",
    "Score Screen", "raycap"
  );

  CREATE_MENU("Bonus",
    "Walk of Life", "ly_10",
    "Walk of Power", "ly_20",
    "Bonus Stage", "bonux"
  );

  CREATE_MENU("Cutscenes",
    "Council Chamber of the Teensies", "nego_10",
    "Meanwhile in the Prison Ship 1", "batam_10",
    "Meanwhile in the Prison Ship 2", "batam_20",
    "Echoing Caves Intro", "bast_09",
    "Iron Mountains Balloon Cutscene", "ball",
    "Intro", "jail_10",
    "Ending", "end_10",
    "Credits", "staff_10",
    "Polokus Mask 1", "poloc_10",
    "Polokus Mask 2", "poloc_20",
    "Polokus Mask 3", "poloc_30",
    "Polokus Mask 4", "poloc_40"
  );

  CREATE_MENU("The Woods of Light",
    "Jail", "jail_20",
    "The Woods of Light", "learn_10"
  );

  CREATE_MENU("The Fairy Glade",
    "Part 1", "learn_30",
    "Part 2", "learn_31",
    "Part 3", "bast_20",
    "Part 4", "bast_22",
    "Part 5", "learn_60"
  );

  CREATE_MENU("The Marshes of Awakening",
    "Part 1", "ski_10",
    "Part 2", "ski_60"
  );

  CREATE_MENU("The Bayou",
    "Part 1", "chase_10",
    "Part 2", "chase_22"
  );

  CREATE_MENU("The Sanctuary of Water and Ice",
    "Part 1", "water_10",
    "Part 2", "water_20"
  );

  CREATE_MENU("The Menhir Hills",
    "Part 1", "rodeo_10",
    "Part 2", "rodeo_40",
    "Part 3", "rodeo_60"
  );

  CREATE_MENU("The Cave of Bad Dreams",
    "Part 1", "vulca_10",
    "Part 2", "vulca_20"
  );

  CREATE_MENU("The Canopy",
    "Part 1", "glob_30",
    "Part 2", "glob_10",
    "Part 3", "glob_20"
  );

  CREATE_MENU("Whale Bay",
    "Part 1", "whale_00",
    "Part 2", "whale_05",
    "Part 3", "whale_10"
  );

  CREATE_MENU("The Sanctuary of Stone and Fire",
    "Part 1", "plum_00",
    "Part 2", "plum_20",
    "Part 3", "plum_10"
  );

  CREATE_MENU("The Echoing Caves",
    "Part 1", "bast_10",
    "Part 2", "cask_10",
    "Part 3", "cask_30"
  );

  CREATE_MENU("The Precipice",
    "Part 1", "nave_10",
    "Part 2", "nave_15",
    "Part 3", "nave_20"
  );

  CREATE_MENU("The Top of the World",
    "Part 1", "seat_10",
    "Part 2", "seat_11"
  );

  CREATE_MENU("The Sanctuary of Rock and Lava",
    "Part 1", "earth_10",
    "Part 2", "earth_20",
    "Part 3", "earth_30"
  );

  CREATE_MENU("Beneath the Sanctuary of Rock and Lava",
    "Part 1", "helic_10",
    "Part 2", "helic_20",
    "Part 3", "helic_30"
  );

  CREATE_MENU("Tomb of the Ancients",
    "Part 1", "morb_00",
    "Part 2", "morb_10",
    "Part 3", "morb_20"
  );

  CREATE_MENU("The Iron Mountains",
    "Part 1", "learn_40",
    "Part 2", "ile_10",
    "Part 3", "mine_10"
  );

  CREATE_MENU("The Prison Ship",
    "Part 1", "boat01",
    "Part 2", "boat02",
    "Part 3", "astro_00",
    "Part 4", "astro_10"
  );

  CREATE_MENU("The Crow's Nest",
    "Part 1", "rhop_10"
  );

  #undef CREATE_MENU
}

void DR_DLG_Menu_Draw() {

  ImGuiContext& g = *ImGui::GetCurrentContext();
  float menuBarHeight = g.FontSize + g.Style.FramePadding.y * 2.0f;

  menuBarHeight = 1;

  if (ImGui::BeginMainMenuBar()) {

    if (ImGui::BeginMenu("Windows")) {
      ImGui::MenuItem("Hierarchy", nullptr, &g_DR_settings.dlg_hierarchy);
      ImGui::MenuItem("Inspector", nullptr, &g_DR_settings.dlg_inspector);
      ImGui::MenuItem("AI Model", nullptr, &g_DR_settings.dlg_aimodel);
      ImGui::MenuItem("AI Debugger", nullptr, &g_DR_settings.dlg_debugwindow);
      ImGui::MenuItem("Playback", nullptr, &g_DR_settings.dlg_playback);
      ImGui::MenuItem("Stats", nullptr, &g_DR_settings.dlg_stats);
      ImGui::MenuItem("Utilities", nullptr, &g_DR_settings.dlg_utils);
      ImGui::MenuItem("Globals", nullptr, &g_DR_settings.dlg_engineglobals); 
      ImGui::MenuItem("Options", nullptr, &g_DR_settings.dlg_options);
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Map")) {
      if (ImGui::MenuItem("Reload")) {
        GAM_fn_vAskToChangeLevel(GAM_fn_p_szGetLevelName(), false);
      }

      DR_DLG_Menu_DrawLevelSelection();

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Tools")) {

      DR_DLG_Cheats_Draw();

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }
}