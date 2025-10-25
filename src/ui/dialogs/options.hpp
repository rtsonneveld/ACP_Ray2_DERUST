#pragma once

extern bool DR_DLG_Options_Enabled;

enum class InactiveSectorVisibility : unsigned int
{
  Visible = 0,
  Transparent = 1,
  Hidden = 2
};

enum class CollisionZoneMask : unsigned int
{
  ZDM = 1 << 0,
  ZDR = 1 << 1,
  ZDE = 1 << 2,
  ZDD = 1 << 3,
};


extern bool opt_drawVisuals;
extern unsigned int opt_drawCollisionZones;
extern bool opt_drawNoCollisionObjects;
extern bool opt_drawInvisibleObjects;
extern bool opt_transparentZDRWalls;
extern InactiveSectorVisibility opt_inactiveSectorVisibility;

bool IsCollisionZoneEnabled(CollisionZoneMask zone);
void DR_DLG_Options_Draw();