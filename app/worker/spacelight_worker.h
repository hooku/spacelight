#ifndef __SPACELIGHT_WORKER_H_
#define __SPACELIGHT_WORKER_H_

#include "spacelight_param.h"
#include "spacelight.h"

typedef enum
{
    INCREASE,
    DECREASE,
} GenericAction;

extern uint16_t worker_get_stage();
extern uint16_t worker_get(SlParaName name);
extern uint16_t worker_get_new(SlParaName name);
extern uint16_t worker_get_min(SlParaName name);
extern uint16_t worker_get_max(SlParaName name);

extern void worker_set_stage(uint16_t stage);
extern void worker_sync(SlParaName name);
extern void worker_set(SlParaName name);
extern uint16_t worker_set_value(SlParaName name, uint16_t value);
extern void worker_set_max(SlParaName name, uint16_t value);

extern void worker_tuner(SlParaName name, GenericAction action);

/* menu */
extern void worker_menu_init(GuiStage gui_stage, GuiStage last_gui_stage, uint8_t item_count);
extern GuiStage worker_menu_press(GuiStage gui_stage);

/* locktime */
extern void worker_locktime_apply();
extern void worker_locktime_init();
extern void worker_locktime_reset();
extern void worker_locktime_unlock();
extern bool worker_locktime_is_locked();

/* msgbox */
extern const char *worker_msgbox_init(MsgType msg_type);

#endif // __SPACELIGHT_WORKER_H_
