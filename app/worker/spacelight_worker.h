#ifndef __SPACELIGHT_WORKER_H_
#define __SPACELIGHT_WORKER_H_

#include "spacelight_param.h"

typedef enum
{
    INCREASE,
    DECREASE,
} GenericAction;

extern uint16_t sl_worker_get_stage();
extern uint16_t sl_worker_get(SlParaName name);
extern uint16_t sl_worker_get_new(SlParaName name);
extern uint16_t sl_worker_get_min(SlParaName name);
extern uint16_t sl_worker_get_max(SlParaName name);

extern void sl_worker_set_stage(uint16_t stage);
extern void sl_worker_sync(SlParaName name);
extern void sl_worker_set(SlParaName name);
extern uint16_t sl_worker_set_value(SlParaName name, uint16_t value);
extern void sl_worker_set_max(SlParaName name, uint16_t value);

extern void sl_worker_tuner(SlParaName name, GenericAction action);

extern void sl_worker_menu_init(GuiStage gui_stage, GuiStage last_gui_stage, uint8_t item_count);
extern void sl_worker_menu_tuner(GenericAction action);
extern GuiStage sl_worker_menu_press(GuiStage gui_stage);

/* locktime */
extern void sl_worker_locktime_apply();
extern void sl_worker_locktime_init();
extern void sl_worker_locktime_reset();
extern void sl_worker_locktime_unlock();
extern uint8_t sl_worker_locktime_is_locked();

#endif // __SPACELIGHT_WORKER_H_
