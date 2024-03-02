#pragma once

#include "cpa_functions.h"

HIE_tdstEngineObject* SPO_Actor(HIE_tdstSuperObject* obj) {
	return obj->hLinkedObject.p_stActor;
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
		spo->ulFlags &= ~HIE_C_Flag_ModuleTransparency;
		spo->hLinkedObject.p_stActor->h3dData->lDrawMask |= GLI_C_lIsNotGrided;
	}
	PLA_fn_vUpdateTransparencyForModules(spo);
}