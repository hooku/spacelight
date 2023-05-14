#include "spacelight.h"

#define GUI_MENU_PADDING_H 5
#define GUI_MENU_PADDING_V 2

#define GUI_MENU_ITEM_COUNT 9

char menu_item[GUI_MENU_ITEM_COUNT][GUI_TEXT_LEN] = {
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

void render_gui_menu(u8g2_t *u8g2, GuiStage gui_stage)
{
    while (1)
    {
        u8g2_SetFont(u8g2, u8g2_font_helvR08_tr);
        u8g2_DrawButtonUTF8(u8g2, 5, 20, U8G2_BTN_INV, GUI_SCREEN_WIDTH - 5 * 2, GUI_MENU_PADDING_H, GUI_MENU_PADDING_V, "Btn");
        u8g2_SendBuffer(u8g2);
    }
}
