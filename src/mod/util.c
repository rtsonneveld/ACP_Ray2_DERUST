#include "util.h"

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