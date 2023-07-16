#include "../worker/spacelight_worker.h"

#include "spacelight.h"
#include "spacelight_param.h"

typedef struct
{
    GuiStage gui_stage;
    void (*func)(ButtonType button_type, void **gui_message, void **worker_message);
} StageControllerMap;

typedef struct
{
    WorkerStage worker_stage;
    GuiStage gui_stage;
} WorkerGuiStageMap;

void spacelight_controller_main(ButtonType button_type, void **gui_message, void **worker_message);
void spacelight_controller_generic(ButtonType button_type, void **gui_message, void **worker_message);
void spacelight_controller_menu(ButtonType button_type, void **gui_message, void **worker_message);

StageControllerMap stage_controller_map[] = {
    {MAIN_CCT, spacelight_controller_main},
    {MAIN_BLINK, spacelight_controller_main},
    {MAIN_BREATHE, spacelight_controller_main},
    {MAIN_ROTATE, spacelight_controller_main},
    {MAIN_LIGHTNING, spacelight_controller_main},
    {MAIN_CCT_DRIFT, spacelight_controller_main},
    {MAIN_FIRE, spacelight_controller_main},
    {MAIN_INDEP, spacelight_controller_main},
    {MENU_MAIN, spacelight_controller_menu},
    {MENU_EFFECT_MODE, spacelight_controller_menu},
    {CFG_LAMP_COUNT, spacelight_controller_generic},
    {CFG_DMX_ADDR, spacelight_controller_generic},
    {CFG_DMX_MODE, spacelight_controller_menu},
    {CFG_WIRELESS, spacelight_controller_menu},
    {CFG_LOCK_TIME, spacelight_controller_generic},
    {CFG_VERSION, spacelight_controller_menu},
};

WorkerGuiStageMap work_gui_stage_map[] = {
    {WORK_CCT, MAIN_CCT},
    {WORK_BLINK, MAIN_BLINK},
    {WORK_BREATHE, MAIN_BREATHE},
    {WORK_ROTATE, MAIN_ROTATE},
    {WORK_LIGHTNING, MAIN_LIGHTNING},
    {WORK_CCT_DRIFT, MAIN_CCT_DRIFT},
    {WORK_FIRE, MAIN_FIRE},
    {WORK_INDEP, MAIN_INDEP},
};

static GuiStage gui_stage = MAIN_CCT;
static int focus_index = 1;

static SpacelightParamName get_focus(ButtonType button_type)
{
#define IS_BTN_DIM(btn) ((btn == BTN_DIM_INC) || (btn == BTN_DIM_DEC))

    int index = IS_BTN_DIM(button_type) ? 0 : focus_index;
    SpacelightParamName name = *(stage_name_map[gui_stage].name + index);
    return name;
}

static void set_focus_next()
{
    focus_index = (focus_index % stage_name_map[gui_stage].param_count) + 1;
}

void spacelight_controller_main(ButtonType button_type, void **gui_message, void **worker_message)
{
#define IS_BLINK_INDEP_CCT(name)                                          \
    ((name >= PARAM_CCT_DRIFT_CCT1) && (name <= PARAM_CCT_DRIFT_CCT1)) || \
        ((name >= PARAM_INDEP_CCT1) && (name <= PARAM_INDEP_CCT4))

    SpacelightParamName focus = get_focus(button_type);
    switch (button_type)
    {
    case BTN_DIM_INC:
    case BTN_DIM_DEC:
    case BTN_CCT_INC:
    case BTN_CCT_DEC:
        spacelight_worker_tuner(focus, GET_INC_DEC(button_type));
        spacelight_worker_set(focus);
        break;
    case BTN_3200K:
        spacelight_worker_set_value(IS_BLINK_INDEP_CCT(focus) ? focus : PARAM_CCT, CCT_3200K);
        break;
    case BTN_5600K:
        spacelight_worker_set_value(IS_BLINK_INDEP_CCT(focus) ? focus : PARAM_CCT, CCT_5600K);
        break;
    case BTN_CCT_PRESS:
        set_focus_next();
        break;
    case BTN_MENU:
        gui_stage = MENU_MAIN;
        break;
    default:
        break;
    }

    *gui_message = (void *)&gui_stage;
}

void spacelight_controller_generic(ButtonType button_type, void **gui_message, void **worker_message)
{
    SpacelightParamName focus = *stage_name_map[gui_stage].name;
    switch (button_type)
    {
    case BTN_DIM_INC:
    case BTN_CCT_INC:
    case BTN_DIM_DEC:
    case BTN_CCT_DEC:
        spacelight_worker_tuner(focus, GET_INC_DEC(button_type));
        break;
    case BTN_DIM_PRESS:
    case BTN_CCT_PRESS:
        spacelight_worker_set(focus);
    case BTN_MENU:
    case BTN_BACK:
        gui_stage = MENU_MAIN;
        break;
    default:
        break;
    }

    *gui_message = (void *)&gui_stage;
}

void spacelight_controller_menu(ButtonType button_type, void **gui_message, void **worker_message)
{
    SpacelightParamName focus = *stage_name_map[gui_stage].name;
    switch (button_type)
    {
    case BTN_DIM_INC:
    case BTN_DIM_DEC:
    case BTN_CCT_INC:
    case BTN_CCT_DEC:
        spacelight_worker_tuner(focus, GET_INC_DEC(button_type));
        spacelight_worker_set(focus);
        break;
    case BTN_DIM_PRESS:
    case BTN_CCT_PRESS:
        GuiStage next_gui_stage = spacelight_worker_menu_press(gui_stage);
        if (gui_stage == MENU_EFFECT_MODE)
        {
            spacelight_worker_set_stage(work_gui_stage_map[next_gui_stage].worker_stage);
            focus_index = 1;
        }
        spacelight_worker_init(*stage_name_map[next_gui_stage].name);
        gui_stage = next_gui_stage;
        break;
    case BTN_MENU:
    case BTN_BACK:
        gui_stage = IS_SUB_MENU_CFG(gui_stage) ? MENU_MAIN : work_gui_stage_map[spacelight_worker_get_stage()].gui_stage;
        break;
    default:
        break;
    }

    *gui_message = (void *)&gui_stage;
}

void spacelight_controller(ButtonType button_type, void **gui_message, void **worker_message)
{
    stage_controller_map[gui_stage].func(button_type, gui_message, worker_message);
}
