#ifdef __cplusplus
extern "C" {
#endif

inline float MU_ClampF(float val, float minVal, float maxVal) {
	return (val < minVal ? minVal : (val > maxVal ? maxVal : val));
}

inline float MU_ClampF01(float val) {
	return MU_ClampF(val, 0.0f, 1.0f);
}

inline float MU_AbsF(float x) {
	return x < 0 ? -x : x;
}

inline float MU_LerpF(float a, float b, float f) {
	return a + (b - a) * f;
}

inline float MU_RemapRange(float value, float oldMin, float oldMax, float newMin, float newMax) {
	float factor = (value - oldMin) / (oldMax - oldMin);
	factor = MU_ClampF01(factor);
	return MU_LerpF(newMin, newMax, factor);
}

#ifdef __cplusplus
}
#endif