#include "spacelight_worker.h"

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
    ITEM_ROTATE,
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
    {ITEM_ROTATE, MAIN_ROTATE},
    {ITEM_LIGHTNING, MAIN_LIGHTNING},
    {ITEM_CCT_DRIFT, MAIN_CCT_DRIFT},
    {ITEM_FIRE, MAIN_FIRE},
    {ITEM_INDEP, MAIN_INDEP},
};

DmxMode dmx_mode = DMX_2CH;
WirelessMode wireless_mode = WIRELESS_ON;

void worker_menu_init(GuiStage gui_stage, GuiStage last_gui_stage, uint8_t item_count)
{
    worker_set_value(PARAM_MENU, MENUMAIN_DEFAULT);
    worker_set_max(PARAM_MENU, item_count - 1);
}

GuiStage worker_menu_press(GuiStage gui_stage)
{
    GuiStage next_gui_stage = MAIN_CCT;

    switch (gui_stage)
    {
    case MENU_EFFECT_MODE:
        next_gui_stage = itemStageMapEffectMode[worker_get(*stage_name_map[gui_stage].name)].gui_stage;
        break;
    case CFG_DMX_MODE:
    case CFG_WIRELESS:
        worker_set(*stage_name_map[gui_stage].name);
        next_gui_stage = MENU_MAIN;
        break;
    case CFG_VERSION:
        next_gui_stage = MENU_MAIN;
        break;
    case MENU_MAIN:
    default:
        next_gui_stage = itemStageMapMain[worker_get_new(*stage_name_map[gui_stage].name)].gui_stage;
        break;
    }
    return next_gui_stage;
}
