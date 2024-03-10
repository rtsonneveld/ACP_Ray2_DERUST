#include "feature_speedometer.h"

DR_FEATURE_FUNC_OnActivate(Speedometer, feature) {

}

DR_FEATURE_FUNC_OnDeactivate(Speedometer, feature) {

}

DR_FEATURE_FUNC_Update(Speedometer, feature) {
}

DR_FEATURE_FUNC_Display(Speedometer, feature) {
	if (feature.options[DR_E_OptionID_Speedometer_RollingAverage].value.asBool) {

	}
}