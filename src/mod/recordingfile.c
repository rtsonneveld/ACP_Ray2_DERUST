#include "recordingfile.h"

#include "recording.h"
#include <stdio.h>
#include <stdint.h>

#define DR_RECORDING_MAGIC 0x44525246  /* "DRRF, DeRust Recording File" */
#define DR_RECORDING_VERSION 1

#define CHUNK_META   0x4D455441  /* "META" */
#define CHUNK_PROG   0x50524F47  /* "PROG" */
#define CHUNK_FRAME  0x4652414D  /* "FRAM" */

static void write_u32(FILE* f, uint32_t v) { fwrite(&v, 4, 1, f); }
static void write_u8(FILE* f, uint8_t v) { fwrite(&v, 1, 1, f); }
static uint32_t read_u32(FILE* f) { uint32_t v; fread(&v, 4, 1, f); return v; }
static uint8_t read_u8(FILE* f) { uint8_t v; fread(&v, 1, 1, f); return v; }

int DR_RecordingFile_Save(const char* filename, DR_InputRecording* rec)
{
  FILE* f = fopen(filename, "wb");
  if (!f) return 0;

  /* HEADER */
  write_u32(f, DR_RECORDING_MAGIC);
  write_u32(f, DR_RECORDING_VERSION);
  write_u32(f, rec->ulNumFrames);

  /* ================= META CHUNK ================= */
  write_u32(f, CHUNK_META);

  long metaSizePos = ftell(f);
  write_u32(f, 0); /* placeholder size */

  fwrite(rec->firstLevelName, sizeof(rec->firstLevelName), 1, f);
  write_u8(f, rec->hitPoints);
  write_u8(f, rec->hitPointsMax);
  write_u8(f, rec->hitPointsMaxMax);

  long metaEnd = ftell(f);
  fseek(f, metaSizePos, SEEK_SET);
  write_u32(f, (uint32_t)(metaEnd - metaSizePos - 4));
  fseek(f, metaEnd, SEEK_SET);

  /* ================= PROGRESS CHUNK ================= */
  write_u32(f, CHUNK_PROG);

  long progSizePos = ftell(f);
  write_u32(f, 0);

  fwrite(rec->savedProgress,
    sizeof(unsigned int),
    GLOBAL_BITS_ARRAYSIZE,
    f);

  long progEnd = ftell(f);
  fseek(f, progSizePos, SEEK_SET);
  write_u32(f, (uint32_t)(progEnd - progSizePos - 4));
  fseek(f, progEnd, SEEK_SET);

  /* ================= FRAMES CHUNK ================= */
  write_u32(f, CHUNK_FRAME);

  long frameSizePos = ftell(f);
  write_u32(f, 0);

  DR_InputRecordingFrame* frame = rec->pFirstFrame;

  signed long prevStates[DR_RECORDING_MAX_ENTRIES] = { 0 };

  while (frame)
  {
    /* Write position/camera/timer raw */
    fwrite(&frame->stRaymanPosition, sizeof(frame->stRaymanPosition), 1, f);
    fwrite(&frame->stCameraPos, sizeof(frame->stCameraPos), 1, f);
    fwrite(&frame->stEngineTimer, sizeof(frame->stEngineTimer), 1, f);

    /* store only actual press/release events */

    uint8_t eventMask[(DR_RECORDING_MAX_ENTRIES + 7) / 8] = { 0 };
    int8_t  events[DR_RECORDING_MAX_ENTRIES];
    int     eventCount = 0;

    /* Detect transitions */
    for (int i = 0; i < DR_RECORDING_MAX_ENTRIES; i++)
    {
      signed long current = frame->a_stEntries[i].lState;
      signed long prev = prevStates[i];

      int8_t event = 0;

      if (prev <= 0 && current > 0)
        event = 1;      /* pressed */
      else if (prev >= 0 && current < 0)
        event = -1;     /* released */

      if (event != 0)
      {
        eventMask[i / 8] |= (1 << (i % 8));
        events[eventCount++] = event;
      }

      prevStates[i] = current;
    }

    /* Write mask */
    fwrite(eventMask, sizeof(eventMask), 1, f);

    /* Write only actual events */
    for (int i = 0; i < eventCount; i++)
      write_u8(f, (uint8_t)events[i]);


    /* analog bitmask */
    uint8_t analogMask[(DR_RECORDING_MAX_ENTRIES + 7) / 8] = { 0 };

    for (int i = 0; i < DR_RECORDING_MAX_ENTRIES; i++)
    {
      if (frame->a_stEntries[i].xAnalogicValue != 0.0f)
        analogMask[i / 8] |= (1 << (i % 8));
    }

    fwrite(analogMask, sizeof(analogMask), 1, f);

    /* write only non-zero analogs */
    for (int i = 0; i < DR_RECORDING_MAX_ENTRIES; i++)
    {
      if (analogMask[i / 8] & (1 << (i % 8)))
      {
        fwrite(&frame->a_stEntries[i].xAnalogicValue,
          sizeof(MTH_tdxReal), 1, f);
      }
    }

    frame = frame->pNextFrame;
  }

  long frameEnd = ftell(f);
  fseek(f, frameSizePos, SEEK_SET);
  write_u32(f, (uint32_t)(frameEnd - frameSizePos - 4));
  fseek(f, frameEnd, SEEK_SET);

  fclose(f);
  return 1;
}

