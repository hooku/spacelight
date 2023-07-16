#ifndef __SPACELIGHT_WORKER_H_
#define __SPACELIGHT_WORKER_H_

#include "spacelight_param.h"

typedef enum
{
    INCREASE,
    DECREASE,
} GenericAction;

extern WorkerStage spacelight_worker_get_stage();
extern void spacelight_worker_set_stage(WorkerStage stage);

extern void spacelight_worker_init(SpacelightParamName name);
extern uint16_t spacelight_worker_get(SpacelightParamName name);
extern uint16_t spacelight_worker_get_new(SpacelightParamName name);
extern uint16_t spacelight_worker_get_min(SpacelightParamName name);
extern uint16_t spacelight_worker_get_max(SpacelightParamName name);

extern void spacelight_worker_set(SpacelightParamName name);
extern uint16_t spacelight_worker_set_value(SpacelightParamName name, uint16_t value);
extern void spacelight_worker_set_max(SpacelightParamName name, uint16_t value);

extern void spacelight_worker_tuner(SpacelightParamName name, GenericAction action);

extern void spacelight_worker_menu_init(GuiStage gui_stage, GuiStage last_gui_stage, uint8_t item_count);
extern void spacelight_worker_menu_tuner(GenericAction action);
extern GuiStage spacelight_worker_menu_press(GuiStage gui_stage);

#endif // __SPACELIGHT_WORKER_H_
