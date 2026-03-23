#include <imgui.h>
#include <ACP_Ray2.h>
#include <LST.h>
#include <mod/globals.h>
#include <stdio.h>

void DR_DLG_EngineGlobals_Draw_Always()
{
  ALW_tdstAlways* alw = ALW_g_stAlways;
  
  if (ImGui::CollapsingHeader("hLstAlwaysModel")) {
    ALW_tdstAlwaysModelList* element;
    int index;

    // Start the table
    ImGui::BeginTable("AlwaysModelTable", 10, ImGuiTableFlags_SizingFixedFit);

    // Define the table headers
    ImGui::TableSetupColumn("Address", ImGuiTableColumnFlags_WidthFixed, 96);
    ImGui::TableSetupColumn("EngineObj*", ImGuiTableColumnFlags_WidthFixed, 96);
    ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 32);
    ImGui::TableSetupColumn("Family");
    ImGui::TableSetupColumn("AI Model", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Brain");
    ImGui::TableSetupColumn("Mind");
    ImGui::TableSetupColumn("DsgMem");
    ImGui::TableSetupColumn("DsgMem->DsgVar");
    ImGui::TableSetupColumn("DsgMem->DsgVar[0]");
    ImGui::TableHeadersRow();

    LST_M_DynamicForEachIndex(&alw->hLstAlwaysModel, element, index) {

      // Start a new table row
      ImGui::TableNextRow();

      // Fill the table cells
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("0x%x", element);

      ImGui::TableSetColumnIndex(1);
      ImGui::Text("0x%x", element->p_stAlwaysObject);

      ImGui::TableSetColumnIndex(2);
      ImGui::Text("%i", element->lObjectModelType);

      if (element->p_stAlwaysObject != NULL && element->p_stAlwaysObject->hStandardGame != NULL) {
        ImGui::TableSetColumnIndex(3);
        ImGui::Text("%s", HIE_fn_szGetFamilyTypeName(element->p_stAlwaysObject->hStandardGame->lObjectFamilyType));

        ImGui::TableSetColumnIndex(4);
        ImGui::Text("%s", HIE_fn_szGetModelTypeName(element->p_stAlwaysObject->hStandardGame->lObjectModelType));

        ImGui::TableSetColumnIndex(5);
        ImGui::Text("0x%x", element->p_stAlwaysObject->hBrain);

        ImGui::TableSetColumnIndex(6);
        ImGui::Text("0x%x", element->p_stAlwaysObject->hBrain != NULL ? element->p_stAlwaysObject->hBrain->p_stMind : NULL);

        ImGui::TableSetColumnIndex(7);
        ImGui::Text("0x%x", element->p_stAlwaysObject->hBrain != NULL && element->p_stAlwaysObject->hBrain->p_stMind != NULL ? element->p_stAlwaysObject->hBrain->p_stMind->p_stDsgMem : NULL);


        ImGui::TableSetColumnIndex(8);
        ImGui::Text("0x%x", element->p_stAlwaysObject->hBrain != NULL && element->p_stAlwaysObject->hBrain->p_stMind != NULL 
          && element->p_stAlwaysObject->hBrain->p_stMind->p_stDsgMem != NULL && element->p_stAlwaysObject->hBrain->p_stMind->p_stDsgMem->pp_stDsgVar != NULL ? element->p_stAlwaysObject->hBrain->p_stMind->p_stDsgMem->pp_stDsgVar : NULL);

        ImGui::TableSetColumnIndex(9);
        ImGui::Text("0x%x", element->p_stAlwaysObject->hBrain != NULL && element->p_stAlwaysObject->hBrain->p_stMind != NULL
          && element->p_stAlwaysObject->hBrain->p_stMind->p_stDsgMem != NULL && element->p_stAlwaysObject->hBrain->p_stMind->p_stDsgMem->pp_stDsgVar != NULL ? element->p_stAlwaysObject->hBrain->p_stMind->p_stDsgMem->pp_stDsgVar[0] : NULL);
      }
      else {

        ImGui::TableSetColumnIndex(2);
        ImGui::Text("Invalid EngineObject");

        ImGui::TableSetColumnIndex(3);
        ImGui::Text("Invalid EngineObject");
      }
    }

    // End the table
    ImGui::EndTable();
  }

  if (ImGui::CollapsingHeader("p_stAlwaysEngineObjectInit")) {
    ALW_tdstAlwaysModelList* element;
    int index;

    // Start the table
    ImGui::BeginTable("AlwaysEngineObjectInitTable", 7, ImGuiTableFlags_SizingFixedFit);

    // Define the table headers
    ImGui::TableSetupColumn("Address", ImGuiTableColumnFlags_WidthFixed, 96);
    ImGui::TableSetupColumn("Family");
    ImGui::TableSetupColumn("AI Model", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Brain*");
    ImGui::TableSetupColumn("Mind*");
    ImGui::TableSetupColumn("AI Model*");
    ImGui::TableSetupColumn("DsgMem->DsgVar*");
    ImGui::TableHeadersRow();

    HIE_tdstEngineObject* obj = alw->p_stAlwaysEngineObjectInit;
    int i;

    for (i = 0; i < alw->ulMaxNbOfAlways; i++, obj++) {

      // Start a new table row
      ImGui::TableNextRow();

      // Fill the table cells
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("0x%x", obj);

      if (obj != NULL && obj->hStandardGame != NULL) {
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%s", HIE_fn_szGetFamilyTypeName(obj->hStandardGame->lObjectFamilyType));

        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%s", HIE_fn_szGetModelTypeName(obj->hStandardGame->lObjectModelType));

        ImGui::TableSetColumnIndex(3);
        ImGui::Text("0x%x", obj->hBrain);

        ImGui::TableSetColumnIndex(4);
        ImGui::Text("0x%x", obj->hBrain!=NULL?obj->hBrain->p_stMind:NULL);

        AI_tdstAIModel* model = obj->hBrain != NULL && obj->hBrain->p_stMind != NULL ? obj->hBrain->p_stMind->p_stAIModel : NULL;
        ImGui::TableSetColumnIndex(5);
        ImGui::Text("0x%x", model);
        
        bool modelIsValid = false;
        LST_M_DynamicForEachIndex(&alw->hLstAlwaysModel, element, index) {

          if (element->p_stAlwaysObject == NULL) {
            continue;
          }

          if (element->p_stAlwaysObject->hBrain == NULL) {
            continue;
          }

          if (element->p_stAlwaysObject->hBrain->p_stMind == NULL) {
            continue;
          }

          if (element->p_stAlwaysObject->hBrain->p_stMind->p_stAIModel == model) {
            modelIsValid = true;
          }
        }
        ImGui::TableSetColumnIndex(6);
        ImGui::Text("0x%x", modelIsValid ? obj->hBrain->p_stMind->p_stDsgMem->pp_stDsgVar[0] : NULL);
      }
      else {

        ImGui::TableSetColumnIndex(1);
        ImGui::Text("Invalid EngineObject");

        ImGui::TableSetColumnIndex(2);
        ImGui::Text("Invalid EngineObject");
      }
    }

    // End the table
    ImGui::EndTable();
  }

  if (ImGui::CollapsingHeader("p_stAlwaysSuperObject")) {
    
    ImGui::BeginTable("AlwaysSuperObjectSlots", 8, ImGuiTableFlags_SizingFixedFit);
    
    ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed, 32);
    ImGui::TableSetupColumn("Address", ImGuiTableColumnFlags_WidthFixed, 128);
    ImGui::TableSetupColumn("EngineObject", ImGuiTableColumnFlags_WidthFixed, 128);

    ImGui::TableSetupColumn("Brain*");
    ImGui::TableSetupColumn("Mind*");
    ImGui::TableSetupColumn("AiModel*");

    ImGui::TableSetupColumn("Family");
    ImGui::TableSetupColumn("Model");

    ImGui::TableHeadersRow();

    HIE_tdstSuperObject* spo = alw->p_stAlwaysSuperObject;
    int i;

    for (i = 0; i < ALW_g_stAlways->ulMaxNbOfAlways; i++, spo++) {

      ImGui::TableNextRow();

      bool active = ALW_g_stAlways->a_stAlwaysGenerator[i] != NULL;
      
      HIE_tdstEngineObject* obj = spo->hLinkedObject.p_stActor;
      GAM_tdstStandardGame* stdGame = obj != NULL ? obj->hStandardGame : NULL;

      if (stdGame == NULL) {
        active = FALSE;
      }

      ImGui::BeginDisabled(!active);

      ImGui::TableSetColumnIndex(0);

      char label[32];
      sprintf(label, "##alwsposelection%i", i);
      bool is_selected = (g_DR_selectedObject == spo);
      if (ImGui::Selectable(label, is_selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap))
      {
        g_DR_selectedObject = spo;
      }
      ImGui::SameLine();
      ImGui::Text("%i", i);
      
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("0x%x", spo);

      ImGui::TableSetColumnIndex(2);
      ImGui::Text("0x%x", obj);

      AI_tdstBrain* brain = obj != NULL ? obj->hBrain : NULL;

      ImGui::TableSetColumnIndex(3);
      ImGui::Text("0x%x", brain);

      AI_tdstMind* mind = brain != NULL ? brain->p_stMind : NULL;

      ImGui::TableSetColumnIndex(4);
      ImGui::Text("0x%x", mind);

      AI_tdstAIModel* aiModel = mind != NULL ? mind->p_stAIModel : NULL;

      ImGui::TableSetColumnIndex(5);
      ImGui::Text("0x%x", aiModel);

      ImGui::TableSetColumnIndex(6);
      ImGui::Text("%s", stdGame != NULL ? HIE_fn_szGetFamilyTypeName(stdGame->lObjectFamilyType) : "<inactive>");

      ImGui::TableSetColumnIndex(7);
      ImGui::Text("%s", stdGame != NULL ? HIE_fn_szGetModelTypeName(stdGame->lObjectModelType) : "<inactive>");

      ImGui::EndDisabled();
    }

    ImGui::EndTable();
  }
}
