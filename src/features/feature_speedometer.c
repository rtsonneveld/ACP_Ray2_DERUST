#include "feature_speedometer.h"
#include "math.h"
#include "../mathutil.h"
#include "../dsgvarnames.h"

extern HIE_tdstSuperObject* rayman;

#define SPEED_HOVERING 5.01f
#define SPEED_STRAFING 7.01f
#define SPEED_AIR 8.01f
#define SPEED_WALKING 10.01f
#define SPEED_ROLLBOOST 12.67f // 12.45 - 12.66
#define SPEED_MAX_NORMAL 15.0f
#define SPEED_MAX_MAX 100.0f

#define COLOR_GRAY	 (GEO_tdstColor){ 0.2f, 0.2f, 0.2f, 1.0f }
#define COLOR_LTGRAY (GEO_tdstColor){ 0.7f, 0.7f, 0.7f, 1.0f }
#define COLOR_RED	 (GEO_tdstColor){ 1.0f, 0.1f, 0.1f, 1.0f } // <= hovering speed
#define COLOR_YELLOW (GEO_tdstColor){ 1.0f, 0.8f, 0.1f, 1.0f } // <= air speed
#define COLOR_GREEN  (GEO_tdstColor){ 0.3f, 1.0f, 0.3f, 1.0f } // <= walking speed
#define COLOR_CYAN	 (GEO_tdstColor){ 0.0f, 1.0f, 1.0f, 1.0f } // <= roll boost speed
#define COLOR_PINK	 (GEO_tdstColor){ 1.0f, 0.0f, 1.0f, 1.0f } // > roll boost speed
#define COLOR_WHITE	 (GEO_tdstColor){ 1.0f, 1.0f, 1.0f, 1.0f } // > roll boost speed

#define MAX_SPEED_SLIDE 100.0f
#define SPDMTR_BAR_OFFSET_X 5.0f
#define SPDMTR_BAR_OFFSET_Y 95.0f
#define SPDMTR_TEXT_OFFSET_Y 100.0f
#define SPDMTR_BAR_HEIGHT 2.0f
#define SPDMTR_BARLINE_HEIGHT 1.0f
#define SPDMTR_BARLINE_WIDTH 0.2f
#define SPDMTR_BAR_MAXLENGTH 90.0f

// Function to interpolate between two colors
GEO_tdstColor interpolateColors(GEO_tdstColor color1, GEO_tdstColor color2, float t) {
	GEO_tdstColor result;
    result.xR = (color1.xR + t * (color2.xR - color1.xR));
    result.xG = (color1.xG + t * (color2.xG - color1.xG));
    result.xB = (color1.xB + t * (color2.xB - color1.xB));
    result.xA = (color1.xA + t * (color2.xA - color1.xA));
    return result;
}

// Function to interpolate between a range of colors (max 8)
GEO_tdstColor interpolateRangeOfColors(float value, int numColors, ...) {
    va_list args;
    va_start(args, numColors);

    // Array to hold colors (max 8)
	GEO_tdstColor colors[8];

    // Retrieve colors from variable arguments
    for (int i = 0; i < numColors; i++) {
        colors[i] = va_arg(args, GEO_tdstColor);
    }

    va_end(args);

    // Calculate interval size
    float interval = 1.0 / (numColors - 1);

    // Determine which interval the value falls into
    int index = (int)(value / interval);

    // Calculate interpolation factor
    float t = (value - index * interval) / interval;

    // Interpolate between the appropriate colors
    return interpolateColors(colors[index], colors[index + 1], t);
}


MTH3D_tdstVector lastPosition = { 0.0, 0.0, 0.0 };
float maxSpeed = SPEED_MAX_NORMAL;
float lastSpeed = 0;

void DRAW_BARLINE(float speed, float maxSpeed, float yOffset, float heightFactor, GEO_tdstColor color) {
	
	float xFactor = (float)speed / maxSpeed;
	float x = SPDMTR_BAR_OFFSET_X + (xFactor * SPDMTR_BAR_MAXLENGTH);
	UTIL_vDrawBar(x - SPDMTR_BARLINE_WIDTH * 0.5f, SPDMTR_BAR_OFFSET_Y + yOffset, SPDMTR_BARLINE_WIDTH, heightFactor * SPDMTR_BARLINE_HEIGHT, color);
}

