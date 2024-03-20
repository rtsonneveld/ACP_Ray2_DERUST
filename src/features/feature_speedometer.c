#include "feature_speedometer.h"
#include "math.h"

extern HIE_tdstSuperObject* rayman;

#define MAX_SPEED 50

void CALLBACK SpeedometerTextFunc(SPTXT_tdstTextInfo* p_stString) {

	p_stString->X = DISPLAY_SIZE_X - 50;
	p_stString->Y = DISPLAY_SIZE_Y - 50;
	p_stString->xSize = 10.0f;
	p_stString->ucAlpha = 127;
	p_stString->bFrame = TRUE;
	p_stString->bRightAlign = TRUE;

	MTH3D_tdstVector speed = rayman->hLinkedObject.p_stActor->hDynam->p_stDynamics->stDynamicsBase.stPreviousSpeed;
	float flatSpeed = sqrtf(speed.x * speed.x + speed.y * speed.y);

	GEO_tdstColor colorBg = { 0.2f, 0.2f, 0.2f, 0.0f };
	GEO_tdstColor colorFg = { 0.3f, 1.0f, 0.3f, 0.0f };
	UTIL_vDrawBar(10, 10, 5 * MAX_SPEED, 5, colorBg);
	UTIL_vDrawBar(10, 10, 5 * flatSpeed, 5, colorFg);

	SPTXT_vPrintFmtLine(TXT_Seq_Red"%.2f%s"TXT_Seq_Reset, flatSpeed, " u;s");
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