#include "spacelight_effect.h"
#include "spacelight.h"

typedef struct
{
    const MainStage stage;
    void (*func)();
} EffectFuncMap;

extern void effect_idle();

static const EffectFuncMap effect_map[] = {
    {MAIN_CCT, effect_cct},
    {MAIN_BLINK, effect_blink},
    {MAIN_BREATHE, effect_breathe},
    {MAIN_ROTATE, effect_rotate},
    {MAIN_LIGHTNING, effect_lightning},
    {MAIN_CCT_DRIFT, effect_cctdrift},
    {MAIN_FIRE, effect_fire},
    {MAIN_INDEP, effect_indep},
    {MAIN_IDLE, effect_idle},
};

MainStage effect_stage = MAIN_CCT;

void effect_idle()
{
    tx_thread_sleep(TH_IDLE_TIMEOUT);
}

void spacelight_effect(MainStage next_stage)
{
    if (next_stage != STAGE_EMPTY)
        effect_stage = next_stage;

    effect_map[effect_stage].func(effect_stage);
}
