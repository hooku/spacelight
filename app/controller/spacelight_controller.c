#include "../worker/spacelight_worker.h"

#include "spacelight.h"

static GuiStage gui_stage = MAIN_CCT;

typedef struct
{
    GuiStage gui_stage;
    void (*func)(ButtonType button_type, void **gui_message, void **worker_message);
} StageControllerMap;

void spacelight_controller_cct(ButtonType button_type, void **gui_message, void **worker_message);
void spacelight_controller_blink(ButtonType button_type, void **gui_message, void **worker_message);
void spacelight_controller_dmxaddr(ButtonType button_type, void **gui_message, void **worker_message);
void spacelight_controller_lampcount(ButtonType button_type, void **gui_message, void **worker_message);
void spacelight_controller_locktime(ButtonType button_type, void **gui_message, void **worker_message);
void spacelight_controller_menu(ButtonType button_type, void **gui_message, void **worker_message);

StageControllerMap stage_controller_map[] = {
    {MAIN_CCT, spacelight_controller_cct},
    {MAIN_BLINK, spacelight_controller_cct},
    {MAIN_BREATHE, spacelight_controller_cct},
    {MAIN_ROTATE, spacelight_controller_cct},
    {MAIN_LIGHTNING, spacelight_controller_cct},
    {MAIN_CCT_DRIFT, spacelight_controller_cct},
    {MAIN_FIRE, spacelight_controller_cct},
    {MAIN_INDEP, spacelight_controller_cct},
    {MENU_MAIN, spacelight_controller_menu},
    {MENU_EFFECT_MODE, spacelight_controller_menu},
    {CFG_LAMP_COUNT, spacelight_controller_lampcount},
    {CFG_DMX_ADDR, spacelight_controller_dmxaddr},
    {CFG_DMX_MODE, spacelight_controller_menu},
    {CFG_WIRELESS, spacelight_controller_menu},
    {CFG_LOCK_TIME, spacelight_controller_locktime},
    {CFG_VERSION, spacelight_controller_menu},
};

void spacelight_controller_cct(ButtonType button_type, void **gui_message, void **worker_message)
{
    switch (button_type)
    {
    case BTN_DIM_INC:
        spacelight_worker_cct_dim_tuner(INCREASE);
        break;
    case BTN_DIM_DEC:
        spacelight_worker_cct_dim_tuner(DECREASE);
        break;
    case BTN_CCT_INC:
        spacelight_worker_cct_cct_tuner(INCREASE);
        break;
    case BTN_CCT_DEC:
        spacelight_worker_cct_cct_tuner(DECREASE);
        break;
    case BTN_3200K:
        spacelight_worker_set_cct_3200();
        break;
    case BTN_5600K:
        spacelight_worker_set_cct_5600();
        break;
    case BTN_CCT_PRESS:
    case BTN_MENU:
        gui_stage = MENU_MAIN;
        break;
    default:
        break;
    }

    *gui_message = (void *)&gui_stage;
}

void spacelight_controller_generic(ButtonType button_type, void **gui_message, void **worker_message, void (*func)(GenericAction action))
{
    switch (button_type)
    {
    case BTN_DIM_INC:
    case BTN_CCT_INC:
        func(INCREASE);
        break;
    case BTN_DIM_DEC:
    case BTN_CCT_DEC:
        func(DECREASE);
        break;
    case BTN_DIM_PRESS:
    case BTN_CCT_PRESS:
        break;
    case BTN_MENU:
    case BTN_BACK:
        gui_stage = MENU_MAIN;
        break;
    default:
        break;
    }

    *gui_message = (void *)&gui_stage;
}

void spacelight_controller_dmxaddr(ButtonType button_type, void **gui_message, void **worker_message)
{
    switch (button_type)
    {
    case BTN_DIM_PRESS:
    case BTN_CCT_PRESS:
        spacelight_worker_set_dmxaddr();
    case BTN_BACK:
        gui_stage = MENU_MAIN;
        break;
    default:
        spacelight_controller_generic(button_type, gui_message, worker_message, spacelight_worker_dmxaddr_tuner);
        break;
    }
    
    *gui_message = (void *)&gui_stage;
}

void spacelight_controller_lampcount(ButtonType button_type, void **gui_message, void **worker_message)
{
    switch (button_type)
    {
    case BTN_DIM_PRESS:
    case BTN_CCT_PRESS:
        spacelight_worker_set_lampcount();
    case BTN_BACK:
        gui_stage = MENU_MAIN;
        break;
    default:
        spacelight_controller_generic(button_type, gui_message, worker_message, spacelight_worker_lampcount_tuner);
        break;
    }
    
    *gui_message = (void *)&gui_stage;
}

void spacelight_controller_locktime(ButtonType button_type, void **gui_message, void **worker_message)
{
    switch (button_type)
    {
    case BTN_DIM_PRESS:
    case BTN_CCT_PRESS:
        spacelight_worker_set_locktime();
    case BTN_BACK:
        gui_stage = MENU_MAIN;
        break;
    default: 
        spacelight_controller_generic(button_type, gui_message, worker_message, spacelight_worker_locktime_tuner);
        break;
    }
    
    *gui_message = (void *)&gui_stage;
}

void spacelight_controller_menu(ButtonType button_type, void **gui_message, void **worker_message)
{
    switch (button_type)
    {
    case BTN_DIM_INC:
    case BTN_CCT_INC:
        spacelight_worker_menu_tuner(INCREASE);
        break;
    case BTN_DIM_DEC:
    case BTN_CCT_DEC:
        spacelight_worker_menu_tuner(DECREASE);
        break;
    case BTN_DIM_PRESS:
    case BTN_CCT_PRESS:
        gui_stage = spacelight_worker_menu_press(gui_stage);
        break;
    case BTN_MENU:
    case BTN_BACK:
        gui_stage = (gui_stage == MENU_MAIN) ? MAIN_CCT : MENU_MAIN;
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
    stage_controller_map[gui_stage].func(button_type, gui_message, worker_message);
}
