#pragma once

#include <stdint.h>

#define FRAMERATE_TARGET 60.0f

// How many frames do we average over?
// longer = smoother
// shorter = more prone to spikes
#define FRAMERATE_AVG_COUNT 30

// Init, Deinit
void InitFrameRate();
void DeInitFrameRate();

void FrameStarted();
void FrameEnded();

void UpdateFrameTime(uint32_t fTimeMicros);
void ResetFrameTime();
float GetFPS();