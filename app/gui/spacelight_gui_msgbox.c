#include "spacelight_gui.h"
#include "../worker/spacelight_worker.h"
#include "spacelight_param.h"

/* units in px */
#define MSGBOX_ICON_WIDTH 16
#define MSGBOX_ICON_HEIGHT 16

#define MSGBOX_ICON_LEFT (GUI_SCREEN_WIDTH - MSGBOX_ICON_WIDTH) / 2
#define MSGBOX_ICON_TOP 0

#define MSGBOX_TEXT_LEFT 0
#define MSGBOX_TEXT_TOP (MSGBOX_ICON_TOP + MSGBOX_ICON_HEIGHT)
#define MSGBOX_TEXT_WIDTH GUI_SCREEN_WIDTH
#define MSGBOX_TEXT_HEIGHT 32

#define MSGBOX_FONT u8g2_font_pxplusibmvga8_tr

typedef struct
{
    MsgStyle msg_style;
    const uint8_t *icon;
} MsgIconMap;

static const uint8_t icon_exclamation[] = {
    0xc0, 0x03, 0x40, 0x02, 0x60, 0x06, 0x20, 0x04, 0xb0, 0x0d, 0x90, 0x09,
    0x98, 0x19, 0x88, 0x11, 0x8c, 0x31, 0x84, 0x21, 0x06, 0x60, 0x02, 0x40,
    0x83, 0xc1, 0x81, 0x81, 0x01, 0x80, 0xff, 0xff};

static const uint8_t icon_critical[] = {
    0xe0, 0x07, 0x18, 0x18, 0x04, 0x20, 0x02, 0x40, 0x32, 0x4c, 0x71, 0x8e,
    0xe1, 0x87, 0xc1, 0x83, 0xc1, 0x83, 0xe1, 0x87, 0x71, 0x8e, 0x32, 0x4c,
    0x02, 0x40, 0x04, 0x20, 0x18, 0x18, 0xe0, 0x07};

MsgIconMap msg_icon_map[] = {
    {MSG_EXCLAMATION, icon_exclamation},
    {MSG_CRITICAL, icon_critical},
};

void render_gui_msgbox(u8g2_t *u8g2, GuiStage gui_stage, MsgType msg_type)
{
    u8g2_ClearBuffer(u8g2);

    u8g2_DrawXBM(u8g2, MSGBOX_ICON_LEFT, MSGBOX_ICON_TOP, MSGBOX_ICON_WIDTH, MSGBOX_ICON_HEIGHT, msg_icon_map[msg_type].icon);

    char msgbox_text[MAX_TEXT_LEN_LONG];
    u8g2_SetFont(u8g2, MSGBOX_FONT);

    const char *text = worker_msgbox_init(msg_type);
    Rect font_pos = {
        MSGBOX_TEXT_LEFT, MSGBOX_TEXT_TOP,
        MSGBOX_TEXT_LEFT + MSGBOX_TEXT_WIDTH, MSGBOX_TEXT_TOP + MSGBOX_TEXT_HEIGHT};
    gui_text(u8g2, &font_pos, text);

    u8g2_SendBuffer(u8g2);
}
