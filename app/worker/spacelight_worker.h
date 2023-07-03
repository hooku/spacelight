#ifndef __SPACELIGHT_WORKER_H_
#define __SPACELIGHT_WORKER_H_

#include <stdint.h>

#include "spacelight.h"

typedef struct
{
    uint16_t value;
    uint16_t new_value;
    uint16_t min;
    uint16_t max;
    uint16_t step;
} TunerParam;

typedef struct
{
    TunerParam dim;
    TunerParam cct;
} CCTParam;

typedef enum
{
    INCREASE,
    DECREASE,
} GenericAction;

typedef enum
{
    CCT,
    BLINK,
    BREATHE,
    ROTATE,
    LIGHTNING,
    CCT_DRIFT,
    FIRE,
    INDEP,
    RECEIVER,
} WorkerStage;

extern void spacelight_worker_tuner(TunerParam *tuner_param, GenericAction action);

extern void spacelight_worker_cct_dim_tuner(GenericAction action);
extern void spacelight_worker_cct_cct_tuner(GenericAction action);
extern uint16_t spacelight_worker_get_dim();
extern uint16_t spacelight_worker_get_cct();
extern void spacelight_worker_set_cct_3200();
extern void spacelight_worker_set_cct_5600();

extern uint16_t spacelight_worker_get_dmxaddr();
extern void spacelight_worker_dmxaddr_tuner(GenericAction action);

extern uint16_t spacelight_worker_get_lampcount();
extern void spacelight_worker_lampcount_tuner(GenericAction action);

extern uint16_t spacelight_worker_get_locktime();
extern uint16_t spacelight_worker_get_locktime_max();
extern uint16_t spacelight_worker_get_locktime_min();
extern void spacelight_worker_locktime_tuner(GenericAction action);

extern void spacelight_worker_menu_init(GuiStage gui_stage, GuiStage last_gui_stage, uint8_t item_count);
extern void spacelight_worker_menu_tuner(GenericAction action);
extern GuiStage spacelight_worker_menu_press(GuiStage gui_stage);
extern uint16_t spacelight_worker_get_menu_cursor();

#endif // __SPACELIGHT_WORKER_H_
