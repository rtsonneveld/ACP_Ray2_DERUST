#include "util.h"
#include <stdint.h>
#include <stdbool.h>

HIE_tdstEngineObject* SPO_Actor(HIE_tdstSuperObject* obj) {
	return obj->hLinkedObject.p_stActor;
}

void UTIL_vDrawBar(float x, float y, float barWidth, float barHeight, GEO_tdstColor color) {

    GLI_tdstMaterial stMaterial = { 0 };
    GLI_fn_vInitMaterialDefaults(&stMaterial);
    GLI_fn_vSetForcedColor(NULL);
	unsigned char alpha = (unsigned char)(color.xA * 255);
	GLI_vSetGlobalAlpha(alpha > 254 ? 254 : alpha);

    stMaterial.stAmbient = color;
    GLI_vDraw2DSpriteWithPercent(&GAM_g_stEngineStructure->stFixViewportAttr, x, y, x + barWidth, y + barHeight, &stMaterial);

    GLI_vSetGlobalAlpha(255);
}

void* ACT_DsgVarPtr(HIE_tdstEngineObject* actor, int dsgVarIndex)
{
  if (!actor || !actor->hBrain || !actor->hBrain->p_stMind)
    return NULL;

  AI_tdstAIModel* aiModel = actor->hBrain->p_stMind->p_stAIModel;
  AI_tdstDsgMem* dsgMem = actor->hBrain->p_stMind->p_stDsgMem;

  if (!aiModel || !aiModel->p_stDsgVar)
    return NULL;

  if (dsgVarIndex < 0 || dsgVarIndex >= aiModel->p_stDsgVar->ucNbDsgVar)
    return NULL;

  AI_tdstDsgVarInfo info = aiModel->p_stDsgVar->a_stDsgVarInfo[dsgVarIndex];
  return (char*)(dsgMem->p_cDsgMemBuffer + info.ulOffsetInDsgMem);
}

void ACT_SetBooleanInArray(HIE_tdstEngineObject* actor, int dsgVarIndex, unsigned long bitIndex, char state)
{
  tdstDsgVarArray* array = ACT_DsgVarPtr(actor, dsgVarIndex);
  if (!array) return;

  unsigned long pos = bitIndex / 32;
  unsigned long bit = bitIndex % 32;

  unsigned long size = sizeof(AI_tduGetSetParam);
  
  unsigned long* value = &(array->d_ArrayElement[pos]);

  if (state)
    array->d_ArrayElement[pos].ulValue |= (1u << bit);
  else
    array->d_ArrayElement[pos].ulValue &= ~(1u << bit);
}

/* --- Get a boolean at a specific bit --- */
char ACT_GetBooleanInArray(HIE_tdstEngineObject* actor, int dsgVarIndex, unsigned long bitIndex)
{
  tdstDsgVarArray* array = ACT_DsgVarPtr(actor, dsgVarIndex);
  if (!array) return false;

  unsigned long pos = bitIndex / 32;
  unsigned long bit = bitIndex % 32;

  return (array->d_ArrayElement[pos].ulValue & (1u << bit)) != 0;
}

/* --- Count the number of bits set in a range --- */
unsigned long ACT_GetNumberOfBooleanInArray(HIE_tdstEngineObject* actor, int dsgVarIndex,
  unsigned long firstIndex, unsigned long lastIndex)
{
  tdstDsgVarArray* array = ACT_DsgVarPtr(actor, dsgVarIndex);
  if (!array) return 0;

  unsigned long firstPos = firstIndex / 32;
  unsigned long firstBit = firstIndex % 32;
  unsigned long lastPos = lastIndex / 32;
  unsigned long lastBit = lastIndex % 32;

  unsigned long count = 0;

  for (unsigned long pos = firstPos; pos <= lastPos; pos++)
  {
    uint32_t value = array->d_ArrayElement[pos].ulValue;
    unsigned long startBit = (pos == firstPos) ? firstBit : 0;
    unsigned long endBit = (pos == lastPos) ? lastBit : 31;

    for (unsigned long b = startBit; b <= endBit; b++)
    {
      if (value & (1u << b))
        count++;
    }
  }

  return count;
}


/// <summary>
/// Sets the transparency of a SuperObject
/// </summary>
/// <param name="spo">The SuperObject</param>
/// <param name="alpha">Alpha where 0 is fully transparent and 1 is fully opaque</param>
void SPO_SetTransparency(HIE_tdstSuperObject* spo, float alpha) {
  spo->fTransparenceLevel = alpha * 255.0f;
  if (alpha < 1.0f) {
    spo->ulFlags = spo->ulFlags | HIE_C_Flag_ModuleTransparency;
    spo->hLinkedObject.p_stActor->h3dData->lDrawMask &= ~GLI_C_lIsNotGrided;
  }
  else {
    spo->ulFlags = spo->ulFlags & ~HIE_C_Flag_ModuleTransparency;
    spo->hLinkedObject.p_stActor->h3dData->lDrawMask |= GLI_C_lIsNotGrided;
  }
  PLA_fn_vUpdateTransparencyForModules(spo);
}