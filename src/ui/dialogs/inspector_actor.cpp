#include "inspector_actor.hpp"
#include "ui/ui.hpp"
#include <sstream>

// C INCLUDE
#include "mod/dsgvarnames.h"
#include "mod/globals.h"

#include <ACP_Ray2.h>

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

  // Collapsible section for bitfield toggles
  if (ImGui::CollapsingHeader(label, ImGuiTreeNodeFlags_DefaultOpen)) {

    if (ImGui::BeginTable("BitfieldTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
      for (int i = 0; i < 32; i++) {
        // Start a new row every 4 columns
        if (i % 4 == 0) {
          ImGui::TableNextRow();
        }
        ImGui::TableNextColumn();

        unsigned long bitMask = 1UL << i; // Calculate bitmask on the fly
        bool isSet = (*bitfield & bitMask) != 0;
        if (ImGui::Checkbox(customBitLabels[i], &isSet)) {
          if (isSet) {
            *bitfield |= bitMask; // Set the bit
          }
          else {
            *bitfield &= ~bitMask; // Clear the bit
          }
        }
      }
      ImGui::EndTable();
    }
  }
}

void InputPerso(const char* label, HIE_tdstSuperObject* p_data) {
  //ImGui::BeginCombo(label, )
}

void DrawDsgVar(char* buffer, unsigned long offset, AI_tdeDsgVarType type) {

  if (buffer == nullptr) {
    ImGui::Text("N/A");
    return;
  }

  void* address = (void*)(&buffer[offset]);

  // Create a unique string based on the memory address
  char label[32];
  snprintf(label, sizeof(label), "##%p", address);

  switch (type) {
  case AI_E_dvt_Perso:            InputPerso(label, (HIE_tdstSuperObject*)address); break;
  case AI_E_dvt_Boolean:          ImGui::Checkbox(label, (bool*)address); break;
  case AI_E_dvt_Integer:          ImGui::InputScalar(label, ImGuiDataType_S32, address); break;
  case AI_E_dvt_PositiveInteger:  ImGui::InputScalar(label, ImGuiDataType_U32, address); break;
  case AI_E_dvt_UChar:            ImGui::InputScalar(label, ImGuiDataType_U8, address); break;
  case AI_E_dvt_Char:             ImGui::InputScalar(label, ImGuiDataType_S8, address); break;
  case AI_E_dvt_Short:            ImGui::InputScalar(label, ImGuiDataType_S16, address); break;
  case AI_E_dvt_UShort:           ImGui::InputScalar(label, ImGuiDataType_U16, address); break;
  case AI_E_dvt_Float:            ImGui::InputScalar(label, ImGuiDataType_Float, address); break;
  case AI_E_dvt_Vector:
    for (int i = 0;i < 3; i++) {

      char label[32];
      snprintf(label, sizeof(label), "%c##%p", 'X' + i, (void*)(&buffer[offset]));

      ImGui::InputScalar(label, ImGuiDataType_Float, &buffer[offset + i * 4]);
    } break;
  }
}

const char* DsgVarTypeToString(AI_tdeDsgVarType type) {
  switch (type) {
  case AI_E_dvt_Boolean: return "Bool"; break;
  case AI_E_dvt_Char: return "Char"; break;
  case AI_E_dvt_UChar: return "UChar"; break;
  case AI_E_dvt_Short: return "Short"; break;
  case AI_E_dvt_UShort: return "UShort"; break;
  case AI_E_dvt_Integer: return "Int"; break;
  case AI_E_dvt_PositiveInteger: return "UInt"; break;
  case AI_E_dvt_Float: return "Float"; break;
  case AI_E_dvt_WayPoint: return "WayPoint"; break;
  case AI_E_dvt_Perso: return "Perso"; break;
  case AI_E_dvt_List: return "List"; break;
  case AI_E_dvt_Vector: return "Vector"; break;
  case AI_E_dvt_Comport: return "Comport"; break;
  case AI_E_dvt_Action: return "Action"; break;
  case AI_E_dvt_Text: return "Text"; break;
  case AI_E_dvt_GameMaterial: return "GameMaterial"; break;
  case AI_E_dvt_Caps: return "Caps"; break;
  case AI_E_dvt_Graph: return "Graph"; break;
  case AI_E_dvt_PersoArray: return "Perso[]"; break;
  case AI_E_dvt_VectorArray: return "Vector[]"; break;
  case AI_E_dvt_FloatArray: return "Float[]"; break;
  case AI_E_dvt_IntegerArray: return "Integer[]"; break;
  case AI_E_dvt_WayPointArray: return "WayPoint[]"; break;
  case AI_E_dvt_TextArray: return "Text[]"; break;
  case AI_E_dvt_SuperObject: return "SuperObject"; break;
  default: return "Unknown";
  }
}

