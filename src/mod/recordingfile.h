#pragma once
#include "recording.h"

#ifdef __cplusplus
extern "C" {
#endif

int DR_RecordingFile_Save(const char* filename, DR_InputRecording* rec);
int DR_RecordingFile_Load(const char* filename, DR_InputRecording* rec);

#ifdef __cplusplus
}
#endif
