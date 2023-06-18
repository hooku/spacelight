#include "spacelight_worker.h"

void spacelight_worker()
{
}

void spacelight_worker_tuner(TunerParam *tuner_param, GenericAction action)
{
    uint16_t new_value = 0;
    switch (action)
    {
    case INCREASE:
        new_value = tuner_param->value + tuner_param->step;
        break;
    case DECREASE:
        new_value = tuner_param->value - tuner_param->step;
        break;
    default:
        break;
    }
    if ((new_value <= tuner_param->max) && (new_value >= tuner_param->min))
        tuner_param->value = new_value;
}
