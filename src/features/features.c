#include "features.h"

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

#define DISPLAY_X 20
#define DISPLAY_Y 600
#define DISPLAY_SHIFT 400

void MenuDisplayFeatures(SPTXT_tdstTextInfo* p_stString, float xOffset, float alphaMult)
{
	p_stString->X = DISPLAY_X + xOffset;
	p_stString->Y = DISPLAY_Y - 50;
	p_stString->xSize = 10.0f;
	p_stString->ucAlpha = 255 * alphaMult;
	p_stString->bFrame = TRUE;

	SPTXT_vPrintFmtLine(TXT_Seq_Red "Select a feature" TXT_Seq_Reset);

	DR_FEATURE_ForEach(i, feature)
	{
		p_stString->X = DISPLAY_X + xOffset;
		p_stString->Y = DISPLAY_Y + i * 30;
		p_stString->xSize = 10.0f;
		p_stString->ucAlpha = (i == DR_SelectedFeature ? 255 : 127) * alphaMult;
		p_stString->bFrame = TRUE;

		if (i != DR_SelectedFeature) {

			SPTXT_vPrintFmtLine(feature.name);
			continue;
		}
		
		SPTXT_vPrintFmtLine("%s%s%s", TXT_Seq_Red, feature.name, TXT_Seq_Reset);
	}
}

void MenuDisplayOptions(SPTXT_tdstTextInfo* p_stString, float xOffset, float alphaMult)
{
	DR_Feature feature = DR_Features[DR_SelectedFeature];

	p_stString->X = DISPLAY_X + xOffset;
	p_stString->Y = DISPLAY_Y - 50;
	p_stString->xSize = 10.0f;
	p_stString->ucAlpha = 255 * alphaMult;
	p_stString->bFrame = TRUE;

	SPTXT_vPrintFmtLine(TXT_Seq_Red "%s Options" TXT_Seq_Reset, feature.name);

	DR_FEATURE_OPTION_ForEach(feature.options, j, option)
	{
		if (option.type == DR_E_OptionType_None) {
			break;
		}

		p_stString->X = DISPLAY_X + xOffset;
		p_stString->Y = DISPLAY_Y + j * 30;
		p_stString->xSize = 10.0f;
		p_stString->ucAlpha = 127 * alphaMult;
		p_stString->bFrame = TRUE;
		if (DR_MenuMode == DR_E_MenuMode_SelectOption && j == DR_SelectedOption) {
			p_stString->ucAlpha = 255;
			SPTXT_vPrintFmtLine("%s%s%s", TXT_Seq_Red, option.name, TXT_Seq_Reset);
		}
		else {
			SPTXT_vPrintFmtLine("%s%s%s", TXT_Seq_Red, option.name, TXT_Seq_Reset);
		}
	}
}

float display_offset_x = 0;

float clamp(float d, float min, float max) {
	const float t = d < min ? min : d;
	return t > max ? max : t;
}

void CALLBACK MenuDisplay(SPTXT_tdstTextInfo* p_stString)
{
	float display_offset_x_target = DR_MenuMode == DR_E_MenuMode_SelectFeature ? 0 : -DISPLAY_SHIFT;
	display_offset_x += (display_offset_x_target - display_offset_x) * 0.15f;
	float display_alpha_mult_features = clamp(1.0f - (display_offset_x / -DISPLAY_SHIFT), 0, 1);
	float display_alpha_mult_options = clamp(1.0f - display_alpha_mult_features, 0, 1);

	if (!DR_MenuActive) return;

	MenuDisplayFeatures(p_stString, display_offset_x, display_alpha_mult_features);
	MenuDisplayOptions(p_stString, display_offset_x + DISPLAY_SHIFT, display_alpha_mult_options);
}

void DR_Features_Init() {
	DR_SelectedFeature = DR_E_Feature_DerustSettings;
	DR_MenuActive = FALSE;
	DR_MenuMode = DR_E_MenuMode_SelectFeature;

	SPTXT_vAddTextCallback(MenuDisplay);
}

