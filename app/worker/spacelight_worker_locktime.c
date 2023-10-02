#include "spacelight_worker.h"

TX_TIMER tmr_locktime;

#define LOCK_BANNA_MS 2000

typedef enum
{
    SM_UNLOCK,
    SM_LOCK,
} SMLocktime;

SMLocktime sm_locktime = SM_UNLOCK;

static void locktime_config(uint32_t ms)
{
    ULONG tick = MS_TO_TICK(ms);
    UINT status;
    status = tx_timer_deactivate(&tmr_locktime);
    assert_param(status == TX_SUCCESS);
    status = tx_timer_change(&tmr_locktime, tick, tick);
    assert_param(status == TX_SUCCESS);
    status = tx_timer_activate(&tmr_locktime);
    assert_param(status == TX_SUCCESS);
}

static void locktime_expired(ULONG param)
{
    if (sm_locktime == SM_UNLOCK)
    {
        sm_locktime = SM_LOCK;
        locktime_config(LOCK_BANNA_MS);
    }
    gui_refresh(worker_get_stage(), MSG_LOCK_UNLOCK_TXT);
}

void worker_locktime_apply()
{
    uint16_t locktime = SEC_TO_MS(worker_get(PARAM_LOCKTIME));
    locktime_config(locktime);
}

void worker_locktime_init()
{
    uint16_t locktime = SEC_TO_MS(worker_get(PARAM_LOCKTIME));
    ULONG locktime_tick = MS_TO_TICK(locktime);

    UINT status;
    status = tx_timer_create(&tmr_locktime, "lock",
                             locktime_expired, 0, locktime_tick, locktime_tick,
                             TX_AUTO_ACTIVATE);
    assert_param(status == TX_SUCCESS);
}

void worker_locktime_reset()
{
    worker_locktime_apply();
}

void worker_locktime_unlock()
{
    sm_locktime = SM_UNLOCK;
    worker_locktime_reset();
}

bool worker_locktime_is_locked()
{
    return (sm_locktime == SM_LOCK);
}
