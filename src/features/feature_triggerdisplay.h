#pragma once
#include "features.h"
#include "derust.h"

// DEFINE OPTIONS HERE //
#define DR_FEATURE_OPTIONS_LIST_TriggerDisplay \
	X_OPTION(ShowDistanceChecks, DR_E_OptionType_Boolean, "Show distance checks", 0)\
	X_OPTION(ShowActivationZones, DR_E_OptionType_Boolean, "Show activation zones", 0)
// END DEFINE OPTIONS //

#define X_OPTION(id, type, name, value) DR_E_OptionID_TriggerDisplay_##id,
DEFINE_OPTIONS(TriggerDisplay);