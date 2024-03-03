#pragma once
#include "../framework.h"

// === Add new features to this list === //

#define DR_FEATURE_LIST \
    X(None, "DERUST") \
    X(GhostReplay, "Ghost Replay") \
    X(Speedometer, "Speedometer") \
    X(RollBoostDisplay, "Roll Boost Display")

// ======= End of feature list ======= //

#define X(feature, name) DR_E_Feature_##feature,
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

#define DR_FEATURE_DECLARE_FUNCTIONS(feature) \
    void DR_##feature##_OnActivate(); \
    void DR_##feature##_OnDeactivate(); \
    void DR_##feature##_Update(); \
	void DR_##feature##_Display(); \

#define X(feature, name) DR_FEATURE_DECLARE_FUNCTIONS(feature)
DR_FEATURE_LIST

#define DR_FEATURE_FUNC_OnActivate(feature) void DR_##feature##_OnActivate()
#define DR_FEATURE_FUNC_OnDeactivate(feature) void DR_##feature##_OnDeactivate()
#define DR_FEATURE_FUNC_Update(feature) void DR_##feature##_Update()
#define DR_FEATURE_FUNC_Display(feature) void DR_##feature##_Display()

typedef struct DR_Feature {
	DR_E_Feature id;
	const char* name;
	BOOL isActive;
	void (*activateFunc)();
	void (*deactivateFunc)();
	void (*updateFunc)();
	void (*displayFunc)();
} DR_Feature;

extern DR_E_Feature DR_DisplayedFeature;