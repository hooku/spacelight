#include "spacelight_worker.h"

#define TUNER_QUICK_SLIDE_THRESHOLD_COUNT 2
#define TUNER_QUICK_SLIDE_THRESHOLD_TIME 25

WorkerStage worker_stage = WORK_CCT;

WorkerStage spacelight_worker_get_stage()
{
    return worker_stage;
}

void spacelight_worker_set_stage(WorkerStage stage)
{
    worker_stage = stage;
}

void spacelight_worker_init(SpacelightParamName name)
{
    TunerParam *param = name_param_map[name].param;
    param->new_value = param->value;
}

uint16_t spacelight_worker_get(SpacelightParamName name)
{
    TunerParam *param = name_param_map[name].param;
    return param->value;
}

uint16_t spacelight_worker_get_new(SpacelightParamName name)
{
    TunerParam *param = name_param_map[name].param;
    return param->new_value;
}

uint16_t spacelight_worker_get_min(SpacelightParamName name)
{
    TunerParam *param = name_param_map[name].param;
    return param->min;
}

uint16_t spacelight_worker_get_max(SpacelightParamName name)
{
    TunerParam *param = name_param_map[name].param;
    return param->max;
}

void spacelight_worker_set(SpacelightParamName name)
{
    TunerParam *param = name_param_map[name].param;
    param->value = param->new_value;
}

uint16_t spacelight_worker_set_value(SpacelightParamName name, uint16_t value)
{
    TunerParam *param = name_param_map[name].param;
    param->value = value;
    param->new_value = param->value;
}

void spacelight_worker_set_max(SpacelightParamName name, uint16_t value)
{
    TunerParam *param = name_param_map[name].param;
    param->max = value;
}

void spacelight_worker_tuner(SpacelightParamName name, GenericAction action)
{
    TunerParam *param = name_param_map[name].param;

    uint16_t new_value = 0;
    uint16_t step = param->step;

    static TunerParam *last_param = NULL;
    static GenericAction last_action;
    static ULONG last_tuner_tick = 0;
    static uint8_t last_tuner_count = 0;

    ULONG current_tuner_tick = tx_time_get();

    if ((param == last_param) && (action == last_action) &&
        (current_tuner_tick - last_tuner_tick < TUNER_QUICK_SLIDE_THRESHOLD_TIME))
    {
        if (last_tuner_count >= TUNER_QUICK_SLIDE_THRESHOLD_COUNT)
            step *= (last_tuner_count + 1);
        last_tuner_count++;
    }
    if (current_tuner_tick - last_tuner_tick > TUNER_QUICK_SLIDE_THRESHOLD_TIME)
    {
        last_tuner_count = 0;
    }
    last_tuner_tick = current_tuner_tick;
    last_param = param;
    last_action = action;

    switch (action)
    {

    case DECREASE:
        if (param->new_value < step)
            param->new_value = param->min;
        else
            new_value = param->new_value - step;
        break;
    case INCREASE:
    default:
        new_value = param->new_value + step;
        break;
    }
    if ((new_value <= param->max) && (new_value >= param->min))
        param->new_value = new_value;
    else if (new_value > param->max)
        param->new_value = param->max;
    else if (new_value < param->min)
        param->new_value = param->min;
}
