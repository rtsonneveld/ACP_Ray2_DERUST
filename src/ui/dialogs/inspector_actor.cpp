#include "inspector_actor.hpp"
#include "ui/ui.hpp"
#include "ui/ui_util.hpp"
#include "ui/comportNames.hpp"
#include "ui/custominputs.hpp"

// C INCLUDE
#include "mod/dsgvarnames.h"
#include "mod/globals.h"
#include "mod/cpa_functions.h"
#include <ACP_Ray2.h>
#include <algorithm>

void InputPerso(const char* label, HIE_tdstSuperObject** p_data) {
  static char searchBuffer[128] = "";
  HIE_tdstSuperObject* spo = *p_data;

  // Make combo wider
  ImGui::SetNextItemWidth(300.0f); // adjust width as needed

  if (ImGui::BeginCombo(label, (spo != nullptr ? SPO_Name(spo).c_str() : "null"), ImGuiComboFlags_HeightLarge)) {

    // Reset search on open
    if (ImGui::IsWindowAppearing()) {
      searchBuffer[0] = '\0';
      ImGui::SetKeyboardFocusHere();
    }

    // Floating search box
    ImGui::PushItemWidth(-1);
    ImGui::InputTextWithHint("##search", "Search...", searchBuffer, IM_ARRAYSIZE(searchBuffer));
    ImGui::PopItemWidth();

    ImGui::Separator();

    // Scrollable region
    ImGui::BeginChild("##combo_scroll", ImVec2(0, 200), false, ImGuiWindowFlags_HorizontalScrollbar);

    std::string searchLower(searchBuffer);
    std::transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);

    // Null option
    if (std::string("null").find(searchLower) != std::string::npos) {
      if (ImGui::Selectable("null", spo == nullptr)) {
        *p_data = nullptr;
        ImGui::CloseCurrentPopup(); // close combo on selection
      }
    }

    HIE_M_ForEachActor(actor) {
      std::string nameLower = SPO_Name(actor);
      std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);

      if (nameLower.find(searchLower) != std::string::npos) {
        if (ImGui::Selectable(SPO_Name(actor).c_str(), actor == spo)) {
          *p_data = actor;
          ImGui::CloseCurrentPopup(); // close combo on selection
        }
      }
    }

    ImGui::EndChild();
    ImGui::EndCombo();
  }
}

void InputVector3(const char* label, char* buffer, int offsetInBuffer = 0) {
  char full_label[64];
  snprintf(full_label, sizeof(full_label), "%s##%p", label, (void*)(&buffer[offsetInBuffer]));

  ImGui::InputScalarN(full_label, ImGuiDataType_Float, &buffer[offsetInBuffer], 3, nullptr, nullptr, "%.3f", 0);
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

    InputVector3(columnLabel_0, (char*)&matrix->stCol_0.x);
    InputVector3(columnLabel_1, (char*)&matrix->stCol_1.x);
    InputVector3(columnLabel_2, (char*)&matrix->stCol_2.x);
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
      InputVector3(label_Position, (char*)&position->stPos.x);
    }
    
    InputMatrix(label_RotationMatrix, &position->stRotationMatrix);
    InputMatrix(label_TransformMatrix, &position->stTransformMatrix);

    ImGui::Unindent();
  }
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

