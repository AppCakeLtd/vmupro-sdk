
#include "framerate.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "vmupro_sdk.h"

// malloced/freed on ctor/dtor
static uint32_t *frameTimes = NULL;
static uint32_t totalFrames = 0;
static uint32_t lastFrameTimeDBG = 0.0f;
static float lastFramerateDBG = 0.0f;
static bool dbgUncappedFramerate = 0;

static const char *TAG = "Framerate";
static int64_t accumulated_us = 0;

static const uint64_t MAX_FRAME_TIME_US = 1000000 / FRAMERATE_TARGET; // microseconds per frame

static int64_t frameStartTime = 0;

void InitFrameRate()
{

    if (frameTimes != NULL)
    {
        vmupro_log(VMUPRO_LOG_ERROR, TAG, "Already inited!");
        return;
    }

    frameTimes = (uint32_t *)malloc(sizeof(uint32_t) * FRAMERATE_AVG_COUNT);
    accumulated_us = 0;
}

void DeInitFrameRate()
{
    free(frameTimes);
}

void FrameStarted()
{

    frameStartTime = vmupro_get_time_us();
}
void FrameEnded()
{

    // Measure mid-frame elapsed time
    // to determine delay

    // Measure mid-frame elapsed time
    // to determine delay

    int64_t elapsed_us = vmupro_get_time_us() - frameStartTime;

    // are we running too slow?
    if (elapsed_us > MAX_FRAME_TIME_US)
    {
        // no point carrying this forward
        // a natural delay has been introduced
        accumulated_us = 0;

        printf("under fps: elapse=%llu / targ=%llu (accum=%llu)\n", elapsed_us, MAX_FRAME_TIME_US, accumulated_us);
    }
    else
    {
        int64_t sleep_us = MAX_FRAME_TIME_US - elapsed_us;
        // add up any accumulated values that were lower than min
        sleep_us += accumulated_us;

        // if the required delay is smaller than a minimum
        // we can add it up and spread it over a few frames

        // add 100micros for jitter
        const uint32_t minSleepDelay = 350;
        if (sleep_us >= minSleepDelay)
        {
            if (!dbgUncappedFramerate)
            {
                vmupro_delay_us(sleep_us - minSleepDelay);
            }
            accumulated_us = 0;
        }
        else
        {
            accumulated_us += sleep_us;
        }
    }

    // Measure post-frame elapsed time
    // i.e. after delay

    int64_t finalTime = vmupro_get_time_us();
    UpdateFrameTime(finalTime - frameStartTime);

    // printf("FT %.1f\n", GetFPS());
}

void UpdateFrameTime(uint32_t fTimeMicros)
{
    frameTimes[totalFrames % FRAMERATE_AVG_COUNT] = fTimeMicros;
    lastFrameTimeDBG = fTimeMicros;
    totalFrames++;
}

void ResetFrameTime()
{
    totalFrames = 0;
    for (int i = 0; i < FRAMERATE_AVG_COUNT; i++)
    {
        frameTimes[i] = 0;
    }
    lastFrameTimeDBG = 0;
    lastFramerateDBG = 0;
}

inline float GetAverageFrameTimeUs()
{
    float total = 0;
    for (int i = 0; i < FRAMERATE_AVG_COUNT; i++)
    {
        total += frameTimes[i];
    }
    total /= (float)FRAMERATE_AVG_COUNT;
    return total;
}

float GetFPS()
{
    float total = GetAverageFrameTimeUs();
    total /= 1e6f;
    total = 1.0f / total;
    lastFramerateDBG = total;
    return total;
}