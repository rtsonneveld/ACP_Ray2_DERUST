#pragma once

#include "../derust.h"
#include "../framework.h"
#include "options.h"
#include "features_constants.h"
#include "features_bindings.h"

#include "feature_derustsettings.h"
#include "feature_ghostreplay.h"
#include "feature_rollboostdisplay.h"
#include "feature_speedometer.h"

// === Add new features to this list === //

#define DR_FEATURE_LIST \
    X_FEATURE(DerustSettings, "DERUST") \
    X_FEATURE(GhostReplay, "Ghost:Replay") \
    X_FEATURE(Speedometer, "Speedometer") \
    X_FEATURE(RollBoostDisplay, "Roll:Boost:Display")

// ======= End of feature list ======= //
#define CLAMP_ENUM(x, length) x = (x < 0) ? (length - 1) : ((x >= length) ? 0 : x);
#define MOD(x, y) ((x % y + y) % y)

#define DEFINE_OPTIONS(feature) \
	typedef enum DR_E_OptionID_##feature { \
		DR_FEATURE_OPTIONS_LIST_##feature \
	} DR_E_OptionID_##feature

typedef enum DR_E_MenuMode {
	DR_E_MenuMode_SelectOption,
	DR_E_MenuMode_EditOption,
	DR_E_MenuMode_LENGTH
} DR_E_MenuMode;

#define X_FEATURE(feature, name) DR_E_Feature_##feature,
typedef enum DR_E_Feature {
	DR_FEATURE_LIST
	DR_E_Feature_LENGTH
} DR_E_Feature;

#define DR_FEATURE_ForEach(i, feature) \
	DR_Feature feature = DR_Features[0]; \
	for (DR_E_Feature i = 0; i < DR_E_Feature_LENGTH; i++, feature = DR_Features[i])

#define DR_FEATURE_FUNCTIONS(feature) \
    DR_##feature##_OnActivate, \
    DR_##feature##_OnDeactivate, \
	DR_##feature##_Update, \
	DR_##feature##_Display \

typedef struct DR_Feature DR_Feature;

#define DR_FEATURE_DECLARE_FUNCTIONS(feature) \
    void DR_##feature##_OnActivate(DR_Feature*); \
    void DR_##feature##_OnDeactivate(DR_Feature*); \
    void DR_##feature##_Update(DR_Feature*); \
	void DR_##feature##_Display(DR_Feature*); \

#define X_FEATURE(feature, name) DR_FEATURE_DECLARE_FUNCTIONS(feature)
DR_FEATURE_LIST

#define DR_FEATURE_FUNC_OnActivate(featureName, feature) void DR_##featureName##_OnActivate(DR_Feature* feature)
#define DR_FEATURE_FUNC_OnDeactivate(featureName, feature) void DR_##featureName##_OnDeactivate(DR_Feature* feature)
#define DR_FEATURE_FUNC_Update(featureName, feature) void DR_##featureName##_Update(DR_Feature* feature)
#define DR_FEATURE_FUNC_Display(featureName, feature) void DR_##featureName##_Display(DR_Feature* feature)

typedef struct DR_Feature {
	DR_E_Feature id;
	const char* name;
	BOOL isActive;
	DR_FeatureOption options[MAX_OPTION_COUNT];
	void (*activateFunc)(DR_Feature*);
	void (*deactivateFunc)(DR_Feature*);
	void (*updateFunc)(DR_Feature*);
	void (*displayFunc)(DR_Feature*);
} DR_Feature;

extern DR_E_Feature DR_SelectedFeature;

void DR_Features_ManageMenuActivation();

void DR_Features_Update_SelectFeature();

void MenuDisplayFeatures(SPTXT_tdstTextInfo* p_stString);

DR_Feature* DR_Features_Get(DR_E_Feature featureType);

void DR_Features_Activate(DR_E_Feature featureType);
void DR_Features_Deactivate(DR_E_Feature featureType);

extern DR_Feature DR_Features[];