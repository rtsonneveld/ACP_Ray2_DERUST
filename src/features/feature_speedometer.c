#include "feature_speedometer.h"

void CALLBACK SpeedometerTextFunc(SPTXT_tdstTextInfo* p_stString) {

	p_stString->X = 20;
	p_stString->Y = 20;
	p_stString->xSize = 10.0f;
	p_stString->ucAlpha = 127;
	p_stString->bFrame = TRUE;
	SPTXT_vPrintFmtLine("%s%s%s", TXT_Seq_Red, "km/h", TXT_Seq_Reset);
}

DR_FEATURE_FUNC_OnActivate(Speedometer, feature) {
	SPTXT_vAddTextCallback(SpeedometerTextFunc);
}

DR_FEATURE_FUNC_OnDeactivate(Speedometer, feature) {
	SPTXT_vRemoveTextCallback(SpeedometerTextFunc);
}

DR_FEATURE_FUNC_Update(Speedometer, feature) {
}

DR_FEATURE_FUNC_Display(Speedometer, feature) {
}