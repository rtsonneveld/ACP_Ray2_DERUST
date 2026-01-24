#include "ai_disablerandopt.h"
#include "cheats.h"

AI_tdstNodeInterpret* MOD_fn_p_stEvalKeyWord_DisableMicroRNG(HIE_tdstSuperObject* spo, AI_tdstNodeInterpret* node, AI_tdstGetSetParam* param) {

  if (!g_DR_Cheats_DisableRandomOptimisations) {
    return AI_fn_p_stEvalKeyWord(spo, node, param);
  }

  if (node->eType != AI_E_ti_KeyWord) {
    return AI_fn_p_stEvalKeyWord(spo, node, param);
  }
  
  if (node->uParam.ulValue < AI_E_kw_If2 || node->uParam.ulValue > AI_E_kw_If16) {
    return AI_fn_p_stEvalKeyWord(spo, node, param);
  }

  // Store parameter
  unsigned long original = node->uParam.ulValue;

  node->uParam.ulValue = AI_E_kw_If;
  AI_tdstNodeInterpret * result = AI_fn_p_stEvalKeyWord(spo, node, param);

  // Restore parameter
  node->uParam.ulValue = original;

  return result;
}

BOOL MOD_fn_b3dDataCanComputeBrain(HIE_tdstSuperObject* spo) {
  
  if (g_DR_Cheats_DisableRandomOptimisations) {
    return TRUE;
  }
  return GAM_fn_b3dDataCanComputeBrain(spo);
}