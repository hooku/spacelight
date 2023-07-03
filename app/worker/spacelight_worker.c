#include "spacelight_worker.h"

#define TUNER_QUICK_SLIDE_THRESHOLD_COUNT 3
#define TUNER_QUICK_SLIDE_THRESHOLD_TIME 15

void spacelight_worker()
{
}

void spacelight_worker_tuner(TunerParam *tuner_param, GenericAction action)
{
    uint16_t new_value = 0;
    uint16_t step = tuner_param->step;

    static TunerParam *last_tuner_param = NULL;
    static GenericAction last_action;
    static ULONG last_tuner_tick = 0;
    static uint8_t last_tuner_count = 0;

    ULONG current_tuner_tick = tx_time_get();

    if ((tuner_param == last_tuner_param) && (action == last_action) &&
        (current_tuner_tick - last_tuner_tick < TUNER_QUICK_SLIDE_THRESHOLD_TIME))
    {
        if (last_tuner_count >= TUNER_QUICK_SLIDE_THRESHOLD_COUNT)
            step *= last_tuner_count;
        last_tuner_count++;
    }
    if (current_tuner_tick - last_tuner_tick > TUNER_QUICK_SLIDE_THRESHOLD_TIME)
    {
        last_tuner_count = 0;
    }
    last_tuner_tick = current_tuner_tick;
    last_tuner_param = tuner_param;
    last_action = action;

    switch (action)
    {

    case DECREASE:
        if (tuner_param->new_value < step)
            tuner_param->new_value = tuner_param->min;
        else
            new_value = tuner_param->new_value - step;
        break;
    case INCREASE:
    default:
        new_value = tuner_param->new_value + step;
        break;
    }
    if ((new_value <= tuner_param->max) && (new_value >= tuner_param->min))
        tuner_param->new_value = new_value;
    else if (new_value > tuner_param->max)
        tuner_param->new_value = tuner_param->max;
    else if (new_value < tuner_param->min)
        tuner_param->new_value = tuner_param->min;
}
