#include "features.h"
#include "../mathutil.h"
#include "features_bindings.h"

// Macro for feature initialization

#define X_OPTION(id, type, name, value) {type, name, value},

#define DR_FEATURE_INIT(id, name) \
    { DR_E_Feature_##id, name, FALSE, { DR_FEATURE_OPTIONS_LIST_##id}, DR_FEATURE_FUNCTIONS(##id##) }

#define X_FEATURE(feature, name) DR_FEATURE_INIT(feature, name),

DR_Feature DR_Features[] = {
	DR_FEATURE_LIST
};

DR_E_Feature DR_SelectedFeature;
int DR_SelectedOption;
BOOL DR_MenuActive;
DR_E_MenuMode DR_MenuMode;

// Public

#define DISPLAY_FEATURES_Y 200
#define DISPLAY_OPTIONS_Y 300
#define DISPLAY_FEATURES_FONTSIZE 7
#define DISPLAY_FEATURES_FONTSIZE_SELECTED 9.9f
#define DISPLAY_FEATURES_FONTALPHA 127
#define DISPLAY_FEATURES_FONTALPHA_SELECTED 255
#define DISPLAY_FEATURES_SPACING 25
#define DISPLAY_OPTIONS_WIDTH 800
#define DISPLAY_SIZE_X 1000
#define DISPLAY_SIZE_Y 1000
#define DISPLAY_CENTER_X (DISPLAY_SIZE_X/2)
#define DISPLAY_CENTER_Y (DISPLAY_SIZE_Y/2)

float scrollX = 0.0f;

float TextWidth(char* text, float size)
{

	JFFTXT_tdstString stDummy = {
		text,
		0,
		0,
		size,
		255,
		.bWaveEffect = FALSE,
	};

	MTH2D_tdstVector stTL = { 0 };
	MTH2D_tdstVector stBR = { 0 };

	JFFTXT_vGetStringExtents(&stDummy, &stTL, &stBR);
	return (stBR.x - stTL.x) * 10;
	//return strlen(text) * SPTXT_fn_lGetCharWidth(size);
}

float featureTextAlpha[DR_E_Feature_LENGTH];

void MenuDisplayFeatures(SPTXT_tdstTextInfo* p_stString, float xOffset, float alphaMult)
{
	float x = DISPLAY_CENTER_X;

	float scrollXTarget = 0.0f;
	DR_Feature feature = DR_Features[0];

	float totalWidth = 0;
	for (DR_E_Feature i = 0; i < DR_E_Feature_LENGTH; i++, feature = DR_Features[i])
	{
		totalWidth += TextWidth(feature.name, DISPLAY_FEATURES_FONTSIZE) + DISPLAY_FEATURES_SPACING * 2;
	}
	
	feature = DR_Features[0];

	for (DR_E_Feature i = 0; i < DR_SelectedFeature; i++, feature = DR_Features[i])
	{
		float width = TextWidth(feature.name, DISPLAY_FEATURES_FONTSIZE);

		if (i < DR_E_Feature_LENGTH - 1) {
			float nextWidth = TextWidth(DR_Features[i + 1].name, DISPLAY_FEATURES_FONTSIZE);
			scrollXTarget -= width * 0.5f + DISPLAY_FEATURES_SPACING;
			scrollXTarget -= nextWidth * 0.5f + DISPLAY_FEATURES_SPACING;
		}
	}

	float scrollDiff = (scrollXTarget - scrollX);
	if (scrollDiff > totalWidth * 0.5) {
		scrollDiff -= totalWidth;
		scrollX += totalWidth;
	}
	if (scrollDiff < -totalWidth * 0.5) {
		scrollDiff += totalWidth;
		scrollX -= totalWidth;
	}

	scrollX += scrollDiff * 0.25f;
	x += scrollX;

	feature = DR_Features[0];

	for (DR_E_Feature i = 0; i < DR_E_Feature_LENGTH; i++, feature = DR_Features[i])
	{
		float width = TextWidth(feature.name, DISPLAY_FEATURES_FONTSIZE);

		float targetTextAlpha = i == DR_SelectedFeature ? 1.0f : 0.0f;
		featureTextAlpha[i] += (targetTextAlpha - featureTextAlpha[i]) * 0.25f;

		for (int j = -1; j <= 1; j++) {

			p_stString->xSize = MU_LerpF(DISPLAY_FEATURES_FONTSIZE, DISPLAY_FEATURES_FONTSIZE_SELECTED, featureTextAlpha[i]);
			float selectedWidth = TextWidth(feature.name, p_stString->xSize);
			p_stString->X = x - (selectedWidth * 0.5f) + j*totalWidth;
			p_stString->Y = DISPLAY_FEATURES_Y;

			float offCenterAlphaMult = MU_ClampF(1.0f - MU_AbsF((p_stString->X + (selectedWidth * 0.5f) - DISPLAY_CENTER_X)) / (DISPLAY_SIZE_X / 2), 0.2f, 1.0f);

			p_stString->bFrame = TRUE;
			p_stString->bSelected = DR_SelectedFeature == i && DR_SelectedOption == -1 && j == 0;

			p_stString->ucAlpha = 255 * offCenterAlphaMult;
			if (offCenterAlphaMult < 0.1f) {
				continue;
			}

			if (i != DR_E_Feature_DerustSettings) {
				SPTXT_vPrintFmtLine("%s%s"TXT_Seq_Reset, feature.isActive ? TXT_Seq_Yellow : TXT_Seq_Red, feature.name);
			} else {
				SPTXT_vPrintFmtLine(TXT_Seq_Reset"%s"TXT_Seq_Reset, feature.name);
			}
		}

		if (i < DR_E_Feature_LENGTH - 1) {
			float nextWidth = TextWidth(DR_Features[i + 1].name, DISPLAY_FEATURES_FONTSIZE);
			x += width * 0.5f + DISPLAY_FEATURES_SPACING;
			x += nextWidth * 0.5f + DISPLAY_FEATURES_SPACING;
		}
	}
}

void MenuDisplayOptions(SPTXT_tdstTextInfo* p_stString, DR_Feature feature, float xOffset, float alphaMult)
{
	DR_FEATURE_OPTION_ForEach(feature.options, j, option)
	{

		p_stString->X = DISPLAY_CENTER_X - DISPLAY_OPTIONS_WIDTH * 0.5f + xOffset;
		p_stString->Y = DISPLAY_OPTIONS_Y + j * 30;
		p_stString->xSize = 10.0f;
		p_stString->ucAlpha = 127 * alphaMult * (feature.isActive ? 1.0f : 0.25f);
		p_stString->bSelected = j == DR_SelectedOption;
		p_stString->bFrame = TRUE;
		if (DR_MenuMode == DR_E_MenuMode_SelectOption && j == DR_SelectedOption) {
			p_stString->ucAlpha = 255 * alphaMult;
			SPTXT_vPrintFmtLine(TXT_Seq_Red"%s"TXT_Seq_Reset, option.name);
		} else {
			SPTXT_vPrintFmtLine(TXT_Seq_Red"%s"TXT_Seq_Reset, option.name);
		}
	}
}

float menuOptionsScroll = 0.0f;

void CALLBACK MenuDisplay(SPTXT_tdstTextInfo* p_stString)
{
	p_stString->X = 20;
	p_stString->Y = 20;
	p_stString->xSize = 5.0f;
	p_stString->ucAlpha = 127;
	p_stString->bFrame = FALSE;

	SPTXT_vPrintLine(TXT_Seq_Reset "DERUST " DERUST_VERSION TXT_Seq_Reset);

	if (!DR_MenuActive) return;

	menuOptionsScroll += (-menuOptionsScroll) * 0.2f;

	MenuDisplayFeatures(p_stString, 0, 1);

	float alphaMult = MU_ClampF(1.0f - MU_AbsF(menuOptionsScroll) / DISPLAY_OPTIONS_WIDTH, 0, 1);

	MenuDisplayOptions(p_stString, DR_Features[MOD(DR_SelectedFeature    , DR_E_Feature_LENGTH)], menuOptionsScroll, alphaMult);
	if (menuOptionsScroll > 0.2f) {
		MenuDisplayOptions(p_stString, DR_Features[MOD(DR_SelectedFeature - 1, DR_E_Feature_LENGTH)], menuOptionsScroll - DISPLAY_OPTIONS_WIDTH, 1 - alphaMult);
	}
	if (menuOptionsScroll < -0.2f) {
		MenuDisplayOptions(p_stString, DR_Features[MOD(DR_SelectedFeature + 1, DR_E_Feature_LENGTH)], menuOptionsScroll + DISPLAY_OPTIONS_WIDTH, 1 - alphaMult);
	}
}

void DR_Features_Init() {
	DR_SelectedFeature = DR_E_Feature_DerustSettings;
	DR_SelectedOption = -1;
	DR_MenuActive = FALSE;
	DR_MenuMode = DR_E_MenuMode_SelectOption;

	DR_Features[DR_E_Feature_DerustSettings].isActive = TRUE;

	for (int i = 0; i < DR_E_Feature_LENGTH; i++) {
		featureTextAlpha[i] = 0;
	}

	SPTXT_vAddTextCallback(MenuDisplay);
}

void DR_Features_Update_SelectFeature()
{
	if (MENU_TRIG_PREV) {
		DR_SelectedFeature--;
		menuOptionsScroll -= DISPLAY_OPTIONS_WIDTH;
		DR_SelectedOption = -1;
	}
	if (MENU_TRIG_NEXT) {
		DR_SelectedFeature++;
		menuOptionsScroll += DISPLAY_OPTIONS_WIDTH;
		DR_SelectedOption = -1;
	}

	CLAMP_ENUM(DR_SelectedFeature, DR_E_Feature_LENGTH);

	if (DR_SelectedOption == -1 && MENU_TRIG_VALIDATE) {
		if (DR_SelectedFeature != DR_E_Feature_DerustSettings) {
			DR_Features[DR_SelectedFeature].isActive ^= 1;
		}
	}

	if (MENU_TRIG_RIGHT) {
		DR_MenuMode = DR_E_MenuMode_SelectOption;
	}
}

DR_Feature* DR_Features_Get(DR_E_Feature featureType) {
	if (featureType < 0 || featureType >= DR_E_Feature_LENGTH) { return NULL; }
	return &DR_Features[featureType];
}

void DR_Features_Activate(DR_E_Feature featureType) {
	DR_Feature* feature = DR_Features_Get(featureType);
	if (feature == NULL) return;

	if (!feature->isActive) {
		feature->isActive = TRUE;
		feature->activateFunc(feature);
	}
}

void DR_Features_Deactivate(DR_E_Feature featureType) {
	DR_Feature* feature = DR_Features_Get(featureType);
	if (feature == NULL) return;

	if (!feature->isActive) {
		feature->isActive = TRUE;
		feature->activateFunc(feature);
	}
}

void DR_Features_Update_SelectOption()
{
	if (DR_SelectedFeature == DR_E_Feature_DerustSettings || DR_Features[DR_SelectedFeature].isActive) {
		if (MENU_TRIG_UP) {
			DR_SelectedOption--;
		}
		if (MENU_TRIG_DOWN) {
			DR_SelectedOption++;
		}
	}

	if (DR_SelectedOption < -1) {
		DR_SelectedOption = MAX_OPTION_COUNT-1;
		while (DR_Features[DR_SelectedFeature].options[DR_SelectedOption].type == DR_E_OptionType_None && DR_SelectedOption > 0) {
			DR_SelectedOption--;
		}
	} else if (DR_SelectedOption >= MAX_OPTION_COUNT || (DR_SelectedOption>=0 && DR_Features[DR_SelectedFeature].options[DR_SelectedOption].type == DR_E_OptionType_None)) {
		DR_SelectedOption = -1;
	}
}

void DR_Features_Update_EditOption()
{
	if (MENU_TRIG_LEFT) {
		DR_MenuMode = DR_E_MenuMode_SelectOption;
	}
}

void DR_Features_Update() {

	DR_FEATURE_ForEach(i, feature) {

		if (DR_Features[i].isActive || i == DR_E_Feature_DerustSettings) {

			if (!DR_MenuActive) {
				feature.updateFunc(DR_Features[i]);
			}
			feature.displayFunc(DR_Features[i]);
		}
	}

	DR_Features_ManageMenuActivation();

	if (!DR_MenuActive) return;
	
	DR_Features_Update_SelectFeature();

	switch (DR_MenuMode) {
		case DR_E_MenuMode_SelectOption: DR_Features_Update_SelectOption(); break;
		case DR_E_MenuMode_EditOption: DR_Features_Update_EditOption(); break;
	}
}

void DR_Features_ManageMenuActivation()
{
	if (!GAM_g_stEngineStructure->bEngineFrozen && !GAM_g_stEngineStructure->bEngineIsInPaused && !GAM_g_stEngineStructure->bEngineHasInPaused) {
		if (MENU_TRIG_ACTIVATE)
		{
			GAM_g_stEngineStructure->bEngineFrozen = TRUE;
			GAM_g_stEngineStructure->bEngineIsInPaused = TRUE;
			GAM_g_stEngineStructure->bEngineHasInPaused = TRUE;

			for (int i = 0; i < DR_E_Feature_LENGTH; i++) {
				featureTextAlpha[i] = 0;
			}

			DR_MenuActive = TRUE;
		}
	} else if (DR_MenuActive) {
		if (MENU_TRIG_DEACTIVATE)
		{
			GAM_g_stEngineStructure->bEngineFrozen = FALSE;
			GAM_g_stEngineStructure->bEngineIsInPaused = FALSE;
			GAM_g_stEngineStructure->bEngineHasInPaused = FALSE;
			DR_MenuActive = FALSE;
		}
	}
}
