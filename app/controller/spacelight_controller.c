#include "../worker/spacelight_worker.h"

#include "spacelight.h"

static GuiStage gui_stage = MAIN_CCT;

typedef struct
{
    GuiStage gui_stage;
    void (*func)(ButtonType button_type, void **gui_message, void **worker_message);
} ControllerMap;

void spacelight_controller_cct(ButtonType button_type, void **gui_message, void **worker_message);
void spacelight_controller_blink(ButtonType button_type, void **gui_message, void **worker_message);

ControllerMap controller_map[] = {
    {MAIN_CCT, spacelight_controller_cct},
    {MAIN_BLINK, spacelight_controller_blink},
    //{MAIN_BREATHE, spacelight_ctl_cct},
    //{MAIN_ROTATE, spacelight_ctl_cct},
    //{MAIN_LIGHTNING, spacelight_ctl_cct},
    //{MAIN_CCT_DRIFT, spacelight_ctl_cct},
    //{MAIN_FIRE, spacelight_ctl_cct},
    //{MAIN_INDEP, spacelight_ctl_cct},
    //{MENU, spacelight_ctl_cct},
    //{CFG_LAMP_COUNT, spacelight_ctl_cct},
    //{CFG_DMX_ADDR, spacelight_ctl_cct},
    //{CFG_DMX_MODE, spacelight_ctl_cct},
    //{CFG_WIRELESS, spacelight_ctl_cct},
    //{CFG_LOCK_TIME, spacelight_ctl_cct},
    //{CFG_VERSION, spacelight_ctl_cct},
};

void spacelight_controller_cct(ButtonType button_type, void **gui_message, void **worker_message)
{
    switch (button_type)
    {
    case BTN_DIM_UP:
        spacelight_worker_cct_dim_tuner(INCREASE);
        break;
    case BTN_DIM_DOWN:
        spacelight_worker_cct_dim_tuner(DECREASE);
        break;
    case BTN_CCT_UP:
        spacelight_worker_cct_cct_tuner(INCREASE);
        break;
    case BTN_CCT_DOWN:
        spacelight_worker_cct_cct_tuner(DECREASE);
        break;
    default:
        break;
    }

    *gui_message = (void *)&gui_stage;
}

void spacelight_controller_blink(ButtonType button_type, void **gui_message, void **worker_message)
{
}

void spacelight_controller(ButtonType button_type, void **gui_message, void **worker_message)
{
    controller_map[gui_stage].func(button_type, gui_message, worker_message);
}
