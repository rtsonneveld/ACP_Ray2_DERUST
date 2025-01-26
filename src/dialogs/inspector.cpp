#include "inspector.h"
#include <imgui.h>
#include <windows.h>
#include <derust.h>
#include <sstream>

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

void DrawDsgVar(char* buffer, unsigned long offset, AI_tdeDsgVarType type) {

  if (buffer == nullptr) {
    ImGui::Text("N/A");
    return;
  }

  // Create a unique string based on the memory address

  char label[32];
  snprintf(label, sizeof(label), "##%p", (void*)(&buffer[offset]));

  switch (type) {
    case AI_E_dvt_Boolean:          ImGui::Checkbox(label, (bool*)(&buffer[offset])); break;
    case AI_E_dvt_Integer:          ImGui::InputScalar(label, ImGuiDataType_S32,   &buffer[offset]); break;
    case AI_E_dvt_PositiveInteger:  ImGui::InputScalar(label, ImGuiDataType_U32,   &buffer[offset]); break;
    case AI_E_dvt_UChar:            ImGui::InputScalar(label, ImGuiDataType_U8,    &buffer[offset]); break;
    case AI_E_dvt_Char:             ImGui::InputScalar(label, ImGuiDataType_S8,    &buffer[offset]); break;
    case AI_E_dvt_Short:            ImGui::InputScalar(label, ImGuiDataType_S16,   &buffer[offset]); break;
    case AI_E_dvt_UShort:           ImGui::InputScalar(label, ImGuiDataType_U16,   &buffer[offset]); break;
    case AI_E_dvt_Float:            ImGui::InputScalar(label, ImGuiDataType_Float, &buffer[offset]); break;
    case AI_E_dvt_Vector:           
      for (int i = 0;i < 3; i++) {

        char label[32];
        snprintf(label, sizeof(label), "%c##%p", 'X' + i, (void*)(&buffer[offset]));

        ImGui::InputScalar(label, ImGuiDataType_Float, &buffer[offset+i*4]);
      } break;
  }
}

const char * DsgVarTypeToString(AI_tdeDsgVarType type) {
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

void DR_DLG_Inspector_Draw_MS_Brain(HIE_tdstEngineObject* actor)
{
  AI_tdstBrain* brain = actor->hBrain;
  AI_tdstMind* mind = brain->p_stMind;
  AI_tdstAIModel* aiModel = brain->p_stMind->p_stAIModel;
  AI_tdstDsgMem* dsgMem = brain->p_stMind->p_stDsgMem;

  if (brain == nullptr || mind == nullptr || aiModel == nullptr) {
    ImGui::Text("Actor has no brain or mind or ai model");
    return;
  }

  if (ImGui::CollapsingHeader("DsgVars")) {

    if (aiModel->p_stDsgVar == nullptr) {
      ImGui::Text("This AI Model has no DsgVars");
    } else if (ImGui::BeginTable("DsgVarTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit)) {

      bool isRayman = (actor == g_DR_rayman->hLinkedObject.p_stActor);

      ImGui::TableSetupColumn("ID");
      ImGui::TableSetupColumn(isRayman ? "Type/Name" : "Type");
      ImGui::TableSetupColumn("Value");
      ImGui::TableSetupColumn("Value (init)");
      ImGui::TableSetupColumn("Value (model)");
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
        ImGui::TableNextColumn();
        DrawDsgVar(dsgMem->p_cDsgMemBufferInit, info.ulOffsetInDsgMem, info.eTypeId);
        ImGui::TableNextColumn();
        DrawDsgVar(aiModel->p_stDsgVar->p_cDsgMemDefaultInit, info.ulOffsetInDsgMem, info.eTypeId);

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