#define MENU_CONFIRM IPT_M_bActionJustValidated(IPT_E_Entry_Action_Sauter)
#define MENU_CANCEL IPT_M_bActionJustValidated(IPT_E_Entry_Action_Tirer)

void DR_Features_Update_SelectFeature()
{
	if (IPT_M_bActionJustValidated(IPT_E_Entry_Action_Clavier_Haut)) {
		DR_SelectedFeature--;
	}
	if (IPT_M_bActionJustValidated(IPT_E_Entry_Action_Clavier_Bas)) {
		DR_SelectedFeature++;
	}

	CLAMP_ENUM(DR_SelectedFeature, DR_E_Feature_LENGTH);

	if (MENU_CONFIRM) {
		DR_MenuMode = DR_E_MenuMode_SelectOption;
	}
}

void DR_Features_Update_SelectOption()
{
	if (IPT_M_bActionJustValidated(IPT_E_Entry_Action_Clavier_Haut)) {
		DR_SelectedOption--;
	}
	if (IPT_M_bActionJustValidated(IPT_E_Entry_Action_Clavier_Bas)) {
		DR_SelectedOption++;
	}

	if (DR_SelectedOption >= MAX_OPTION_COUNT || DR_Features[DR_SelectedFeature].options[DR_SelectedOption].type == DR_E_OptionType_None) {
		DR_SelectedOption = 0;
	}
	if (DR_SelectedOption < 0) {
		DR_SelectedOption = MAX_OPTION_COUNT-1;
		while (DR_Features[DR_SelectedFeature].options[DR_SelectedOption].type == DR_E_OptionType_None && DR_SelectedOption > 0) {
			DR_SelectedOption--;
		}
	}

	if (MENU_CONFIRM) {
		DR_MenuMode = DR_E_MenuMode_EditOption;
	}

	if (MENU_CANCEL) {
		DR_MenuMode = DR_E_MenuMode_SelectFeature;
	}
}

void DR_Features_Update_EditOption()
{
	if (MENU_CANCEL) {
		DR_MenuMode = DR_E_MenuMode_SelectOption;
	}
}

void DR_Features_Update() {

	DR_FEATURE_ForEach(i, feature) {

		if (DR_Features[i].isActive) {

			if (!DR_MenuActive) {
				feature.updateFunc(DR_Features[i]);
			}
			feature.displayFunc(DR_Features[i]);
		}
	}

	DR_Features_ManageMenuActivation();

	if (!DR_MenuActive) return;

	switch (DR_MenuMode) {
		case DR_E_MenuMode_SelectFeature: DR_Features_Update_SelectFeature(); break;
		case DR_E_MenuMode_SelectOption: DR_Features_Update_SelectOption(); break;
		case DR_E_MenuMode_EditOption: DR_Features_Update_EditOption(); break;
	}
}

void DR_Features_ManageMenuActivation()
{
	if (!GAM_g_stEngineStructure->bEngineFrozen && !GAM_g_stEngineStructure->bEngineIsInPaused && !GAM_g_stEngineStructure->bEngineHasInPaused) {
		if (IPT_M_bActionJustValidated(IPT_E_Entry_Action_Affiche_Jauge))
		{
			GAM_g_stEngineStructure->bEngineFrozen = TRUE;
			GAM_g_stEngineStructure->bEngineIsInPaused = TRUE;
			GAM_g_stEngineStructure->bEngineHasInPaused = TRUE;
			DR_MenuActive = TRUE;
		}
	} else if (DR_MenuActive) {
		if (IPT_M_bActionJustValidated(IPT_E_Entry_Action_Affiche_Jauge))
		{
			GAM_g_stEngineStructure->bEngineFrozen = FALSE;
			GAM_g_stEngineStructure->bEngineIsInPaused = FALSE;
			GAM_g_stEngineStructure->bEngineHasInPaused = FALSE;
			DR_MenuActive = FALSE;
		}
	}
}
