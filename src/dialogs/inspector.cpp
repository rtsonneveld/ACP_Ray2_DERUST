#include "inspector.h"
#include <imgui.h>
#include <windows.h>
#include <derust.h>

bool DR_DLG_Inspector_Enabled = FALSE;


// Function to render bitfield toggles
void RenderCustomBitsEditor(const char* label, unsigned long* bitfield) {
  // Define an array of labels and bit masks for the custom bits
  static const char* customBitLabels[] = {
      "UnseenFrozenAnimPlayer",       // 1
      "NeedModuleMatrices",           // 2
      "Movable",                      // 3
      "Projectile",                   // 4
      "RayHit",                       // 5
      "Sightable",                    // 6
      "CannotCrushPrincipalActor",    // 7
      "Collectable",                  // 8
      "ActorHasShadow",               // 9
      "ShadowOnMe",                   // 10
      "Prunable",                     // 11
      "OutOfVisibility",              // 12
      "UnseenFrozen",                 // 13
      "NoAnimPlayer",                 // 14
      "Fightable",                    // 15
      "NoMechanic",                   // 16
      "NoAI",                         // 17
      "DestroyWhenAnimEnded",         // 18
      "NoAnimPlayerWhenTooFar",       // 19
      "NoAIWhenTooFar",               // 20
      "Unfreezable",                  // 21
      "UsesTransparencyZone",         // 22
      "NoMecaWhenTooFar",             // 23
      "SoundWhenInvisible",           // 24
      "Protection",                   // 25
      "CameraHit",                    // 26
      "CustomBit_27",                 // 27
      "AIUser1",                      // 28
      "AIUser2",                      // 29
      "AIUser3",                      // 30
      "AIUser4",                      // 31
      "Rayman"                        // 32
  };

  static const unsigned long customBitMasks[] = {
      0x00000001, 0x00000002, 0x00000004, 0x00000008, 0x00000010, 0x00000020,
      0x00000040, 0x00000080, 0x00000100, 0x00000200, 0x00000400, 0x00000800,
      0x00001000, 0x00002000, 0x00004000, 0x00008000, 0x00010000, 0x00020000,
      0x00040000, 0x00080000, 0x00100000, 0x00200000, 0x00400000, 0x00800000,
      0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000,
      0x40000000, 0x80000000
  };

  // Collapsible section for bitfield toggles
  if (ImGui::CollapsingHeader(label, ImGuiTreeNodeFlags_DefaultOpen)) {
    if (ImGui::BeginTable("BitfieldTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
      for (int i = 0; i < 32; i++) {
        // Start a new row every 4 columns
        if (i % 4 == 0) {
          ImGui::TableNextRow();
        }
        ImGui::TableNextColumn();

        bool isSet = (*bitfield & customBitMasks[i]) != 0;
        if (ImGui::Checkbox(customBitLabels[i], &isSet)) {
          if (isSet) {
            *bitfield |= customBitMasks[i]; // Set the bit
          }
          else {
            *bitfield &= ~customBitMasks[i]; // Clear the bit
          }
        }
      }
      ImGui::EndTable();
    }
  }
}

void DR_DLG_Inspector_Draw_Actor(HIE_tdstSuperObject* actorSPO) {

  HIE_tdstEngineObject* actor = actorSPO->hLinkedObject.p_stActor;
  if (actor->hStandardGame == NULL) {
    ImGui::Text("This always object has been destroyed!");
    return;
  }
  long lPersonalType = HIE_M_lActorGetPersonalType(actor);

  if (lPersonalType == GAM_C_InvalidObjectType || lPersonalType >= GAM_C_AlwaysObjectType) {

    ImGui::SeparatorText("Actor (Always)");
    ImGui::Text("%s/%s/%i", HIE_fn_szGetObjectFamilyName(actorSPO), HIE_fn_szGetObjectModelName(actorSPO), (lPersonalType - GAM_C_AlwaysObjectType));
  }
  else {
    ImGui::SeparatorText("Actor");
    ImGui::Text("%s/%s/%s", HIE_fn_szGetObjectFamilyName(actorSPO), HIE_fn_szGetObjectModelName(actorSPO), HIE_fn_szGetObjectPersonalName(actorSPO));
  }

  ImGui::SeparatorText("Mini-structs");

  if (ImGui::CollapsingHeader("Standard Game")) {
    GAM_tdstStandardGame* stdGame = actor->hStandardGame;

    const char* eInitFlagOptions[] = {
        "WhenPlayerGoOutOfActionZone",
        "Always",
        "WhenPlayerIsDead",
        "WhenMapJustLoaded",
        "WhenSavedGameJustLoaded",
        "NeverBackWhenTaken",
        "NumberOfObjectInit",
        "WhenGeneratorIsDesactivated"
    };

    ImGui::Combo("eInitFlagWhenOutOfZone", reinterpret_cast<int*>(&stdGame->eInitFlagWhenOutOfZone), eInitFlagOptions, IM_ARRAYSIZE(eInitFlagOptions));
    ImGui::Combo("eInitFlagWhenDeadOrTaken", reinterpret_cast<int*>(&stdGame->eInitFlagWhenDeadOrTaken), eInitFlagOptions, IM_ARRAYSIZE(eInitFlagOptions));

    // Unsigned longs
    ImGui::InputScalar("ulLastTrame", ImGuiDataType_U32, &stdGame->ulLastTrame);
    ImGui::InputScalar("ubf32Capabilities", ImGuiDataType_U32, &stdGame->ubf32Capabilities);

    // Unsigned chars
    ImGui::InputScalar("ucTractionFactor", ImGuiDataType_U8, &stdGame->ucTractionFactor);
    ImGui::InputScalar("ucHitPoints", ImGuiDataType_U8, &stdGame->ucHitPoints);
    ImGui::InputScalar("ucHitPointsMax", ImGuiDataType_U8, &stdGame->ucHitPointsMax);
    ImGui::InputScalar("ucHitPointsMaxMax", ImGuiDataType_U8, &stdGame->ucHitPointsMaxMax);
    RenderCustomBitsEditor("ulCustomBits", &stdGame->ulCustomBits);
    ImGui::InputScalar("ucPlatformType", ImGuiDataType_U8, &stdGame->ucPlatformType);
    ImGui::InputScalar("ucMiscFlags", ImGuiDataType_U8, &stdGame->ucMiscFlags);
    ImGui::InputScalar("ucTransparencyZoneMin", ImGuiDataType_U8, &stdGame->ucTransparencyZoneMin);
    ImGui::InputScalar("ucTransparencyZoneMax", ImGuiDataType_U8, &stdGame->ucTransparencyZoneMax);
    RenderCustomBitsEditor("ulSaveCustomBits", &stdGame->ulSaveCustomBits);
    ImGui::InputScalar("ucSaveHitPoints", ImGuiDataType_U8, &stdGame->ucSaveHitPoints);
    ImGui::InputScalar("ucSaveHitPointsMax", ImGuiDataType_U8, &stdGame->ucSaveHitPointsMax);
    ImGui::InputScalar("ucSaveMiscFlags", ImGuiDataType_U8, &stdGame->ucSaveMiscFlags);
    ImGui::InputScalar("ucTooFarLimit", ImGuiDataType_U8, &stdGame->ucTooFarLimit);
  }
  if (ImGui::CollapsingHeader("3dData")) {

  }
}

void DR_DLG_Inspector_Draw() {

  if (!DR_DLG_Inspector_Enabled) return;

  bool open = true;
  if (ImGui::Begin("Inspector", &DR_DLG_Inspector_Enabled, ImGuiWindowFlags_NoCollapse)) {

    if (g_DR_selectedObject != nullptr) {
      HIE_tdstSuperObject* spo = g_DR_selectedObject; // Alias

      switch (spo->ulType) {
        case HIE_C_Type_Unknown: break;
        case HIE_C_Type_Actor: DR_DLG_Inspector_Draw_Actor(spo); break;
      }
    }
    else {
      ImGui::Text("Select a superobject in the hierarchy");
    }
  }
  ImGui::End();
}