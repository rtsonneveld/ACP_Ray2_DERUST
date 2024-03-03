#include "features.h"

// Macro for feature initialization
#define DR_FEATURE_INIT(id, name) \
    {DR_E_Feature_##id, name, FALSE, DR_FEATURE_FUNCTIONS(##id##) }

#define X(feature, name) DR_FEATURE_INIT(feature, name),

DR_Feature DR_Features[] = {
	DR_FEATURE_LIST
};

DR_E_Feature DR_DisplayedFeature;

// Public

void CALLBACK ModeDisplay(SPTXT_tdstTextInfo* p_stString)
{
	DR_FEATURE_ForEach(i, feature)
{
		p_stString->X = 30;
		p_stString->Y = 600 + i * 30;
		p_stString->xSize = 10.0f;
		p_stString->ucAlpha = i == DR_DisplayedFeature ? 255 : 127;
		p_stString->bFrame = TRUE;

		if (i == DR_DisplayedFeature) {
			SPTXT_vPrintFmtLine("%s%s%s", TXT_Seq_Red, feature.name, TXT_Seq_Reset);
		} else {
			SPTXT_vPrintFmtLine(feature.name);
		}
	}
}

void DR_Features_Init() {
	DR_DisplayedFeature = DR_E_Feature_None;

	SPTXT_vAddTextCallback(ModeDisplay);
}

void DR_Features_Update() {


	if (IPT_M_bActionJustValidated(IPT_E_Entry_Action_Nage_Plonger)) {
		DR_DisplayedFeature++;
	}

	if (IPT_M_bActionIsValidated(IPT_E_Entry_Action_Affiche_Jauge)) {
		if (IPT_M_bActionJustValidated(IPT_E_Entry_Action_Clavier_Haut)) {
			DR_DisplayedFeature--;
		}
		if (IPT_M_bActionJustValidated(IPT_E_Entry_Action_Clavier_Bas)) {
			DR_DisplayedFeature++;
		}
	}

	if (DR_DisplayedFeature < 0) {
		DR_DisplayedFeature = DR_E_Feature_LENGTH - 1;
	}
	if (DR_DisplayedFeature >= DR_E_Feature_LENGTH) {
		DR_DisplayedFeature = 0;
	}

	//DR_FEATURE_ForEach(i, feature) {
		//feature.updateFunc();
	//}

	DR_Feature displayedFeature = DR_Features[DR_DisplayedFeature];
	displayedFeature.displayFunc();
}