bool displayDsgVarBufferInitial = false;
bool displayDsgVarBufferModel = false;

void DR_DLG_Inspector_Draw_MS_Brain(HIE_tdstEngineObject* actor)
{
  AI_tdstBrain* brain = actor->hBrain;

  if (brain == nullptr) {
    ImGui::Text("Actor has no brain");
    return;
  }

  AI_tdstMind* mind = brain->p_stMind;

  if (mind == nullptr) {
    ImGui::Text("Actor has no mind");
    return;
  }

  AI_tdstAIModel* aiModel = brain->p_stMind->p_stAIModel;
  AI_tdstDsgMem* dsgMem = brain->p_stMind->p_stDsgMem;

  if (aiModel == nullptr) {
    ImGui::Text("Actor has no AI Model");
    return;
  }

  if (ImGui::CollapsingHeader("DsgVars")) {

    ImGui::Checkbox("Initial instance values", &displayDsgVarBufferInitial); ImGui::SameLine();
    ImGui::Checkbox("Default AI Model values", &displayDsgVarBufferModel);

    if (aiModel->p_stDsgVar == nullptr) {
      ImGui::Text("This AI Model has no DsgVars");
    }
    else if (ImGui::BeginTable("DsgVarTable", (displayDsgVarBufferInitial ? 1 : 0) + (displayDsgVarBufferModel ? 1 : 0) + 3,
      ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit)) {

      bool isRayman = (actor == g_DR_rayman->hLinkedObject.p_stActor);

      ImGui::TableSetupColumn("ID");
      ImGui::TableSetupColumn(isRayman ? "Type/Name" : "Type");
      ImGui::TableSetupColumn("Value");
      if (displayDsgVarBufferInitial) {
        ImGui::TableSetupColumn("Value (init)");
      }
      if (displayDsgVarBufferModel) {
        ImGui::TableSetupColumn("Value (default)");
      }
      ImGui::TableHeadersRow();

      for (int i = 0;i < aiModel->p_stDsgVar->ucNbDsgVar;i++) {

        AI_tdstDsgVarInfo info = aiModel->p_stDsgVar->a_stDsgVarInfo[i];

        ImGui::TableNextColumn();
        ImGui::Text("%i", i);
        ImGui::TableNextColumn();
        ImGui::Text(DsgVarTypeToString(info.eTypeId));

        if (isRayman) {
          ImGui::SameLine();
          ImGui::Text(DV_STR_Rayman[i]);
        }

        ImGui::TableNextColumn();
        DrawDsgVar(dsgMem->p_cDsgMemBuffer, info.ulOffsetInDsgMem, info.eTypeId);

        if (displayDsgVarBufferInitial) {
          ImGui::TableNextColumn();
          DrawDsgVar(dsgMem->p_cDsgMemBufferInit, info.ulOffsetInDsgMem, info.eTypeId);
        }
        if (displayDsgVarBufferModel) {
          ImGui::TableNextColumn();
          DrawDsgVar(aiModel->p_stDsgVar->p_cDsgMemDefaultInit, info.ulOffsetInDsgMem, info.eTypeId);
        }

        ImGui::TableNextRow();
      }
      ImGui::EndTable();
    }

  }
}


void DR_DLG_Inspector_Draw_MS_StandardGame(HIE_tdstEngineObject* actor)
{
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

void DR_DLG_Inspector_Draw_Actor(HIE_tdstSuperObject* actorSPO) {

  HIE_tdstEngineObject* actor = actorSPO->hLinkedObject.p_stActor;
  if (actor->hStandardGame == NULL) {
    ImGui::Text("This always object has been destroyed!");
    return;
  }

  ImGui::SeparatorText("Mini-structs");

  if (ImGui::CollapsingHeader("StandardGame")) {
    ImGui::Indent();
    DR_DLG_Inspector_Draw_MS_StandardGame(actor);
    ImGui::Unindent();
  }
  if (ImGui::CollapsingHeader("3dData")) {
    ImGui::Indent();
    // TODO
    ImGui::Unindent();
  }
  if (ImGui::CollapsingHeader("Brain")) {
    ImGui::Indent();
    DR_DLG_Inspector_Draw_MS_Brain(actor);
    ImGui::Unindent();
  }
}