#pragma once

extern const char* BITFIELD_CUSTOMBITS[32];
extern const char* BITFIELD_DYNAMICS_FLAGS[32];
extern const char* BITFIELD_DYNAMICS_ENDFLAGS[32];
extern const char* BITFIELD_SPOFLAGS[15];
extern const char* BITFIELD_STDGAME_MISCFLAGS[8];
extern const char* BITFIELD_DR_DISPLAYOPTIONS_ZONE[4];

struct MTH3D_tdstMatrix;
struct POS_tdstCompletePosition;
struct GMT_tdstCollideMaterial;
struct GMT_tdstGameMaterial;
struct HIE_tdstSuperObject;
struct HIE_tdstEngineObject;
enum AI_tdeDsgVarType;

void InputBitField(const char* label, unsigned long* bitfield, const char* bitLabels[], int numItems);
void InputMatrix(const char* label, MTH3D_tdstMatrix* matrix);
void InputCompletePosition(const char* label, POS_tdstCompletePosition* position);

void InputCollideMaterial(GMT_tdstCollideMaterial* mat);
void InputGameMaterial(GMT_tdstGameMaterial* mat);
void InputPerso(const char* label, HIE_tdstSuperObject** p_data);
void InputPerso(const char* label, HIE_tdstEngineObject** p_data);
void DrawDsgVar(char* buffer, unsigned long offset, AI_tdeDsgVarType type);
void DrawDsgVarId(HIE_tdstSuperObject* spo, int dsgVarId);