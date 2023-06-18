#include "spacelight_worker.h"

#define CCT_DIM_DEFAULT 100
#define CCT_DIM_MIN 0
#define CCT_DIM_MAX 100
#define CCT_DIM_STEP 1

#define CCT_CCT_DEFAULT 6500
#define CCT_CCT_MIN 2600
#define CCT_CCT_MAX 10000
#define CCT_CCT_STEP 100

CCTParam cctParam = {
    {
        CCT_DIM_DEFAULT,
        CCT_DIM_MIN,
        CCT_DIM_MAX,
        CCT_DIM_STEP,
    },
    {
        CCT_CCT_DEFAULT,
        CCT_CCT_MIN,
        CCT_CCT_MAX,
        CCT_CCT_STEP,
    }};

void spacelight_worker_cct()
{
}

void spacelight_worker_cct_dim_tuner(GenericAction action)
{
    spacelight_worker_tuner(&cctParam.dim, action);
}

void spacelight_worker_cct_cct_tuner(GenericAction action)
{
    spacelight_worker_tuner(&cctParam.cct, action);
}

uint16_t spacelight_worker_get_dim()
{
    return cctParam.dim.value;
}

uint16_t spacelight_worker_get_cct()
{
    return cctParam.cct.value;
}
