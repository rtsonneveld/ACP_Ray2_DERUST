#pragma once
#include "json.hpp"
#include "dialogs\options.hpp"
using json = nlohmann::json;

#define SETTINGS_FIELDS \
/* Dialogs */ \
    X(bool, dlg_aimodel, false) \
    X(bool, dlg_debugwindow, false) \
    X(bool, dlg_hierarchy, false) \
    X(bool, dlg_inspector, false) \
    X(bool, dlg_options, false) \
    X(bool, dlg_stats, false) \
    X(bool, dlg_playback, false) \
    X(bool, dlg_utils, false) \
    X(bool, dlg_engineglobals, false) \
/* Options */ \
    X(bool, opt_tryCatchExceptions, true) \
    X(bool, opt_drawVisuals, false) \
    X(unsigned int, opt_drawCollisionZones, (int)CollisionZoneMask::ZDM | (int)CollisionZoneMask::ZDR | (int)CollisionZoneMask::ZDE | (int)CollisionZoneMask::ZDD) \
    X(InactiveItemVisibility, opt_inactiveZoneVisibility, InactiveItemVisibility::Transparent) \
    X(bool, opt_drawNoCollisionObjects, false) \
    X(bool, opt_drawInvisibleObjects, true) \
    X(bool, opt_transparentZDRSlopes, false) \
    X(bool, opt_transparentZDRSlopesInvert, false) \
    X(float, opt_transparentZDRSlopesMin, 0.7071067812) \
    X(float, opt_transparentZDRSlopesMax, 0.7071067812) \
    X(float, opt_transparentZDRSlopesAlpha, 0.5f) \
    X(bool, opt_drawVirtualSectors, false) \
    X(bool, opt_drawSectorBorders, false) \
    X(InactiveItemVisibility, opt_inactiveSectorVisibility, InactiveItemVisibility::Transparent) \
    X(bool, opt_showTooFarLimitAnim, false) \
    X(bool, opt_showTooFarLimitMeca, false) \
    X(bool, opt_showTooFarLimitAI, false) \
    X(int, opt_tooFarLimitMinSize, 0) \
    X(int, opt_tooFarLimitMaxSize, 255) \
    X(float, opt_tooFarLimitHideRange, 0.0f) \
    X(DistanceCheckVisibility, opt_distanceCheckVisibility, DistanceCheckVisibility::FadeOut) \
/* Utilities */ \
    X(bool, util_showGLM, false) \
    /* Format: X(type, name, default_value) */

struct DR_Settings {
  // Define all fields automatically
#define X(type, name, default_value) type name = default_value;
  SETTINGS_FIELDS
#undef X
};

// JSON conversion hooks
inline void to_json(json& j, const DR_Settings& s) {
#define X(type, name, default_value) j[#name] = s.name;
  SETTINGS_FIELDS
#undef X
}

inline void from_json(const json& j, DR_Settings& s) {
  // Start with defaults
  DR_Settings temp;

#define X(type, name, default_value) \
        if (j.contains(#name)) temp.name = j.at(#name).get<type>();
  SETTINGS_FIELDS
#undef X

    s = temp;
}

// Global instance
extern DR_Settings g_DR_settings;

void DR_SaveSettings();
void DR_LoadSettings();