#include "feature_triggerdisplay.h"

#include "MTH.h"
#include <math.h>

GLI_tdstMaterial* stCircleYellow;
GLI_tdstMaterial* stCircleRed;
GLI_tdstMaterial* stCircleBlue;

#define GLI_C_lIsTextured							1
#define C_TooFarLimitMargin 5.0f
#define C_TooFarLimitMargin 5.0f
#define C_TooFarLimitParticleScale 0.5f

DR_FEATURE_FUNC_OnActivate(TriggerDisplay, feature) {

	stCircleYellow = (GLI_tdstMaterial*)malloc(sizeof(GLI_tdstMaterial));
	GLI_fn_vInitMaterialDefaults(stCircleYellow);
	stCircleYellow->ulMaterialType = GLI_C_Mat_lIsTextured;

	stCircleRed = (GLI_tdstMaterial*)malloc(sizeof(GLI_tdstMaterial));
	GLI_fn_vInitMaterialDefaults(stCircleRed);
	stCircleRed->ulMaterialType = GLI_C_Mat_lIsTextured;

	stCircleBlue = (GLI_tdstMaterial*)malloc(sizeof(GLI_tdstMaterial));
	GLI_fn_vInitMaterialDefaults(stCircleBlue);
	stCircleBlue->ulMaterialType = GLI_C_Mat_lIsTextured;

	stCircleYellow->p_stTexture = GLI_fn_pstFindTexture("effets_speciaux\\faisc_centre_txy_ad.tga");;
	stCircleRed->p_stTexture = GLI_fn_pstFindTexture("effets_speciaux\\faisc_centre_mort_adnzw.tga");
	stCircleBlue->p_stTexture = GLI_fn_pstFindTexture("effets_speciaux\\faisc_centre_mort_bleu_adnzw.tga");
}

DR_FEATURE_FUNC_OnDeactivate(TriggerDisplay, feature) {

}

DR_FEATURE_FUNC_Update(TriggerDisplay, feature) {

	HIE_tdstSuperObject* p_stSuperObj;

	LST_M_DynamicForEach((*GAM_g_p_stDynamicWorld), p_stSuperObj)
	{
		if (p_stSuperObj == rayman) {
			continue;
		}

		if (p_stSuperObj->ulType != HIE_C_Type_Actor) {
			continue;
		}

		HIE_tdstEngineObject* p_stActor = HIE_M_hSuperObjectGetActor(p_stSuperObj);

		if (!p_stActor->hStandardGame->ulCustomBits & (Std_C_CustBit_NoAIWhenTooFar | Std_C_CustBit_NoAnimPlayerWhenTooFar | Std_C_CustBit_NoMecaWhenTooFar)) {
			continue;
		}

		float tooFarLimit = (float)(p_stActor->hStandardGame->ucTooFarLimit) * 0.5f;
		if (tooFarLimit > 0) {

			MTH3D_tdstVector raymanPos = rayman->p_stGlobalMatrix->stPos;
			MTH3D_tdstVector pos = p_stSuperObj->p_stGlobalMatrix->stPos;

			float deltaX = raymanPos.x - pos.x;
			float deltaY = raymanPos.y - pos.y;
			float deltaZ = raymanPos.z - pos.z;

			float linearDist = (fabsf(deltaX) + fabsf(deltaY) + fabsf(deltaZ)) * 0.5f;
			float realDist = sqrtf(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);
			float normalizedX = deltaX / realDist;
			float normalizedY = deltaY / realDist;
			float normalizedZ = deltaZ / realDist;

			if (linearDist < tooFarLimit + C_TooFarLimitMargin && linearDist > tooFarLimit - C_TooFarLimitMargin) {

				float alpha = 1.0f - (fabsf(linearDist - tooFarLimit) / C_TooFarLimitMargin);
				float scale = alpha * C_TooFarLimitParticleScale;
				float dist = 1.0f / (fabsf(normalizedX) + fabsf(normalizedY) + fabsf(normalizedZ));

				MTH3D_tdstVector diamondPos = (MTH3D_tdstVector){
					pos.x + (normalizedX * 2.0f * tooFarLimit * dist),
					pos.y + (normalizedY * 2.0f * tooFarLimit * dist),
					pos.z + (normalizedZ * 2.0f * tooFarLimit * dist),
				};

				AGO_vCreatePart(AGO_STILL | AGO_TIMELIMITED, &diamondPos, &MTH3D_C_ZeroVector, 0.02f, alpha * C_TooFarLimitParticleScale, scale, linearDist > tooFarLimit ? stCircleBlue : stCircleRed);
			}
		}
	}

	return NULL;
}

DR_FEATURE_FUNC_Display(TriggerDisplay, feature) {
}