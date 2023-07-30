#include "../worker/spacelight_worker.h"
#include "spacelight_param.h"

typedef struct
{
    uint16_t stage;
    void (*func)(ButtonType btn_type, void **stage, void **worker_message);
} StageControllerMap;

static void sl_controller_main(ButtonType btn_type, void **stage, void **worker_message);
static void sl_controller_generic(ButtonType btn_type, void **stage, void **worker_message);
static void sl_controller_menu(ButtonType btn_type, void **stage, void **worker_message);

StageControllerMap stage_controller_map[] = {
    {MAIN_CCT, sl_controller_main},
    {MAIN_BLINK, sl_controller_main},
    {MAIN_BREATHE, sl_controller_main},
    {MAIN_ROTATE, sl_controller_main},
    {MAIN_LIGHTNING, sl_controller_main},
    {MAIN_CCT_DRIFT, sl_controller_main},
    {MAIN_FIRE, sl_controller_main},
    {MAIN_INDEP, sl_controller_main},
    {MENU_MAIN, sl_controller_menu},
    {MENU_EFFECT_MODE, sl_controller_menu},
    {CFG_LAMP_COUNT, sl_controller_generic},
    {CFG_DMX_ADDR, sl_controller_generic},
    {CFG_DMX_MODE, sl_controller_menu},
    {CFG_WIRELESS, sl_controller_menu},
    {CFG_LOCK_TIME, sl_controller_generic},
    {CFG_VERSION, sl_controller_menu},
};

static uint16_t gui_stage = GUI_MAIN;
static int focus_index = 1;

static SlParaName get_focus(ButtonType btn_type)
{
#define IS_BTN_DIM(btn) ((btn == BTN_DIM_INC) || (btn == BTN_DIM_DEC))

    int index = IS_BTN_DIM(btn_type) ? 0 : focus_index;
    SlParaName name = *(stage_name_map[gui_stage].name + index);
    return name;
}

static void set_focus_next()
{
    focus_index = (focus_index % stage_name_map[gui_stage].param_count) + 1;
}

static void sl_controller_main(ButtonType btn_type, void **stage, void **worker_message)
{
#define IS_BLINK_INDEP_CCT(name)                                          \
    ((name >= PARAM_CCT_DRIFT_CCT1) && (name <= PARAM_CCT_DRIFT_CCT1)) || \
        ((name >= PARAM_INDEP_CCT1) && (name <= PARAM_INDEP_CCT4))

    if (sl_worker_locktime_is_locked())
    {
        if (btn_type == BTN_BACK)
            sl_worker_locktime_unlock();
        else
            return;
    }
    else
    {
        SlParaName focus = get_focus(btn_type);
        switch (btn_type)
        {
        case BTN_DIM_INC:
        case BTN_DIM_DEC:
        case BTN_CCT_INC:
        case BTN_CCT_DEC:
            sl_worker_tuner(focus, GET_INC_DEC(btn_type));
            sl_worker_set(focus);
            break;
        case BTN_3200K:
            sl_worker_set_value(IS_BLINK_INDEP_CCT(focus) ? focus : PARAM_CCT, CCT_3200K);
            break;
        case BTN_5600K:
            sl_worker_set_value(IS_BLINK_INDEP_CCT(focus) ? focus : PARAM_CCT, CCT_5600K);
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

        sl_worker_locktime_reset();
    }

    *stage = (void *)&gui_stage;
}

static void sl_controller_generic(ButtonType btn_type, void **stage, void **worker_message)
{
    SlParaName focus = *stage_name_map[gui_stage].name;
    switch (btn_type)
    {
    case BTN_DIM_INC:
    case BTN_CCT_INC:
    case BTN_DIM_DEC:
    case BTN_CCT_DEC:
        sl_worker_tuner(focus, GET_INC_DEC(btn_type));
        break;
    case BTN_DIM_PRESS:
    case BTN_CCT_PRESS:
        sl_worker_set(focus);
        switch (focus)
        {
        case PARAM_LOCKTIME:
            sl_worker_locktime_apply();
            break;
        default:
            break;
        }
    case BTN_MENU:
    case BTN_BACK:
        gui_stage = MENU_MAIN;
        break;
    default:
        break;
    }

    sl_worker_locktime_reset();
    *stage = (void *)&gui_stage;
}

static void sl_controller_menu(ButtonType btn_type, void **stage, void **worker_message)
{
    SlParaName focus = *stage_name_map[gui_stage].name;
    switch (btn_type)
    {
    case BTN_DIM_INC:
    case BTN_DIM_DEC:
    case BTN_CCT_INC:
    case BTN_CCT_DEC:
        sl_worker_tuner(focus, GET_INC_DEC(btn_type));
        sl_worker_set(focus);
        break;
    case BTN_DIM_PRESS:
    case BTN_CCT_PRESS:
        GuiStage next_gui_stage = sl_worker_menu_press(gui_stage);
        if (gui_stage == MENU_EFFECT_MODE)
        {
            sl_worker_set_stage(next_gui_stage);
            focus_index = 1;
        }
        sl_worker_sync(*stage_name_map[next_gui_stage].name);
        gui_stage = next_gui_stage;
        break;
    case BTN_MENU:
    case BTN_BACK:
        gui_stage = IS_SUB_MENU_CFG(gui_stage) ? MENU_MAIN : sl_worker_get_stage();
        break;
    default:
        break;
    }

    sl_worker_locktime_reset();
    *stage = (void *)&gui_stage;
}

void sl_controller(ButtonType button_type, void **stage, void **worker_message)
{
    if (sl_worker_locktime_is_locked())
    {
        if (gui_stage >= MAIN_END)
            gui_stage = sl_worker_get_stage();
    }
    stage_controller_map[gui_stage].func(button_type, stage, worker_message);
}
