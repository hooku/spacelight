#include "spacelight.h"

#define MENU_PADDING_H 5
#define MENU_PADDING_V 1

#define MENU_ITEM_ONSCREEN 4

/* Menu element fronts*/
#define MENU_FONT_TWO_LINE_0 u8g2_font_pxplusibmvga8_tr

typedef struct
{
    char *menu;
    uint8_t item_count;
    uint8_t first_visible;
    uint8_t cursor;
} MenuParam;

char menu_main[MENU_MAIN_ITEM_COUNT][GUI_TEXT_LEN] = {
    {"CCT Mode"},
    {"Effect Mode"},
    {"Lamp Count"},
    {"DMX Addr"},
    {"DMX Mode"},
    {"Wireless"},
    {"Lock Time"},
    {"Version"},
    {"Exit"},
};

char menu_effect_mode[MENU_EFFECT_MODE_ITEM_COUNT][GUI_TEXT_LEN] = {
    {"None"},
    {STR_BLINK},
    {STR_BREATHE},
    {STR_ROTATE},
    {STR_LIGHTNING},
    {STR_CCTDRIFT},
    {STR_FIRE},
    {STR_INDEP},
};

char menu_dmx_mode[MENU_DMX_MODE_ITEM_COUNT][GUI_TEXT_LEN] = {
    {"2Ch-" STR_2CH},
    {"8Ch-" STR_8CH},
    {"11Ch-" STR_11CH},
};

char menu_wireless[MENU_WIRELESS_ITEM_COUNT][GUI_TEXT_LEN] = {
    {"On"},
    {"Off"},
};

char menu_version[MENU_VER_ITEM_COUNT][GUI_TEXT_LEN] = {
    {"COLT LED"},
};

void init_gui_menu()
{
    sprintf(menu_version[1], "HW v%d.%d", VER_HW_MAJOR, VER_HW_MINOR);
    sprintf(menu_version[2], "BT v%d.%d", VER_BT_MAJOR, VER_BT_MINOR);
    sprintf(menu_version[3], "SW v%d.%d", VER_SW_MAJOR, VER_SW_MINOR);
    sprintf(menu_version[4], "SN %s", VER_SN);
    sprintf(menu_version[5], "BU %s", __DATE__);
}

static void draw_menu(u8g2_t *u8g2, MenuParam *menu_param)
{
    u8g2_ClearBuffer(u8g2);

    u8g2_SetFontPosBaseline(u8g2);
    u8g2_SetFont(u8g2, u8g2_font_pxplusibmvga8_tr);
    uint8_t font_height = u8g2_GetAscent(u8g2) - u8g2_GetDescent(u8g2);

    uint8_t i_item;
    for (i_item = 0; i_item < MENU_ITEM_ONSCREEN; i_item++)
    {
        u8g2_uint_t flags = U8G2_BTN_BW0;
        uint8_t menu_index = menu_param->first_visible + i_item;
        if (menu_index >= menu_param->item_count)
            break;
        if (menu_index == menu_param->cursor)
            flags = U8G2_BTN_INV;

        uint16_t menu_text_bottom = MENU_PADDING_V + font_height + (font_height + MENU_PADDING_V * 2) * i_item;
        char *menu = menu_param->menu + GUI_TEXT_LEN * i_item;
        u8g2_DrawButtonUTF8(u8g2, MENU_PADDING_H, menu_text_bottom, flags, GUI_SCREEN_WIDTH, MENU_PADDING_H, MENU_PADDING_V, menu);
    }
}

void render_gui_menu(u8g2_t *u8g2, GuiStage gui_stage, GuiStage last_gui_stage)
{
    MenuParam menu_param;
    static uint32_t last_first_visible = 0;
    static uint32_t menu_main_last_first_visible = 0, menu_main_last_cursor = 0;

    switch (gui_stage)
    {
    case MENU_MAIN:
        menu_param.menu = (char *)menu_main;
        menu_param.item_count = MENU_MAIN_ITEM_COUNT;
        break;
    case MENU_EFFECT_MODE:
        menu_param.menu = (char *)menu_effect_mode;
        menu_param.item_count = MENU_EFFECT_MODE_ITEM_COUNT;
        break;
    case CFG_DMX_MODE:
        menu_param.menu = (char *)menu_dmx_mode;
        menu_param.item_count = MENU_DMX_MODE_ITEM_COUNT;
        break;
    case CFG_WIRELESS:
        menu_param.menu = (char *)menu_wireless;
        menu_param.item_count = MENU_WIRELESS_ITEM_COUNT;
        break;
    case CFG_VERSION:
        menu_param.menu = (char *)menu_version;
        menu_param.item_count = MENU_VER_ITEM_COUNT;
        break;
    default:
        break;
    }

    if (gui_stage != last_gui_stage)
    {
        /* enter sub menu */
        if (gui_stage != MENU_MAIN)
            last_first_visible = 0;
        spacelight_worker_menu_init(gui_stage, last_gui_stage, menu_param.item_count);
        if ((gui_stage == MENU_MAIN) && (!IS_MAIN_GUI(last_gui_stage)))
        {
            /* enter main menu from sub memu exit */
            last_first_visible = menu_main_last_first_visible;
            menu_param.cursor = menu_main_last_cursor;
            spacelight_worker_set_menu_cursor(menu_param.cursor);
        }
    }

    menu_param.cursor = spacelight_worker_get_menu_cursor();
    if (menu_param.cursor < last_first_visible)
    {
        last_first_visible = menu_param.cursor;
    }
    else if ((menu_param.cursor - last_first_visible) >= MENU_ITEM_ONSCREEN)
    {
        last_first_visible = menu_param.cursor - MENU_ITEM_ONSCREEN + 1;
    }

    if (gui_stage == MENU_MAIN)
    {
        /* remember main menu focus */
        menu_main_last_first_visible = last_first_visible;
        menu_main_last_cursor = spacelight_worker_get_menu_cursor();
    }

    menu_param.first_visible = last_first_visible;
    menu_param.menu += GUI_TEXT_LEN * last_first_visible;
    draw_menu(u8g2, &menu_param);
    u8g2_SendBuffer(u8g2);
}
