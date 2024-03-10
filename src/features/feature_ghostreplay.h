#pragma once
#include "features.h"

// DEFINE OPTIONS HERE //
#define DR_FEATURE_OPTIONS_LIST_GhostReplay \
	X_OPTION(SomeBoolean, DR_E_OptionType_Boolean, "some boolean", 0)
// END DEFINE OPTIONS //

#define X_OPTION(id, type, name, value) DR_E_OptionID_GhostReplay_##id,
DEFINE_OPTIONS(GhostReplay);