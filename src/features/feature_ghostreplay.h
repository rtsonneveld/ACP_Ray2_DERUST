#pragma once
#include "features.h"

// DEFINE OPTIONS HERE //
#define DR_FEATURE_OPTIONS_LIST_GhostReplay \
	X_OPTION(SomeBoolean1, DR_E_OptionType_Boolean, "Never Gonna Give You Up, Never Gonna Let You Down, Never Gonna Run Around And Desert You!", 0)\
	X_OPTION(SomeBoolean2, DR_E_OptionType_Boolean, "Never Gonna Give You Up, Never Gonna Let You Down, Never Gonna Run Around And Desert You!", 0)\
	X_OPTION(SomeBoolean3, DR_E_OptionType_Boolean, "Never Gonna Give You Up, Never Gonna Let You Down, Never Gonna Run Around And Desert You!", 0)\
	X_OPTION(SomeBoolean4, DR_E_OptionType_Boolean, "Never Gonna Give You Up, Never Gonna Let You Down, Never Gonna Run Around And Desert You!", 0)\
	X_OPTION(SomeBoolean5, DR_E_OptionType_Boolean, "Never Gonna Give You Up, Never Gonna Let You Down, Never Gonna Run Around And Desert You!", 0)\
	X_OPTION(SomeBoolean6, DR_E_OptionType_Boolean, "Never Gonna Give You Up, Never Gonna Let You Down, Never Gonna Run Around And Desert You!", 0)\
	X_OPTION(SomeBoolean7, DR_E_OptionType_Boolean, "Never Gonna Give You Up, Never Gonna Let You Down, Never Gonna Run Around And Desert You!", 0)\
	X_OPTION(SomeBoolean8, DR_E_OptionType_Boolean, "Never Gonna Give You Up, Never Gonna Let You Down, Never Gonna Run Around And Desert You!", 0)\
	X_OPTION(SomeBoolean9, DR_E_OptionType_Boolean, "Never Gonna Give You Up, Never Gonna Let You Down, Never Gonna Run Around And Desert You!", 0)

// END DEFINE OPTIONS //

#define X_OPTION(id, type, name, value) DR_E_OptionID_GhostReplay_##id,
DEFINE_OPTIONS(GhostReplay);