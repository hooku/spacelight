#include "spacelight_worker.h"

#define MENU_DEFAULT 0
#define MENU_MIN 0
#define MENU_MAX MENU_MIN
#define MENU_STEP 1

typedef enum
{
    ITEM_CCT_MODE,
    ITEM_EFFECT_MODE,
    ITEM_LAMP_COUNT,
    ITEM_DMX_ADDR,
    ITEM_DMX_MODE,
    ITEM_WIRELESS,
    ITEM_LOCK_TIME,
    ITEM_VERSION,
    ITEM_EXIT,
} MenuItemMain;

typedef enum
{
    ITEM_NONE,
    ITEM_BLINK,
    ITEM_BREATHE,
    ITEM_LIGHTNING,
    ITEM_CCT_DRIFT,
    ITEM_FIRE,
    ITEM_INDEP,
} MenuItemEffectMode;

typedef struct
{
    uint32_t menu_item;
    GuiStage gui_stage;
} ItemStageMap;

ItemStageMap itemStageMapMain[] = {
    {ITEM_CCT_MODE, MAIN_CCT},
    {ITEM_EFFECT_MODE, MENU_EFFECT_MODE},
    {ITEM_LAMP_COUNT, CFG_LAMP_COUNT},
    {ITEM_DMX_ADDR, CFG_DMX_ADDR},
    {ITEM_DMX_MODE, CFG_DMX_MODE},
    {ITEM_WIRELESS, CFG_WIRELESS},
    {ITEM_LOCK_TIME, CFG_LOCK_TIME},
    {ITEM_VERSION, CFG_VERSION},
    {ITEM_EXIT, MAIN_CCT},
};

ItemStageMap itemStageMapEffectMode[] = {
    {ITEM_NONE, MAIN_CCT},
    {ITEM_BLINK, MAIN_BLINK},
    {ITEM_BREATHE, MAIN_BREATHE},
    {ITEM_LIGHTNING, MAIN_LIGHTNING},
    {ITEM_CCT_DRIFT, MAIN_CCT_DRIFT},
    {ITEM_FIRE, MAIN_FIRE},
    {ITEM_INDEP, MAIN_INDEP},
};

TunerParam menuParam = {
    MENU_DEFAULT,
    MENU_MIN,
    MENU_MAX,
    MENU_STEP,
};

void spacelight_worker_menu_init(uint8_t item_count)
{
    menuParam.value = MENU_DEFAULT;
    menuParam.max = item_count - 1;
}

void spacelight_worker_menu_tuner(GenericAction action)
{
    spacelight_worker_tuner(&menuParam, action);
}

uint16_t spacelight_worker_get_menu_cursor()
{
    return menuParam.value;
}

GuiStage spacelight_worker_menu_press(GuiStage gui_stage)
{
    GuiStage next_gui_stage = MAIN_CCT;

    switch (gui_stage)
    {
    case MENU_EFFECT_MODE:
        next_gui_stage = itemStageMapEffectMode[menuParam.value].gui_stage;
        break;
    case CFG_DMX_MODE:
    case CFG_WIRELESS:
    case CFG_VERSION:
        next_gui_stage = MENU_MAIN;
        break;
    case MENU_MAIN:
    default:
        next_gui_stage = itemStageMapMain[menuParam.value].gui_stage;
        break;
    }
    return next_gui_stage;
}
