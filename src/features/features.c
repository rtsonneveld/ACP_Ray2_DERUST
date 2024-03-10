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

void CALLBACK MenuDisplay(SPTXT_tdstTextInfo* p_stString)
{
	if (!DR_MenuActive) return;
	switch (DR_MenuMode) {
		case DR_E_MenuMode_SelectFeature: MenuDisplayFeatures(p_stString); break;
		case DR_E_MenuMode_SelectOption: MenuDisplayFeatures(p_stString); break;
	}
}

void MenuDisplayFeatures(SPTXT_tdstTextInfo* p_stString)
{

	DR_FEATURE_ForEach(i, feature)
	{
		p_stString->X = 20;
		p_stString->Y = 600 + i * 30;
		p_stString->xSize = 10.0f;
		p_stString->ucAlpha = i == DR_SelectedFeature ? 255 : 127;
		p_stString->bFrame = TRUE;

		if (i != DR_SelectedFeature) {

			SPTXT_vPrintFmtLine(feature.name);
			continue;
		}
		
		SPTXT_vPrintFmtLine("%s%s%s", TXT_Seq_Red, feature.name, TXT_Seq_Reset);
	}
}

void MenuDisplayOptions(SPTXT_tdstTextInfo* p_stString)
{
	DR_Feature feature = DR_Features[DR_SelectedFeature];

	DR_FEATURE_OPTION_ForEach(feature.options, j, option)
	{
		if (option.type == DR_E_OptionType_None) {
			break;
		}

		p_stString->X = 20;
		p_stString->Y = 600 + j * 30;
		p_stString->xSize = 10.0f;
		p_stString->ucAlpha = 127;
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

void DR_Features_Init() {
	DR_SelectedFeature = DR_E_Feature_DerustSettings;
	DR_MenuActive = FALSE;
	DR_MenuMode = DR_E_MenuMode_SelectFeature;

	SPTXT_vAddTextCallback(MenuDisplay);
}

void DR_Features_Update_SelectFeature()
{
	if (IPT_M_bActionJustValidated(IPT_E_Entry_Action_Clavier_Haut)) {
		DR_SelectedFeature--;
	}
	if (IPT_M_bActionJustValidated(IPT_E_Entry_Action_Clavier_Bas)) {
		DR_SelectedFeature++;
	}

	CLAMP_ENUM(DR_SelectedFeature, DR_E_Feature_LENGTH);
}

void DR_Features_Update_SelectOption()
{
	if (IPT_M_bActionJustValidated(IPT_E_Entry_Action_Clavier_Haut)) {
		DR_SelectedOption--;
	}
	if (IPT_M_bActionJustValidated(IPT_E_Entry_Action_Clavier_Bas)) {
		DR_SelectedOption++;
	}
}

void DR_Features_Update_EditOption()
{
	// TODO
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
