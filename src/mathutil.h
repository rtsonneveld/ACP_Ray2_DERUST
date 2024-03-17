
inline float MU_ClampF(float val, float minVal, float maxVal) {
	return (val < minVal ? minVal : (val > maxVal ? maxVal : val));
}

inline float MU_AbsF(float x) {
	return x < 0 ? -x : x;
}

inline float MU_LerpF(float a, float b, float f) {
	return a + (b - a) * f;
}
