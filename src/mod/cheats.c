#include "cheats.h"
#include "globals.h"
#include "util.h"
#include "dsgvarnames.h"

char g_DR_Cheats_InfiniteHealth = FALSE;
char g_DR_Cheats_MegaShoots = FALSE;
char g_DR_Cheats_DisableStartingCutscenes = FALSE;

BOOL DisableObject(HIE_tdstSuperObject* spo) {
  if (spo != NULL) {

    if (spo->hLinkedObject.p_stActor != NULL && spo->hLinkedObject.p_stActor->hStandardGame != NULL) {
      GAM_tdstStandardGame* std = spo->hLinkedObject.p_stActor->hStandardGame;
      if (!(std->ulCustomBits & Std_C_CustBit_NoAI)) {
        std->ulCustomBits |= Std_C_CustBit_NoAI;
        fn_vKillEngineObjectOrAlwaysByPointer(spo->hLinkedObject.p_stActor);
        return TRUE;
      }
    }
  }

  return FALSE;
}

BOOL DisableObjectOfModelType(const char* modelType)
{
  HIE_tdstSuperObject* spo = HIE_fn_p_stFindObjectByModelType(HIE_fn_lFindModelTypeByName(modelType));
  return DisableObject(spo);
}

BOOL DisableObjectOfPersonalType(const char* personalType)
{
  HIE_tdstSuperObject* spo = HIE_fn_p_stFindObjectByPersonalType(HIE_fn_lFindPersonalTypeByName(personalType));
  return DisableObject(spo);
}

void DR_Cheats_Apply() {
    if (g_DR_Cheats_InfiniteHealth) {
        if (g_DR_rayman != NULL) {
          GAM_tdstStandardGame* stdGame = g_DR_rayman->hLinkedObject.p_stActor->hStandardGame;
          stdGame->ucHitPoints = stdGame->ucHitPointsMax = stdGame->ucHitPointsMaxMax;
        }
    }

    if (g_DR_Cheats_MegaShoots) {
      if (g_DR_global != NULL) {

        ACT_SetBooleanInArray(g_DR_global->hLinkedObject.p_stActor, DV_GLOBAL_GLOBAL_Bits, GB_FLAG_MEGASHOOT_HIBIT, TRUE);
        ACT_SetBooleanInArray(g_DR_global->hLinkedObject.p_stActor, DV_GLOBAL_GLOBAL_Bits, GB_FLAG_MEGASHOOT_LOBIT, TRUE);
      }
    }

    if (g_DR_Cheats_DisableStartingCutscenes) {
      
      HIE_tdstSuperObject* debutSPO = HIE_fn_p_stFindObjectByModelType(HIE_fn_lFindModelTypeByName("ARG_DebutDeMap"));
      if (debutSPO != NULL) {
        if (DisableObject(debutSPO)) {
          char* raymanReacts = (char*)ACT_DsgVarPtr(g_DR_rayman->hLinkedObject.p_stActor, DV_RAY_RAY_ReagitAuxCommandes);
          *raymanReacts = TRUE;

          WP_tdstGraph* graph = *((WP_tdstGraph**)ACT_DsgVarPtr(debutSPO->hLinkedObject.p_stActor, 0));
          MTH3D_tdstVector lastWaypointPos = graph->m_hListOfNode.hLastElementDyn->m_hWayPoint->m_stVertex;
          ACT_Teleport(g_DR_rayman, lastWaypointPos);
        }
      }

      DisableObjectOfModelType("ARG_DebutDeMap"); 
      DisableObjectOfModelType("MIC_CameraCinoche");
      DisableObjectOfModelType("MIC_GenCamera");
      DisableObjectOfModelType("MIC_RetientRay");
      DisableObjectOfModelType("LevelLy_10");
      DisableObjectOfModelType("LevelLy_20");
      DisableObjectOfPersonalType("FRG_CamMic_Debut");

      HIE_tdstSuperObject* spiralDoor = HIE_fn_p_stFindObjectByModelType(HIE_fn_lFindModelTypeByName("YAM_PastilleVersLaMapMonde"));
      if (spiralDoor != NULL) {
        int* dsgVar0 = (int*)ACT_DsgVarPtr(spiralDoor->hLinkedObject.p_stActor, 0);
        *dsgVar0 = 0;
      }
    }
}

char DR_Cheats_GetFlag(int index) {

  return ACT_GetBooleanInArray(g_DR_global->hLinkedObject.p_stActor, DV_GLOBAL_GLOBAL_Bits, index);
}

void DR_Cheats_ToggleFlag(int index) {
  DR_Cheats_SetFlag(index, DR_Cheats_GetFlag(index) ? FALSE : TRUE);
}

void DR_Cheats_SetFlag(int index, char state) {

  ACT_SetBooleanInArray(g_DR_global->hLinkedObject.p_stActor, DV_GLOBAL_GLOBAL_Bits, index, state);
}