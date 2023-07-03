#include "spacelight_worker.h"

#define LAMPCOUNT_DEFAULT 4
#define LAMPCOUNT_MIN 1
#define LAMPCOUNT_MAX 4
#define LAMPCOUNT_STEP 1

TunerParam lampcountParam = {
    LAMPCOUNT_DEFAULT,
    LAMPCOUNT_DEFAULT,
    LAMPCOUNT_MIN,
    LAMPCOUNT_MAX,
    LAMPCOUNT_STEP,
};

uint16_t spacelight_worker_get_lampcount()
{
    return lampcountParam.value;
}

void spacelight_worker_set_lampcount()
{
    lampcountParam.value = lampcountParam.new_value;
}

uint16_t spacelight_worker_get_lampcount_new()
{
    return lampcountParam.new_value;
}

void spacelight_worker_lampcount_tuner(GenericAction action)
{
    spacelight_worker_tuner(&lampcountParam, action);
}
