#include "inspector_actor.hpp"
#include "ui/ui.hpp"
#include "ui/ui_util.hpp"
#include "ui/custominputs.hpp"

// C INCLUDE
#include "mod/dsgvarnames.h"
#include "mod/globals.h"
#include "mod/cpa_functions.h"
#include <ACP_Ray2.h>


bool forceOverwriteDynam = FALSE;
DNM_tdstDynamicsBaseBlock dynamicsBase;
DNM_tdstDynamicsAdvancedBlock dynamicsAdvanced;
DNM_tdstDynamicsComplexBlock dynamicsComplex;

DNM_tdstDynamics savedDynamics;

#include "imgui.h"

void DrawObstacle(const char* label, DNM_tdstObstacle obs, bool collision)
{
  
  if (ImGui::TreeNode(label, "%s%s", label, collision ? " (hit)":"")) {
    ImGui::Text("Rate: %.3f", obs.xRate);
    ImGui::DragFloat3("Normal", (float*)&(obs.stNorm.x));
    ImGui::DragFloat3("Contact", (float*) & (obs.stContact.x));

    if (ImGui::TreeNode("My Material")) {
      InputGameMaterial(obs.hMyMaterial);
      ImGui::TreePop();
    }

    if (ImGui::TreeNode("Collided Material")) {
      InputGameMaterial(obs.hCollidedMaterial);
      ImGui::TreePop();
    }

    ImGui::Text("SuperObject: %p", (void*)obs.p_stSupObj);

    ImGui::TreePop();
  }
}

void DrawMove(const char* label, MTH_tdstMove move)
{
  if (ImGui::TreeNode(label)) {
    ImGui::DragFloat3("Linear", &(move.stLinear.x));
    ImGui::Text("Angular Angle: %.3f", move.stAngular.xAngle);
    ImGui::DragFloat3("Angular Axis", &(move.stAngular.stAxis.x));
    ImGui::TreePop();
  }
}

void SurfaceStateText(const char* label, unsigned long state) {
  std::string stateText;

  if (state & MEC_C_WOT_ulError)        stateText += "Error | ";
  if (state & MEC_C_WOT_ulGround)       stateText += "Ground | ";
  if (state & MEC_C_WOT_ulWall)         stateText += "Wall | ";
  if (state & MEC_C_WOT_ulCeiling)      stateText += "Ceiling | ";
  if (state & MEC_C_WOT_ulWater)        stateText += "Water | ";
  if (state & MEC_C_WOT_ulForceMobile)  stateText += "ForceMobile | ";
  if (state & MEC_C_WOT_ulMobile)       stateText += "Mobile | ";
  if ((state & MEC_C_WOT_ulFather) == MEC_C_WOT_ulFather) stateText += "Father | "; // Unused anyways

  if (stateText.empty())
    stateText = "NoObstacle";
  else
    stateText.erase(stateText.size() - 3); // remove last " | "

  ImGui::Text("%s : 0x%lX (%s)", label, state, stateText.c_str());
}

