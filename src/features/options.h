#pragma once

#define MAX_OPTION_COUNT 20

typedef enum DR_E_OptionType {
	DR_E_OptionType_None,
	DR_E_OptionType_Action,
	DR_E_OptionType_Boolean,
	DR_E_OptionType_Enum,
	DR_E_OptionType_Integer,
	DR_E_OptionType_Float,
	DR_E_OptionType_LENGTH,
} DR_E_OptionType;

typedef struct DR_FeatureOption {
	DR_E_OptionType type;
	const char* name;
	union {
		int asInt;
		float asFloat;
		BOOL asBool;
		void* asAction;
	} value;

} DR_FeatureOption;

#define DR_FEATURE_OPTION_ForEach(options, i, option) \
	DR_FeatureOption option = options[0]; \
	for (int i = 0; i < DR_E_Feature_LENGTH; i++, option = options[i])