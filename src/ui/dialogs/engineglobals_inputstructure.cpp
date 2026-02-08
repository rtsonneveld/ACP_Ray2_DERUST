#include <imgui.h>
#include <ACP_Ray2.h>

void DR_DLG_EngineGlobals_Draw_InputStructure()
{
  GAM_tdstEngineStructure* eng = GAM_g_stEngineStructure;
  IPT_tdstInput* ipt = IPT_g_stInputStructure;

  // Start the table
  ImGui::BeginTable("InputStructureTable", 4); // 4 columns

  // Define the table headers
  ImGui::TableSetupColumn("Entry");
  ImGui::TableSetupColumn("Name");
  ImGui::TableSetupColumn("State");
  ImGui::TableSetupColumn("Analog Value");
  ImGui::TableHeadersRow();

  for (int i = 0; i < ipt->ulNumberOfEntryElement; i++) {
    IPT_tdstEntryElement el = ipt->d_stEntryElementArray[i];

    // Start a new table row
    ImGui::TableNextRow();

    // Fill the table cells
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("Entry %d", i);

    ImGui::TableSetColumnIndex(1);
    ImGui::Text("%s", el.p_szEntryName);

    ImGui::TableSetColumnIndex(2);
    ImGui::Text("%i", el.lState);

    ImGui::TableSetColumnIndex(3);
    ImGui::Text("%.2f", el.xAnalogicValue);
  }

  // End the table
  ImGui::EndTable();
}