// Main report draw
void DrawDnmReport(DNM_tdstReport report)
{
  ImGui::Text("Surface States:");
  ImGui::Indent();
  SurfaceStateText("Previous", report.ulPrevSurfaceState);
  SurfaceStateText("Current", report.ulCurrSurfaceState);
  ImGui::Unindent();

  ImGui::Separator();

  DrawObstacle("Obstacle", report.stObstacle, report.ulCurrSurfaceState != 0);
  DrawObstacle("Ground", report.stGround, report.ulCurrSurfaceState & MEC_C_WOT_ulGround);
  DrawObstacle("Wall", report.stWall, report.ulCurrSurfaceState & MEC_C_WOT_ulWall);
  DrawObstacle("Character", report.stCharacter, report.ulCurrSurfaceState & MEC_C_WOT_ulMobile);
  DrawObstacle("Water", report.stWater, report.ulCurrSurfaceState & MEC_C_WOT_ulWater);
  DrawObstacle("Ceiling", report.stCeil, report.ulCurrSurfaceState & MEC_C_WOT_ulCeiling);

  ImGui::Separator();

  DrawMove("Absolute Previous Speed", report.stAbsolutePrevSpeed);
  DrawMove("Absolute Current Speed", report.stAbsoluteCurrSpeed);
  DrawMove("Absolute Previous Position", report.stAbsolutePrevPosition);
  DrawMove("Absolute Current Position", report.stAbsoluteCurrPosition);

  ImGui::Separator();

  ImGui::Text("Bitfield: 0x%02X", report.ucBitField);
}


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

      if (ImGui::Button("Save")) {
        savedDynamics = *dynamics;
      } ImGui::SameLine();
      if (ImGui::Button("Load")) {
        dynam->p_stDynamics->stDynamicsBase = savedDynamics.stDynamicsBase;
        dynam->p_stDynamics->stDynamicsAdvanced = savedDynamics.stDynamicsAdvanced;
        dynam->p_stDynamics->stDynamicsComplex = savedDynamics.stDynamicsComplex;
      }

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

        ImGui::DragFloat3("stImposeSpeed", (float*)&(dynamicsBase.stImposeSpeed.x));
        ImGui::DragFloat3("stProposeSpeed", (float*)&(dynamicsBase.stProposeSpeed.x));
        ImGui::DragFloat3("stPreviousSpeed", (float*)&(dynamicsBase.stPreviousSpeed.x));
        ImGui::DragFloat3("stScale", (float*)&(dynamicsBase.stScale.x));
        ImGui::DragFloat3("stSpeedAnim", (float*)&(dynamicsBase.stSpeedAnim.x));
        ImGui::DragFloat3("stSafeTranslation", (float*)&(dynamicsBase.stSafeTranslation.x));
        ImGui::DragFloat3("stAddTranslation", (float*)&(dynamicsBase.stAddTranslation.x));

        InputCompletePosition("stPreviousMatrix", &dynamicsBase.stPreviousMatrix);
        InputCompletePosition("stCurrentMatrix", &dynamicsBase.stCurrentMatrix);
        InputMatrix("stImposeRotationMatrix", &dynamicsBase.stImposeRotationMatrix);

        if (ImGui::CollapsingHeader("p_stReport")) {
          DrawDnmReport(*dynamicsBase.p_stReport);
        }

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

          ImGui::DragFloat3("stMaxSpeed", &(dynamicsAdvanced.stMaxSpeed.x));
          ImGui::DragFloat3("stStreamSpeed", &(dynamicsAdvanced.stStreamSpeed.x));
          ImGui::DragFloat3("stAddSpeed", &(dynamicsAdvanced.stAddSpeed.x));
          ImGui::DragFloat3("stLimit", &(dynamicsAdvanced.stLimit.x));

          ImGui::DragFloat3("stCollisionTranslation", &(dynamicsAdvanced.stCollisionTranslation.x));
          ImGui::DragFloat3("stInertiaTranslation", &(dynamicsAdvanced.stInertiaTranslation.x));
          ImGui::DragFloat3("stGroundNormal", &(dynamicsAdvanced.stGroundNormal.x));
          ImGui::DragFloat3("stWallNormal", &(dynamicsAdvanced.stWallNormal.x));

          ImGui::InputScalar("ucCollideCounter", ImGuiDataType_U8, &(dynamicsAdvanced.ucCollideCounter));

          ImGui::Unindent();

          dynamics->stDynamicsAdvanced = dynamicsAdvanced;
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

          ImGui::DragFloat3("stContact", &(dynamicsComplex.stContact.x));
          ImGui::DragFloat3("stFallTranslation", &(dynamicsComplex.stFallTranslation.x));

          if (ImGui::CollapsingHeader("stExternalDatas")) {

            ImGui::Indent();
            DNM_tdstMACDPID stExternalDatas = dynamicsComplex.stExternalDatas;

            ImGui::InputScalar("xData0", ImGuiDataType_Float, &stExternalDatas.xData0);
            ImGui::DragFloat3("stData1", &(stExternalDatas.stData1.x));
            ImGui::DragFloat3("stData2", &(stExternalDatas.stData2.x));
            ImGui::DragFloat3("stData3", &(stExternalDatas.stData3.x));
            ImGui::InputScalar("xData3", ImGuiDataType_Float, &stExternalDatas.xData3);
            ImGui::InputScalar("xData4", ImGuiDataType_Float, &stExternalDatas.xData4);
            ImGui::InputScalar("xData5", ImGuiDataType_Float, &stExternalDatas.xData5);

            ImGui::InputScalar("stData6.xAngle", ImGuiDataType_Float, &stExternalDatas.stData6.xAngle);
            ImGui::DragFloat3("stData6.stAxis", &(stExternalDatas.stData6.stAxis.x));

            ImGui::InputScalar("stData7.xAngle", ImGuiDataType_Float, &stExternalDatas.stData7.xAngle);
            ImGui::DragFloat3("stData7.stAxis", &(stExternalDatas.stData7.stAxis.x));

            ImGui::InputScalar("cData8", ImGuiDataType_S8, &stExternalDatas.cData8);
            ImGui::InputScalar("uwData9", ImGuiDataType_U16, &stExternalDatas.uwData9);
            ImGui::DragFloat3("stData10", &(stExternalDatas.stData10.x));
            ImGui::InputScalar("xData11", ImGuiDataType_Float, &stExternalDatas.xData11);
            ImGui::DragFloat3("stData12", &(stExternalDatas.stData12.x));
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