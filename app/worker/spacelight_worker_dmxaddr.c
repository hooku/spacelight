#include "spacelight_worker.h"

#define DMXADDR_DEFAULT 1
#define DMXADDR_MIN 1
#define DMXADDR_MAX 512
#define DMXADDR_STEP 2 

TunerParam dmxaddrParam = {
    DMXADDR_DEFAULT,
    DMXADDR_MIN,
    DMXADDR_MAX,
    DMXADDR_STEP,
};

uint16_t spacelight_worker_get_dmxaddr()
{
    return dmxaddrParam.value;
}

void spacelight_worker_dmxaddr_tuner(GenericAction action)
{
    spacelight_worker_tuner(&dmxaddrParam, action);
}
