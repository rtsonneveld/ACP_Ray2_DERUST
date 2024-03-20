#pragma once
#include "features.h"
#include "../util.h"

// DEFINE OPTIONS HERE //
#define DR_FEATURE_OPTIONS_LIST_Speedometer \
	X_OPTION(RollingAverage, DR_E_OptionType_Boolean, "Show Rolling Average", 0) \
	X_OPTION(RollingAverageDuration, DR_E_OptionType_Integer, "Rolling Average Duration", 0)
// END DEFINE OPTIONS //

#define X_OPTION(id, type, name, value) DR_E_OptionID_Speedometer_##id,
DEFINE_OPTIONS(Speedometer);