#include "inspector_actor.hpp"
#include "inspector_actor_dynam.hpp"
#include "inspector_actor_brain.hpp"
#include "ui/ui.hpp"
#include "ui/ui_util.hpp"
#include "ui/custominputs.hpp"
#include "ui/nameLookup.hpp"

// C INCLUDE
#include "mod/cpa_functions.h"
#include <ACP_Ray2.h>
#include <LST.h>

void InputStateSelection(GAM_tdst3dData* h3dData, HIE_tdstState** state, const char * label) {
  HIE_tdstState* currentState = *state;
  HIE_tdstState* iter;

  int currentStateNum = -1;
  int index = 0;
  LST_M_StaticForEach(&h3dData->h_Family->stForStateArray, iter) {
    if (iter == currentState) {
      currentStateNum = index;
      break;
    }
    index++;
  }

  int i = 0;
  std::string familyName = HIE_fn_szGetFamilyTypeName(h3dData->h_Family->lObjectFamilyType);
  std::string previewLabel = NameFromIndex(NameType::Family_State, familyName, currentStateNum) + "##preview";
  if (ImGui::BeginCombo(label, previewLabel.c_str())) {
    LST_M_StaticForEach(&h3dData->h_Family->stForStateArray, iter) {
      bool isSelected = (iter == currentState);

      // Bit string
      unsigned char bits = iter->ucCustomBits;
      std::string bitString;
      for (int b = 7; b >= 0; --b)
        bitString += (bits & (1 << b)) ? '1' : '0';

      std::string itemLabel = NameFromIndex(NameType::Family_State, familyName, i) + " (" + bitString + ")##" + label;

      if (ImGui::Selectable(itemLabel.c_str(), isSelected)) {
        *state = iter;
      }
      if (isSelected) {
        ImGui::SetItemDefaultFocus();
      }
      i++;
    }
    ImGui::EndCombo();
  }
}

void DR_DLG_Inspector_Draw_MS_3dData(HIE_tdstEngineObject* actor)
{
  GAM_tdst3dData* h3dData = actor->h3dData;

  if (h3dData == nullptr) {
    ImGui::Text("3dData is null");
    return;
  }

  InputStateSelection(h3dData, &h3dData->h_InitialState, "h_InitialState");
  HIE_tdstState* currentState = h3dData->h_CurrentState;
  InputStateSelection(h3dData, &h3dData->h_CurrentState, "h_CurrentState");
  if (h3dData->h_CurrentState != currentState) {
    PLA_fn_bSetNewState(actor->hStandardGame->p_stSuperObject, h3dData->h_CurrentState, true, true);
  }
  InputStateSelection(h3dData, &h3dData->h_FirstStateOfAction, "h_FirstStateOfAction");
  // TODO HIE_tdstState* h_InitialState;
  // TODO HIE_tdstState* h_CurrentState;
  // TODO HIE_tdstState* h_FirstStateOfAction;
  // TODO HIE_tdstObjectsTablesList* h_InitialObjectsTable;
  // TODO HIE_tdstObjectsTablesList* h_CurrentObjectsTable;
  // TODO HIE_tdstFamilyList* h_Family;
  // TODO POS_tdstCompletePosition stGLIObjectMatrix;
  // TODO POS_tdstCompletePosition* p_stGLIObjectAbsoluteMatrix;

  ImGui::InputScalar("uwCurrentFrame", ImGuiDataType_U16, &h3dData->uwCurrentFrame);
  ImGui::InputScalar("ucRepeatAnimation", ImGuiDataType_U8, &h3dData->ucRepeatAnimation);
  ImGui::InputScalar("ucNextEvent", ImGuiDataType_U8, &h3dData->ucNextEvent);
  
  // TODO unsigned char* d_ucEventActivation;
  // TODO GAM_tdstCouple* p_stCurrentHieCouples;
  
  ImGui::InputScalar("wCurrentHieNbCouples", ImGuiDataType_U16, &h3dData->wCurrentHieNbCouples);
  ImGui::InputScalar("wSizeOfArrayOfElts3d", ImGuiDataType_U16, &h3dData->wSizeOfArrayOfElts3d);

  // TODO HIE_tdstState* h_StateInLastFrame;
  // TODO HIE_tdstState* h_WantedState;

  ImGui::InputScalar("uwForcedFrame", ImGuiDataType_U16, &h3dData->uwForcedFrame);
  ImGui::InputScalar("ucFlagEndState", ImGuiDataType_U8, &h3dData->ucFlagEndState);
  ImGui::InputScalar("ucFlagEndOfAnim", ImGuiDataType_U8, &h3dData->ucFlagEndOfAnim);

  // TODO void* hArrayOfChannels;
  ImGui::InputScalar("ulNumberOfChannels", ImGuiDataType_U32, &h3dData->ulNumberOfChannels);
  // TODO void* hFirstActiveChannel;
  // TODO GAM_tdstFrame3d stFrame3d;

  // TODO void* hMorphChannelList;

  ImGui::InputScalar("ulStartTime", ImGuiDataType_U32, &h3dData->ulStartTime);
  ImGui::InputScalar("ulTimeDelay", ImGuiDataType_U32, &h3dData->ulTimeDelay);
  ImGui::InputScalar("ulTimePreviousFrame", ImGuiDataType_U32, &h3dData->ulTimePreviousFrame);
  ImGui::InputScalar("sLastFrame", ImGuiDataType_S16, &h3dData->sLastFrame);
  ImGui::Checkbox("bStateJustModified", (bool*)& h3dData->bStateJustModified);
  ImGui::Checkbox("bSkipCurrentFrame", (bool*)&h3dData->bSkipCurrentFrame);

  // TODO GMT_tdstGameMaterial* p_stShadowMaterial;
  // TODO GLI_tdstTexture* p_stShadowTexture;

  MTH_tdxReal xShadowScaleX;
  MTH_tdxReal xShadowScaleY;

  ImGui::InputScalar("xShadowScaleX", ImGuiDataType_Float, &h3dData->xShadowScaleX);
  ImGui::InputScalar("xShadowScaleY", ImGuiDataType_Float, &h3dData->xShadowScaleY);
  ImGui::InputScalar("xShadowQuality", ImGuiDataType_S16, &h3dData->xShadowQuality);
  ImGui::InputScalar("uwNbEngineFrameSinceLastMechEvent", ImGuiDataType_U16, &h3dData->uwNbEngineFrameSinceLastMechEvent);
  ImGui::InputScalar("ucFrameRate", ImGuiDataType_U8, &h3dData->ucFrameRate);
  ImGui::InputScalar("ucFlagModifState", ImGuiDataType_U8, &h3dData->ucFlagModifState);
  ImGui::InputScalar("lDrawMaskInit", ImGuiDataType_S32, &h3dData->lDrawMaskInit);
  ImGui::InputScalar("lDrawMask", ImGuiDataType_S32, &h3dData->lDrawMask);
  ImGui::InputScalar("lLastComputeFrame", ImGuiDataType_S32, &h3dData->lLastComputeFrame);

  // TODO MTH3D_tdstVector stLastEventGlobalPosition;

  ImGui::InputScalar("ucUserEventFlags", ImGuiDataType_U8, &h3dData->ucUserEventFlags);
  ImGui::InputScalar("ucBrainComputationFrequency", ImGuiDataType_U8, &h3dData->ucBrainComputationFrequency);
  ImGui::InputScalar("cBrainCounter", ImGuiDataType_S8, &h3dData->cBrainCounter);
  ImGui::InputScalar("uwBrainMainCounter", ImGuiDataType_U16, &h3dData->uwBrainMainCounter);
  ImGui::InputScalar("ucTransparency", ImGuiDataType_U8, &h3dData->ucTransparency);
  ImGui::InputScalar("ucLightComputationFrequency", ImGuiDataType_U8, &h3dData->ucLightComputationFrequency);
  ImGui::InputScalar("cLightCounter", ImGuiDataType_S8, &h3dData->cLightCounter);

  // TODO MTH3D_tdstVector stSHWDeformationVector;
  ImGui::InputScalar("xSHWHeight", ImGuiDataType_Float, &h3dData->xSHWHeight);
}

