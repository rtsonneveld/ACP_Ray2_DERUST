#pragma once
#include "features.h"

void DisableAllFeatures();
void EnableAllFeatures();

// DEFINE OPTIONS HERE //
#define DR_FEATURE_OPTIONS_LIST_DerustSettings \
	X_OPTION(DisableAll, DR_E_OptionType_Action, "Disable All Features", DisableAllFeatures)\
	X_OPTION(EnableAll, DR_E_OptionType_Action, "Enable All Features", EnableAllFeatures)\
// END DEFINE OPTIONS //

#define X_OPTION(id, type, name, value) DR_E_OptionID_DerustSettings_##id,
DEFINE_OPTIONS(DerustSettings);