void DrawComportPicker(HIE_tdstEngineObject* actor, AI_tdstMind * mind, AI_tdstAIModel * model, bool isReflex) {

  int activeComport = -1;
  
  AI_tdstScriptAI* scriptAI = isReflex ? model->a_stScriptAIReflex : model->a_stScriptAIIntel;
  AI_tdstIntelligence* intelligence = isReflex ? mind->p_stReflex : mind->p_stIntelligence;

  if (scriptAI == nullptr || intelligence == nullptr) {
    ImGui::BeginDisabled();
    ImGui::BeginCombo(isReflex ? "Active Reflex" : "Active Comport", isReflex ? "No Reflex" : "No Comport");
    ImGui::EndDisabled();
    return;
  }

  for (int i = 0;i < scriptAI->ulNbComport;i++) {
    if (intelligence->p_stCurrentComport == &scriptAI->a_stComport[i]) {
      activeComport = i;
      break;
    }
  }

  char* modelName = HIE_fn_szGetObjectModelName(actor->hStandardGame->p_stSuperObject);
  char label[32];
  snprintf(label, sizeof(label), "%s##preview", (isReflex ? GetReflexName(modelName, activeComport) : GetComportName(modelName, activeComport)).c_str());
  if (ImGui::BeginCombo(isReflex?"Active Reflex":"Active Comport", label)) {

    for (int i = 0;i < scriptAI->ulNbComport;i++) {
      snprintf(label, sizeof(label), "%s##selection", (isReflex ? GetReflexName(modelName, i) : GetComportName(modelName, i)).c_str());
      if (ImGui::Selectable(label, i == activeComport)) {
        AI_fn_ucChangeComportIntell(intelligence, &scriptAI->a_stComport[i]);
      }
    }

    ImGui::EndCombo();
  }
}

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


  if (ImGui::CollapsingHeader("Behavior")) {

    DrawComportPicker(actor, mind, aiModel, false);
    DrawComportPicker(actor, mind, aiModel, true);
  }

  if (ImGui::CollapsingHeader("DsgVars")) {

    ImGui::Checkbox("Initial instance values", &displayDsgVarBufferInitial); ImGui::SameLine();
    ImGui::Checkbox("Default AI Model values", &displayDsgVarBufferModel);

    if (aiModel->p_stDsgVar == nullptr) {
      ImGui::Text("This AI Model has no DsgVars");
    }
    else if (ImGui::BeginTable("DsgVarTable", (displayDsgVarBufferInitial ? 1 : 0) + (displayDsgVarBufferModel ? 1 : 0) + 3,
      ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollY)) {

      bool isRayman = (actor == g_DR_rayman->hLinkedObject.p_stActor);
      bool isGlobal = (actor == g_DR_global->hLinkedObject.p_stActor);

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
        } else if (isGlobal) {
          ImGui::SameLine();
          ImGui::Text(DV_STR_Global[i]);
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

bool forceOverwriteDynam = FALSE;
DNM_tdstDynamicsBaseBlock dynamicsBase;
DNM_tdstDynamicsAdvancedBlock dynamicsAdvanced;
DNM_tdstDynamicsComplexBlock dynamicsComplex;

void DR_DLG_Inspector_Draw_MS_Dynam(HIE_tdstEngineObject* actor)
{
  DNM_tdstDynam* dynam = actor->hDynam;

  if (dynam == nullptr) {
    ImGui::Text("Dynam is null");
    return;
  }

  DNM_tdstDynamics* dynamics = dynam->p_stDynamics;
  if (dynamics != nullptr) {
    if (ImGui::CollapsingHeader("Dynamics")) {

      ImGui::Checkbox("Force overwrite", &forceOverwriteDynam);

      ImGui::Indent();

      if (ImGui::CollapsingHeader("Base")) {

        ImGui::Indent();

        if (!forceOverwriteDynam) {
          dynamicsBase = dynamics->stDynamicsBase;
        }

        // TODO dynamicsBase.lObjectType (DNM_eMIC_Error = -1,DNM_eCamera, DNM_eBase)
        // TODO dynamicsBase.pCurrentIdCard (DNM_tdstMecBaseIdCard*)

        if (ImGui::CollapsingHeader("pCurrentIdCard")) {
          ImGui::Indent();

          DNM_tdstMecBaseIdCard* idCard = dynamicsBase.pCurrentIdCard;

          ImGui::InputScalar("xGravity", ImGuiDataType_Float, &idCard->m_xGravity);
          /*ImGui::InputScalar("xSlopeLimit", ImGuiDataType_Float, &dynamicsBase.xSlopeLimit);
          ImGui::InputScalar("xCosSlope", ImGuiDataType_Float, &dynamicsBase.xCosSlope);
          ImGui::InputScalar("xSlide", ImGuiDataType_Float, &dynamicsBase.xSlide);
          ImGui::InputScalar("xRebound", ImGuiDataType_Float, &dynamicsBase.xRebound);*/

          ImGui::Unindent();
        }

        InputBitField("ulFlags", &dynamicsBase.ulFlags, BITFIELD_DYNAMICS_FLAGS, IM_ARRAYSIZE(BITFIELD_DYNAMICS_FLAGS));
        InputBitField("ulEndFlags", &dynamicsBase.ulEndFlags, BITFIELD_DYNAMICS_ENDFLAGS, IM_ARRAYSIZE(BITFIELD_DYNAMICS_ENDFLAGS));

        ImGui::InputScalar("xGravity", ImGuiDataType_Float, &dynamicsBase.xGravity);
        ImGui::InputScalar("xSlopeLimit", ImGuiDataType_Float, &dynamicsBase.xSlopeLimit);
        ImGui::InputScalar("xCosSlope", ImGuiDataType_Float, &dynamicsBase.xCosSlope);
        ImGui::InputScalar("xSlide", ImGuiDataType_Float, &dynamicsBase.xSlide);
        ImGui::InputScalar("xRebound", ImGuiDataType_Float, &dynamicsBase.xRebound);

        InputVector3("stImposeSpeed", (char*)&(dynamicsBase.stImposeSpeed.x));
        InputVector3("stProposeSpeed", (char*)&(dynamicsBase.stProposeSpeed.x));
        InputVector3("stPreviousSpeed", (char*)&(dynamicsBase.stPreviousSpeed.x));
        InputVector3("stScale", (char*)&(dynamicsBase.stScale.x));
        InputVector3("stSpeedAnim", (char*)&(dynamicsBase.stSpeedAnim.x));
        InputVector3("stSafeTranslation", (char*)&(dynamicsBase.stSafeTranslation.x));
        InputVector3("stAddTranslation", (char*)&(dynamicsBase.stAddTranslation.x));

        InputCompletePosition("stPreviousMatrix", &dynamicsBase.stPreviousMatrix);
        InputCompletePosition("stCurrentMatrix", &dynamicsBase.stCurrentMatrix);
        InputMatrix("stImposeRotationMatrix", &dynamicsBase.stImposeRotationMatrix);

        ImGui::Unindent();

        dynamics->stDynamicsBase = dynamicsBase;

      }

      if (DNM_M_bDynamicsIsAdvancedSize(dynamics) || DNM_M_bDynamicsIsComplexSize(dynamics)) {

        if (ImGui::CollapsingHeader("Advanced")) {

          ImGui::Indent();
          
          if (!forceOverwriteDynam) {
            dynamicsAdvanced = dynamics->stDynamicsAdvanced;
          }

          ImGui::InputScalar("xInertiaX", ImGuiDataType_Float, &dynamicsAdvanced.xInertiaX);
          ImGui::InputScalar("xInertiaY", ImGuiDataType_Float, &dynamicsAdvanced.xInertiaY);
          ImGui::InputScalar("xInertiaZ", ImGuiDataType_Float, &dynamicsAdvanced.xInertiaZ);

          ImGui::InputScalar("xStreamPrority", ImGuiDataType_Float, &dynamicsAdvanced.xStreamPrority);
          ImGui::InputScalar("xStreamFactor", ImGuiDataType_Float, &dynamicsAdvanced.xStreamFactor);

          ImGui::InputScalar("xSlideFactorX", ImGuiDataType_Float, &dynamicsAdvanced.xSlideFactorX);
          ImGui::InputScalar("xSlideFactorY", ImGuiDataType_Float, &dynamicsAdvanced.xSlideFactorY);
          ImGui::InputScalar("xSlideFactorZ", ImGuiDataType_Float, &dynamicsAdvanced.xSlideFactorZ);
          ImGui::InputScalar("xPreviousSlide", ImGuiDataType_Float, &dynamicsAdvanced.xPreviousSlide);

          InputVector3("stMaxSpeed", (char*)&(dynamicsAdvanced.stMaxSpeed.x));
          InputVector3("stStreamSpeed", (char*)&(dynamicsAdvanced.stStreamSpeed.x));
          InputVector3("stAddSpeed", (char*)&(dynamicsAdvanced.stAddSpeed.x));
          InputVector3("stLimit", (char*)&(dynamicsAdvanced.stLimit.x));

          InputVector3("stCollisionTranslation", (char*)&(dynamicsAdvanced.stCollisionTranslation.x));
          InputVector3("stInertiaTranslation", (char*)&(dynamicsAdvanced.stInertiaTranslation.x));
          InputVector3("stGroundNormal", (char*)&(dynamicsAdvanced.stGroundNormal.x));
          InputVector3("stWallNormal", (char*)&(dynamicsAdvanced.stWallNormal.x));

          ImGui::InputScalar("ucCollideCounter", ImGuiDataType_U8, &(dynamicsAdvanced.ucCollideCounter));

          ImGui::Unindent();

          dynamics->stDynamicsAdvanced= dynamicsAdvanced;
        }
      }
      if (DNM_M_bDynamicsIsComplexSize(dynamics)) {

        if (ImGui::CollapsingHeader("Complex")) {

          ImGui::Indent();
          
          if (!forceOverwriteDynam) {
            dynamicsComplex = dynamics->stDynamicsComplex;
          }

          ImGui::InputScalar("xTiltIntensity", ImGuiDataType_Float, &dynamicsComplex.xTiltIntensity);
          ImGui::InputScalar("xTiltInertia", ImGuiDataType_Float, &dynamicsComplex.xTiltInertia);
          ImGui::InputScalar("xTiltOrigin", ImGuiDataType_Float, &dynamicsComplex.xTiltOrigin);
          ImGui::InputScalar("xTiltAngle", ImGuiDataType_Float, &dynamicsComplex.xTiltAngle);
          ImGui::InputScalar("xHangingLimit", ImGuiDataType_Float, &dynamicsComplex.xHangingLimit);

          InputVector3("stContact", (char*)&(dynamicsComplex.stContact.x));
          InputVector3("stFallTranslation", (char*)&(dynamicsComplex.stFallTranslation.x));

          if (ImGui::CollapsingHeader("stExternalDatas")) {
            
            ImGui::Indent();
            DNM_tdstMACDPID stExternalDatas = dynamicsComplex.stExternalDatas;

            ImGui::InputScalar("xData0", ImGuiDataType_Float, &stExternalDatas.xData0);
            InputVector3("stData1", (char*)&(stExternalDatas.stData1.x));
            InputVector3("stData2", (char*)&(stExternalDatas.stData2.x));
            InputVector3("stData3", (char*)&(stExternalDatas.stData3.x));
            ImGui::InputScalar("xData3", ImGuiDataType_Float, &stExternalDatas.xData3);
            ImGui::InputScalar("xData4", ImGuiDataType_Float, &stExternalDatas.xData4);
            ImGui::InputScalar("xData5", ImGuiDataType_Float, &stExternalDatas.xData5);

            ImGui::InputScalar("stData6.xAngle", ImGuiDataType_Float, &stExternalDatas.stData6.xAngle);
            InputVector3("stData6.stAxis", (char*)&(stExternalDatas.stData6.stAxis.x));

            ImGui::InputScalar("stData7.xAngle", ImGuiDataType_Float, &stExternalDatas.stData7.xAngle);
            InputVector3("stData7.stAxis", (char*)&(stExternalDatas.stData7.stAxis.x));

            ImGui::InputScalar("cData8", ImGuiDataType_S8, &stExternalDatas.cData8);
            ImGui::InputScalar("uwData9", ImGuiDataType_U16, &stExternalDatas.uwData9);
            InputVector3("stData10", (char*)&(stExternalDatas.stData10.x));
            ImGui::InputScalar("xData11", ImGuiDataType_Float, &stExternalDatas.xData11);
            InputVector3("stData12", (char*)&(stExternalDatas.stData12.x));
            ImGui::InputScalar("xData13", ImGuiDataType_Float, &stExternalDatas.xData13);
            ImGui::InputScalar("ucData14", ImGuiDataType_U8, &stExternalDatas.ucData14);
            
            ImGui::Unindent();

            dynamicsComplex.stExternalDatas = stExternalDatas;
          }

          InputPerso("p_stPlatform", &dynamicsComplex.p_stPlatform);

          InputCompletePosition("stAbsolutePreviousMatrix", &dynamicsComplex.stAbsolutePreviousMatrix);
          InputCompletePosition("stPrevPreviousMatrix", &dynamicsComplex.stPrevPreviousMatrix);

          ImGui::Unindent();

          dynamics->stDynamicsComplex = dynamicsComplex;
        }
      }
      
      ImGui::Unindent();
    }
  }

}

void DR_DLG_Inspector_Draw_MS_3dData(HIE_tdstEngineObject* actor)
{
  GAM_tdst3dData* h3dData = actor->h3dData;

  if (h3dData == nullptr) {
    ImGui::Text("3dData is null");
    return;
  }

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