#include "custominputs.hpp"
#include "ui/ui.hpp"
#include "ui/ui_util.hpp"
#include <string>
#include <algorithm>
#include <functional>
// C INCLUDES
#include <ACP_Ray2.h>
#include "mod/ai_strings.h"
#include "mod/cpa_functions.h"
#include "mod/globals.h"
#include "mod/dsgvarnames.h"

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

const char* BITFIELD_SPOFLAGS[15] = {
  
  "No Collision", // Not Pickable
  "Hidden", 
  "No Transformation Matrix", 
  "Zoom Instead Of Scale", 
  "Type Of Bounding Volume", 
  "Superimposed", 
  "Not Hit By Ray Trace", 
  "No Shadow On Me", 
  "Semi Look At", 
  "Check Children", 
  "Magnet Modification", 
  "Module Transparency", 
  "Exclude Light", 
  "Superimposed Clipping", 
  "Outline Mode"
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

const char* BITFIELD_STDGAME_MISCFLAGS[8] = {
  "Desactive at all",
  "Activable",
  "Active",
  "All second pass done",
  "Always",
  "Useless culling",
  "Always active",
  "Too far"
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

const char* BITFIELD_DR_DISPLAYOPTIONS_ZONE[4] = {
    "Mechanics",
    "Repositioning",
    "Event",
    "Detection",
};

// Helper function to summarize selected flags
std::string GetSelectedFlagsString(const char * label, uint32_t flags, const char* bitLabels[], int numItems)
{
  std::string result;

  for (int i=0;i<numItems;i++)
  {
    if (flags & 1<<i)
    {
      if (!result.empty()) result += ", ";
      result += bitLabels[i];
    }
  }
  if (result.empty()) result = "None";

  result = std::string(label) + " (" + result + ")" + "###" + label;
  return result;
}


void InputBitField(const char* label, unsigned long* bitfield, const char* bitLabels[], int numItems) {

  std::string headerText = GetSelectedFlagsString(label, *bitfield, bitLabels, numItems);

  if (ImGui::CollapsingHeader(headerText.c_str())) {
    if (ImGui::BeginTable("BitfieldTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
      for (int i = 0; i < numItems; i++) {
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

void InputMatrix(const char* label, MTH3D_tdstMatrix* matrix) {

  if (ImGui::CollapsingHeader(label)) {
    ImGui::Indent();
    char columnLabel_0[64];
    char columnLabel_1[64];
    char columnLabel_2[64];

    snprintf(columnLabel_0, sizeof(columnLabel_0), "%s[0]", label);
    snprintf(columnLabel_1, sizeof(columnLabel_1), "%s[1]", label);
    snprintf(columnLabel_2, sizeof(columnLabel_2), "%s[2]", label);

    ImGui::DragFloat3(columnLabel_0, (float*)&matrix->stCol_0.x);
    ImGui::DragFloat3(columnLabel_1, (float*)&matrix->stCol_1.x);
    ImGui::DragFloat3(columnLabel_2, (float*)&matrix->stCol_2.x);
    ImGui::Unindent();
  }
}

void InputCompletePosition(const char* label, POS_tdstCompletePosition* position) {

  if (ImGui::CollapsingHeader(label)) {

    ImGui::Indent();

    char label_Position[64];
    char label_RotationMatrix[64];
    char label_TransformMatrix[64];

    snprintf(label_Position, sizeof(label_Position), "%s[position]", label);
    snprintf(label_RotationMatrix, sizeof(label_RotationMatrix), "%s[rotation]", label);
    snprintf(label_TransformMatrix, sizeof(label_TransformMatrix), "%s[transform]", label);

    if (ImGui::CollapsingHeader(label_Position)) {
      ImGui::DragFloat3(label_Position, (float*)&position->stPos.x);
    }

    InputMatrix(label_RotationMatrix, &position->stRotationMatrix);
    InputMatrix(label_TransformMatrix, &position->stTransformMatrix);

    ImGui::Unindent();
  }
}


void InputCollideMaterial(GMT_tdstCollideMaterial* mat)
{
  if (mat == nullptr || (int)mat == -1) {
    ImGui::Text("Collide Material: null");
    return;
  }

  ImGui::Text("Collide Material:");
  ImGui::Indent();
  ImGui::Text("Type of Zone: %d", mat->wTypeOfZone);
  ImGui::Text("Identifier: 0x%X", mat->xIdentifier);
  ImGui::Unindent();
}

void InputGameMaterial(GMT_tdstGameMaterial* mat)
{
  if (mat == nullptr || (int)mat == -1) {
    ImGui::Text("Game Material: null");
    return;
  }

  InputCollideMaterial(mat->hCollideMaterial);
}

// Core implementation - ALL shared logic lives here
static void InputPerso_Internal(
  const char* label,
  HIE_tdstSuperObject* currentSpo,
  const std::function<void(HIE_tdstSuperObject*)>& writeBack
) {
  static char searchBuffer[128] = "";

  // Make combo wider
  ImGui::SetNextItemWidth(300.0f);

  if (ImGui::BeginCombo(label, (currentSpo ? SPO_Name(currentSpo).c_str() : "null"), ImGuiComboFlags_HeightLarge)) {

    // Reset search on open
    if (ImGui::IsWindowAppearing()) {
      searchBuffer[0] = '\0';
      ImGui::SetKeyboardFocusHere();
    }

    // Search input
    ImGui::PushItemWidth(-1);
    ImGui::InputTextWithHint("##search", "Search...", searchBuffer, IM_ARRAYSIZE(searchBuffer));
    ImGui::PopItemWidth();
    ImGui::Separator();

    // Scrollable area
    ImGui::BeginChild("##combo_scroll", ImVec2(0, 200), false, ImGuiWindowFlags_HorizontalScrollbar);

    std::string searchLower(searchBuffer);
    std::transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);

    // Null option
    if (std::string("null").find(searchLower) != std::string::npos) {
      if (ImGui::Selectable("null", currentSpo == nullptr)) {
        writeBack(nullptr);
        ImGui::CloseCurrentPopup();
      }
    }

    // Actor list
    HIE_M_ForEachActor(actor) {

      std::string nameLower = SPO_Name(actor);
      std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);

      if (nameLower.find(searchLower) != std::string::npos) {

        if (ImGui::Selectable(SPO_Name(actor).c_str(), actor == currentSpo)) {

          writeBack(actor);
          ImGui::CloseCurrentPopup();
        }
      }
    }

    ImGui::EndChild();
    ImGui::EndCombo();
  }
}

void InputPerso(const char* label, HIE_tdstSuperObject** p_spo) {

  InputPerso_Internal(
    label,
    *p_spo,                            // read current SPO
    [&](HIE_tdstSuperObject* newSpo) { // write back SPO
      *p_spo = newSpo;
    }
  );
}

void InputPerso(const char* label, HIE_tdstEngineObject** p_actor) {

  HIE_tdstSuperObject* currentSpo =
    (*p_actor ? (*p_actor)->hStandardGame->p_stSuperObject : nullptr);

  InputPerso_Internal(
    label,
    currentSpo,                        // read current SPO
    [&](HIE_tdstSuperObject* newSpo) { // write back SPO
      if (newSpo)
        *p_actor = newSpo->hLinkedObject.p_stActor;
      else
        *p_actor = nullptr;
    }
  );
}

static void InputVector3(const char* label, char* buffer, int offsetInBuffer = 0) {
  char full_label[64];
  snprintf(full_label, sizeof(full_label), "%s##%p", label, (void*)(&buffer[offsetInBuffer]));

  ImGui::InputScalarN(full_label, ImGuiDataType_Float, &buffer[offsetInBuffer], 3, nullptr, nullptr, "%.3f", 0);
}

void DrawDsgVar(char* buffer, unsigned long offset, AI_tdeDsgVarType type) {

  if (buffer == nullptr) {
    ImGui::Text("N/A");
    return;
  }

  void* address = (void*)(&buffer[offset]);

  // Create a unique string based on the memory address
  char label[32];
  snprintf(label, sizeof(label), "%p_", address);

  switch (type) {
  case AI_E_dvt_Perso:            InputPerso(label, (HIE_tdstSuperObject**)address); break;
  case AI_E_dvt_Boolean:          ImGui::Checkbox(label, (bool*)address); break;
  case AI_E_dvt_Integer:          ImGui::InputScalar(label, ImGuiDataType_S32, address); break;
  case AI_E_dvt_PositiveInteger:  ImGui::InputScalar(label, ImGuiDataType_U32, address); break;
  case AI_E_dvt_UChar:            ImGui::InputScalar(label, ImGuiDataType_U8, address); break;
  case AI_E_dvt_Char:             ImGui::InputScalar(label, ImGuiDataType_S8, address); break;
  case AI_E_dvt_Short:            ImGui::InputScalar(label, ImGuiDataType_S16, address); break;
  case AI_E_dvt_UShort:           ImGui::InputScalar(label, ImGuiDataType_U16, address); break;
  case AI_E_dvt_Float:            ImGui::InputScalar(label, ImGuiDataType_Float, address); break;
  case AI_E_dvt_Vector:           InputVector3(label, buffer, offset); break;
  }
}

void DrawDsgVarId(HIE_tdstSuperObject* spo, int dsgVarId) {

    assert(spo != nullptr);
    HIE_tdstEngineObject * actor = spo->hLinkedObject.p_stActor;
    assert(actor != nullptr);
    AI_tdstBrain* brain = actor->hBrain;
    assert(brain != nullptr);
    AI_tdstMind* mind = brain->p_stMind;
    assert(mind != nullptr);
    AI_tdstAIModel* aiModel = mind->p_stAIModel;
    assert(aiModel != nullptr);
    AI_tdstDsgMem* dsgMem = brain->p_stMind->p_stDsgMem;
    assert(dsgMem != nullptr);

    bool isRayman = (actor == g_DR_rayman->hLinkedObject.p_stActor);
    bool isGlobal = (actor == g_DR_global->hLinkedObject.p_stActor);

    AI_tdstDsgVarInfo info = aiModel->p_stDsgVar->a_stDsgVarInfo[dsgVarId];

    ImGui::Text("%i", dsgVarId);
    ImGui::SameLine();
    ImGui::Text(AI_GetDsgVarTypeString(info.eTypeId));

    if (isRayman) {
      ImGui::SameLine();
      ImGui::Text(DV_STR_Rayman[dsgVarId]);
    }
    else if (isGlobal) {
      ImGui::SameLine();
      ImGui::Text(DV_STR_Global[dsgVarId]);
    }

    ImGui::SameLine();
    DrawDsgVar(dsgMem->p_cDsgMemBuffer, info.ulOffsetInDsgMem, info.eTypeId);
}