#pragma once

extern const char* BITFIELD_CUSTOMBITS[32];
extern const char* BITFIELD_DYNAMICS_FLAGS[32];
extern const char* BITFIELD_DYNAMICS_ENDFLAGS[32];

void InputBitField(const char* label, unsigned long* bitfield, const char* bitLabels[]);