int DR_RecordingFile_Load(const char* filename, DR_InputRecording* rec)
{
  FILE* f = fopen(filename, "rb");
  if (!f) return 0;

  uint32_t magic = read_u32(f);
  if (magic != DR_RECORDING_MAGIC)
    return 0;

  uint32_t version = read_u32(f);
  if (version != DR_RECORDING_VERSION)
    return 0; /* version check */

  rec->ulNumFrames = read_u32(f);

  signed long prevStates[DR_RECORDING_MAX_ENTRIES] = { 0 };

  long fileStart = ftell(f);
  fseek(f, 0, SEEK_END);
  long fileEnd = ftell(f);
  fseek(f, fileStart, SEEK_SET);

  while (ftell(f) < fileEnd)
  {
    uint32_t chunkId = read_u32(f);
    uint32_t chunkSize = read_u32(f);
    long chunkStart = ftell(f);

    switch (chunkId)
    {
    case CHUNK_META:
      fread(rec->firstLevelName,
        sizeof(rec->firstLevelName), 1, f);
      rec->hitPoints = read_u8(f);
      rec->hitPointsMax = read_u8(f);
      rec->hitPointsMaxMax = read_u8(f);
      break;

    case CHUNK_PROG:
      fread(rec->savedProgress,
        sizeof(unsigned int),
        GLOBAL_BITS_ARRAYSIZE,
        f);
      break;

    case CHUNK_FRAME:
    {
      DR_InputRecordingFrame* prevFrame = NULL;

      for (unsigned long frameIndex = 0;
        frameIndex < rec->ulNumFrames;
        frameIndex++)
      {
        DR_InputRecordingFrame* frame =
          calloc(1, sizeof(DR_InputRecordingFrame));

        fread(&frame->stRaymanPosition,
          sizeof(frame->stRaymanPosition), 1, f);
        fread(&frame->stCameraPos,
          sizeof(frame->stCameraPos), 1, f);
        fread(&frame->stEngineTimer,
          sizeof(frame->stEngineTimer), 1, f);

        /* restore lState from sparse press/release events */

        uint8_t eventMask[(DR_RECORDING_MAX_ENTRIES + 7) / 8];
        fread(eventMask, sizeof(eventMask), 1, f);

        for (int i = 0; i < DR_RECORDING_MAX_ENTRIES; i++)
        {
          signed long state = prevStates[i];

          if (eventMask[i / 8] & (1 << (i % 8)))
          {
            /* Read event only if bit is set */
            int8_t event = (int8_t)read_u8(f);

            if (event == 1)
              state = 1;      /* pressed */
            else
              state = -1;     /* released */
          }
          else
          {
            /* No event → continue counting */
            if (state > 0)
              state++;
            else
              state--;
          }

          frame->a_stEntries[i].lState = state;
          prevStates[i] = state;
        }

        /* analog mask */
        uint8_t analogMask[(DR_RECORDING_MAX_ENTRIES + 7) / 8];
        fread(analogMask, sizeof(analogMask), 1, f);

        for (int i = 0; i < DR_RECORDING_MAX_ENTRIES; i++)
        {
          if (analogMask[i / 8] & (1 << (i % 8)))
          {
            fread(&frame->a_stEntries[i].xAnalogicValue,
              sizeof(MTH_tdxReal), 1, f);
          }
          else
          {
            frame->a_stEntries[i].xAnalogicValue = 0.0f;
          }
        }

        /* link list */
        if (!prevFrame)
          rec->pFirstFrame = frame;
        else
          prevFrame->pNextFrame = frame;

        prevFrame = frame;
      }

      rec->pLastFrame = prevFrame;
      break;
    }

    default:
      /* skip unknown chunk */
      fseek(f, chunkStart + chunkSize, SEEK_SET);
      break;
    }
  }

  fclose(f);
  return 1;
}
