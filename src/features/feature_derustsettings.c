#include "feature_derustsettings.h"

DR_FEATURE_FUNC_OnActivate(DerustSettings, feature) {

}

DR_FEATURE_FUNC_OnDeactivate(DerustSettings, feature) {

}

DR_FEATURE_FUNC_Update(DerustSettings, feature) {

}

DR_FEATURE_FUNC_Display(DerustSettings, feature) {

}

void DisableAllFeatures() {
	DR_FEATURE_ForEach(i, feature) {
		DR_Features_Deactivate(feature.id);
	}
}

void EnableAllFeatures() {
	DR_FEATURE_ForEach(i, feature) {
		DR_Features_Activate(feature.id);
	}
}