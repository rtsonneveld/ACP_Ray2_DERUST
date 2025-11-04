#pragma once

enum class InactiveItemVisibility : unsigned int
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

bool IsCollisionZoneEnabled(CollisionZoneMask zone);
void DR_DLG_Options_Draw();