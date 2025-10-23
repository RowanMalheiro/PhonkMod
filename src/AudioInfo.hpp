#pragma once

#define PAUSE_DURATION 5000

#define NUM_SONGS 4

static const int OFFSET_ARR[] = {
    15500,6000,8000,6000
};

#define EFFECT_ARGS(x) 1, 0, 1, 1, true, false, OFFSET_ARR[x-1], OFFSET_ARR[x-1] + PAUSE_DURATION, 0, 0, false, 0, false, true, 0, 0, 0, 0