void CALLBACK SpeedometerTextFunc(SPTXT_tdstTextInfo* p_stString) {

	p_stString->xSize = 10.0f;
	p_stString->ucAlpha = 127;
	p_stString->bFrame = TRUE;

	//MTH3D_tdstVector speed = rayman->hLinkedObject.p_stActor->hDynam->p_stDynamics->stDynamicsBase.stPreviousSpeed;
	DNM_tdstDynamicsBaseBlock base = rayman->hLinkedObject.p_stActor->hDynam->p_stDynamics->stDynamicsBase;

	MTH3D_tdstVector* slideSpeed;
	AI_fn_bGetDsgVar(rayman, (unsigned char)DV_RAY_INTERN_RaySlideSpeed, NULL, &slideSpeed);

	MTH3D_tdstVector speed;
	MTH3D_M_vSubVector(&speed, &base.stCurrentMatrix.stPos, &lastPosition);
	MTH3D_M_vMulVectorScalar(&speed, &speed, 60.0f); // 60 frames in one second
	lastPosition = base.stCurrentMatrix.stPos;
	float newSpeed = sqrtf(speed.x * speed.x + speed.y * speed.y);
	float flatSpeed = lastSpeed;

	if (newSpeed < lastSpeed + 5) { // Prevent speed from increasing too fast
		flatSpeed = newSpeed;
	}
	lastSpeed = newSpeed;

	p_stString->X = DISPLAY_SIZE_X - 50;
	p_stString->Y = DISPLAY_SIZE_Y - SPDMTR_TEXT_OFFSET_Y;
	p_stString->bRightAlign = TRUE;
	SPTXT_vPrintFmtLine(TXT_Seq_Red"%.2f%s"TXT_Seq_Reset, flatSpeed, " u;s");

	p_stString->X = 50;
	p_stString->Y = DISPLAY_SIZE_Y - SPDMTR_TEXT_OFFSET_Y;
	p_stString->bRightAlign = FALSE;

	SPTXT_vPrintFmtLine(TXT_Seq_Yellow"%.2f %.2f %.2f"TXT_Seq_Reset, slideSpeed->x, slideSpeed->y, slideSpeed->z);

	float maxSpeedTarget = MU_RemapRange(flatSpeed, 10, 40, SPEED_MAX_NORMAL, SPEED_MAX_MAX);

	maxSpeed += (maxSpeedTarget - maxSpeed) * 0.02f;
	if (maxSpeed < flatSpeed && maxSpeed < SPEED_MAX_MAX) {
		maxSpeed = flatSpeed;
	}

	float speedFactor = MU_ClampF(flatSpeed / maxSpeed, 0.0f, 1.0f);
	float speedFactorForColor = MU_ClampF(flatSpeed / SPEED_ROLLBOOST, 0.0f, 1.0f);
	
	GEO_tdstColor colorBg = COLOR_GRAY;
	GEO_tdstColor colorFg = interpolateRangeOfColors(speedFactorForColor, 5, COLOR_RED, COLOR_YELLOW, COLOR_GREEN, COLOR_CYAN, COLOR_PINK);

	for (int i = 0; i <= (int)maxSpeed; i++) {
		DRAW_BARLINE(i, maxSpeed, 0, (i % 5 == 0 ? 2 : 1), COLOR_LTGRAY);
	}

	DRAW_BARLINE(5.0f, maxSpeed, -SPDMTR_BAR_HEIGHT, 1, COLOR_WHITE);
	DRAW_BARLINE(10.0f, maxSpeed, -SPDMTR_BAR_HEIGHT, 1, COLOR_WHITE);
	DRAW_BARLINE(12.55f, maxSpeed, -SPDMTR_BAR_HEIGHT, 1, COLOR_WHITE);

	UTIL_vDrawBar(SPDMTR_BAR_OFFSET_X, SPDMTR_BAR_OFFSET_Y, speedFactor * SPDMTR_BAR_MAXLENGTH, SPDMTR_BAR_HEIGHT, colorFg);
	UTIL_vDrawBar(SPDMTR_BAR_OFFSET_X, SPDMTR_BAR_OFFSET_Y, SPDMTR_BAR_MAXLENGTH, SPDMTR_BAR_HEIGHT, colorBg);
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