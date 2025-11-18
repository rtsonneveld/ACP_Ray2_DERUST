#include "inspector_actor.hpp"
#include "inspector_actor_dynam.hpp"
#include "ui/ui.hpp"
#include "ui/ui_util.hpp"
#include "ui/nameLookup.hpp"
#include "ui/custominputs.hpp"

// C INCLUDE
#include "mod/dsgvarnames.h"
#include "mod/globals.h"
#include "mod/cpa_functions.h"
#include "mod/ai_strings.h"
#include <ACP_Ray2.h>


bool displayDsgVarBufferInitial = false;
bool displayDsgVarBufferModel = false;

void DrawComportPicker(HIE_tdstEngineObject* actor, AI_tdstMind* mind, AI_tdstAIModel* model, bool isReflex) {

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
  snprintf(label, sizeof(label), "%s##preview", (isReflex ? NameFromIndex(NameType::AIModel_Reflex, modelName, activeComport) : NameFromIndex(NameType::AIModel_Comport, modelName, activeComport)).c_str());
  if (ImGui::BeginCombo(isReflex ? "Active Reflex" : "Active Comport", label)) {

    for (int i = 0;i < scriptAI->ulNbComport;i++) {
      snprintf(label, sizeof(label), "%s##selection", (isReflex ? NameFromIndex(NameType::AIModel_Reflex, modelName, i) : NameFromIndex(NameType::AIModel_Comport, modelName, i)).c_str());
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
        ImGui::Text(AI_GetDsgVarTypeString(info.eTypeId));

        if (isRayman) {
          ImGui::SameLine();
          ImGui::Text(DV_STR_Rayman[i]);
        }
        else if (isGlobal) {
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