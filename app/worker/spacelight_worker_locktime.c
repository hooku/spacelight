#include "spacelight_worker.h"

#define LOCKTIME_DEFAULT 60
#define LOCKTIME_MIN 10
#define LOCKTIME_MAX 300
#define LOCKTIME_STEP 1

TunerParam locktimeParam = {
    LOCKTIME_DEFAULT,
    LOCKTIME_MIN,
    LOCKTIME_MAX,
    LOCKTIME_STEP,
};

uint16_t spacelight_worker_get_locktime()
{
    return locktimeParam.value;
}

uint16_t spacelight_worker_get_locktime_max()
{
    return locktimeParam.max;
}

uint16_t spacelight_worker_get_locktime_min()
{
    return locktimeParam.min;
}

void spacelight_worker_locktime_tuner(GenericAction action)
{
    spacelight_worker_tuner(&locktimeParam, action);
}
