#pragma once

#define PAUSE_DUR 2.5f
#define PAUSE_DUR_I (int)(PAUSE_DUR*1000)

#define NUM_SONGS 4

static const int OFFSET_ARR[] = {
    15500,8000,8500,8000
};

#define EFFECT_ARGS(x) 1, 0, 1, 1, true, false, OFFSET_ARR[x-1], OFFSET_ARR[x-1] + PAUSE_DUR_I, 0, 0, false, 0, false, true, 0, 0, 0, 0
