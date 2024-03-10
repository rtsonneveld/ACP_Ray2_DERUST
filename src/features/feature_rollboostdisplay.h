#pragma once
#include "features.h"

// DEFINE OPTIONS HERE //
#define DR_FEATURE_OPTIONS_LIST_RollBoostDisplay \
	X_OPTION(SomeBoolean, DR_E_OptionType_Boolean, "some boolean", 0)
// END DEFINE OPTIONS //

#define X_OPTION(id, type, name, value) DR_E_OptionID_RollBoostDisplay_##id,
DEFINE_OPTIONS(RollBoostDisplay);