void DR_DLG_Inspector_Draw_MS_StandardGame(HIE_tdstEngineObject* actor)
{
  GAM_tdstStandardGame* stdGame = actor->hStandardGame;

  if (stdGame == nullptr) {
    ImGui::Text("stdGame is null, actor is dead!");
    return;
  }

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
  InputBitField("ulCustomBits", &stdGame->ulCustomBits, BITFIELD_CUSTOMBITS, IM_ARRAYSIZE(BITFIELD_CUSTOMBITS));
  ImGui::InputScalar("ucPlatformType", ImGuiDataType_U8, &stdGame->ucPlatformType);

  InputBitField("ucMiscFlags", (unsigned long*)&stdGame->ucMiscFlags, BITFIELD_STDGAME_MISCFLAGS, IM_ARRAYSIZE(BITFIELD_STDGAME_MISCFLAGS));

  ImGui::InputScalar("ucTransparencyZoneMin", ImGuiDataType_U8, &stdGame->ucTransparencyZoneMin);
  ImGui::InputScalar("ucTransparencyZoneMax", ImGuiDataType_U8, &stdGame->ucTransparencyZoneMax);
  InputBitField("ulSaveCustomBits", &stdGame->ulSaveCustomBits, BITFIELD_CUSTOMBITS, IM_ARRAYSIZE(BITFIELD_CUSTOMBITS));
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

  if (ImGui::Button("Kill")) {
    fn_vKillEngineObjectOrAlwaysByPointer(actor);
  }

  if (ImGui::Button("Make Main Actor")) {
    GAM_g_stEngineStructure->g_hMainActor = actorSPO;
  }

  ImGui::SeparatorText("Mini-structs");

  if (ImGui::CollapsingHeader("StandardGame")) {
    ImGui::Indent();
    DR_DLG_Inspector_Draw_MS_StandardGame(actor);
    ImGui::Unindent();
  }
  if (ImGui::CollapsingHeader("3dData")) {
    ImGui::Indent();
    DR_DLG_Inspector_Draw_MS_3dData(actor);
    ImGui::Unindent();
  }
  if (ImGui::CollapsingHeader("Dynam")) {
    ImGui::Indent();
    DR_DLG_Inspector_Draw_MS_Dynam(actor);
    ImGui::Unindent();
  }
  if (ImGui::CollapsingHeader("Brain")) {
    ImGui::Indent();
    DR_DLG_Inspector_Draw_MS_Brain(actor);
    ImGui::Unindent();
  }
}