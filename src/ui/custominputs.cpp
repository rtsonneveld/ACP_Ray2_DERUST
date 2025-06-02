#include "custominputs.hpp"
#include "ui/ui.hpp"

const char* BITFIELD_UNKNOWN[32] = {
    "Flag 0",   // 0x00000001
    "Flag 1",   // 0x00000002
    "Flag 2",   // 0x00000004
    "Flag 3",   // 0x00000008
    "Flag 4",   // 0x00000010
    "Flag 5",   // 0x00000020
    "Flag 6",   // 0x00000040
    "Flag 7",   // 0x00000080
    "Flag 8",   // 0x00000100
    "Flag 9",   // 0x00000200
    "Flag 10",  // 0x00000400
    "Flag 11",  // 0x00000800
    "Flag 12",  // 0x00001000
    "Flag 13",  // 0x00002000
    "Flag 14",  // 0x00004000
    "Flag 15",  // 0x00008000
    "Flag 16",  // 0x00010000
    "Flag 17",  // 0x00020000
    "Flag 18",  // 0x00040000
    "Flag 19",  // 0x00080000
    "Flag 20",  // 0x00100000
    "Flag 21",  // 0x00200000
    "Flag 22",  // 0x00400000
    "Flag 23",  // 0x00800000
    "Flag 24",  // 0x01000000
    "Flag 25",  // 0x02000000
    "Flag 26",  // 0x04000000
    "Flag 27",  // 0x08000000
    "Flag 28",  // 0x10000000
    "Flag 29",  // 0x20000000
    "Flag 30",  // 0x40000000
    "Flag 31"  // 0x80000000
};


const char* BITFIELD_CUSTOMBITS[32] = {
    "UnseenFrozenAnimPlayer",    // 0x00000001
    "NeedModuleMatrices",        // 0x00000002
    "Movable",                   // 0x00000004
    "Projectile",                // 0x00000008
    "RayHit",                    // 0x00000010
    "Sightable",                 // 0x00000020
    "CannotCrushPrincipalActor", // 0x00000040
    "Collectable",               // 0x00000080
    "ActorHasShadow",            // 0x00000100
    "ShadowOnMe",                // 0x00000200
    "Prunable",                  // 0x00000400
    "OutOfVisibility",           // 0x00000800
    "UnseenFrozen",              // 0x00001000
    "NoAnimPlayer",              // 0x00002000
    "Fightable",                 // 0x00004000
    "NoMechanic",                // 0x00008000
    "NoAI",                      // 0x00010000
    "DestroyWhenAnimEnded",      // 0x00020000
    "NoAnimPlayerWhenTooFar",    // 0x00040000
    "NoAIWhenTooFar",            // 0x00080000
    "Unfreezable",               // 0x00100000
    "UsesTransparencyZone",      // 0x00200000
    "NoMecaWhenTooFar",          // 0x00400000
    "SoundWhenInvisible",        // 0x00800000
    "Protection",                // 0x01000000
    "CameraHit",                 // 0x02000000
    "CustomBit_27",              // 0x04000000
    "AIUser1",                   // 0x08000000
    "AIUser2",                   // 0x10000000
    "AIUser3",                   // 0x20000000
    "AIUser4",                   // 0x40000000
    "Rayman"                     // 0x80000000
};

const char* BITFIELD_DYNAMICS_FLAGS[32] = {
    "Animation",                // 0x00000001
    "Collide",                  // 0x00000002
    "Gravity",                  // 0x00000004
    "Tilt",                     // 0x00000008
    "Gi",                       // 0x00000010
    "OnGround",                 // 0x00000020
    "Climb",                    // 0x00000040
    "CollisionControl",         // 0x00000080
    "KeepWallSpeedZ",           // 0x00000100
    "SpeedLimit",               // 0x00000200
    "Inertia",                  // 0x00000400
    "Stream",                   // 0x00000800
    "StickOnPlatform",          // 0x00001000
    "Scale",                    // 0x00002000
    "AbsoluteImposeSpeed",      // 0x00004000
    "AbsoluteProposeSpeed",     // 0x00008000
    "AbsoluteAddSpeed",         // 0x00010000
    "ImposeSpeedX",             // 0x00020000
    "ImposeSpeedY",             // 0x00040000
    "ImposeSpeedZ",             // 0x00080000
    "ProposeSpeedX",            // 0x00100000
    "ProposeSpeedY",            // 0x00200000
    "ProposeSpeedZ",            // 0x00400000
    "AddSpeedX",                // 0x00800000
    "AddSpeedY",                // 0x01000000
    "AddSpeedZ",                // 0x02000000
    "LimitX",                   // 0x04000000
    "LimitY",                   // 0x08000000
    "LimitZ",                   // 0x10000000
    "ImposeRotation",           // 0x20000000
    "LockPlatform",             // 0x40000000
    "ImposeTranslation"         // 0x80000000
};

const char* BITFIELD_DYNAMICS_ENDFLAGS[32] = {
    "BaseSize",                 // 0x00000001
    "AdvancedSize",             // 0x00000002
    "ComplexSize",              // 0x00000004
    "Reservated",               // 0x00000008
    "MechanicChange",           // 0x00000010
    "PlatformCrash",            // 0x00000020
    "CanFall",                  // 0x00000040
    "Init",                     // 0x00000080
    "Spider",                   // 0x00000100
    "Shoot",                    // 0x00000200
    "SafeValid",                // 0x00000400
    "ComputeInvertMatrix",      // 0x00000800
    "ChangeScale",              // 0x00001000
    "Exec",                     // 0x00002000
    "CollisionReport",          // 0x00004000
    "NoGravity",                // 0x00008000
    "Stop",                     // 0x00010000
    "SlidingGround",            // 0x00020000
    "Always",                   // 0x00040000
    "Crash",                    // 0x00080000
    "Swim",                     // 0x00100000
    "NeverFall",                // 0x00200000
    "Hanging",                  // 0x00400000
    "WallAdjust",               // 0x00800000
    "ActorMove",                // 0x01000000
    "ForceSafeWalk",            // 0x02000000
    "DontUseNewMechanic",       // 0x04000000
    "Unused (27)",              // 0x08000000
    "Unused (28)",              // 0x10000000
    "Unused (29)",              // 0x20000000
    "Unused (30)",              // 0x40000000
    "Unused (31)"               // 0x80000000
};

void InputBitField(const char* label, unsigned long* bitfield, const char* bitLabels[]) {
  if (ImGui::CollapsingHeader(label, ImGuiTreeNodeFlags_DefaultOpen)) {
    if (ImGui::BeginTable("BitfieldTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
      for (int i = 0; i < 32; i++) {
        if (i % 4 == 0) ImGui::TableNextRow();
        ImGui::TableNextColumn();
        unsigned long bitMask = 1UL << i;
        bool isSet = (*bitfield & bitMask) != 0;
        if (ImGui::Checkbox(bitLabels[i], &isSet)) {
          if (isSet) *bitfield |= bitMask;
          else *bitfield &= ~bitMask;
        }
      }
      ImGui::EndTable();
    }
  }
}
