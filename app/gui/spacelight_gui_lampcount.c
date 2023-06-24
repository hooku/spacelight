#include "spacelight.h"

#define LAMP_TOTAL 4
#define LAMP_BOUNDBOX (GUI_SCREEN_HEIGHT / 2)
#define LAMP_OUTER_PADDING 5
#define LAMP_INNER_PADDING 4

#define LAMPCOUNT_LEFT (GUI_SCREEN_WIDTH / 2 + GUI_SCREEN_WIDTH / 4)
#define LAMPCOUNT_TOP (GUI_SCREEN_HEIGHT / 2)

#define LAMPCOUNT_FONT u8g2_font_crox4h_tr

typedef enum
{
    LAMP_ON,
    LAMP_OFF,
} LampStatus;

/* Lamp Index:
 * 0 1
 * 2 3
 */

LampStatus lampCountStatusMapping[LAMP_TOTAL][LAMP_TOTAL] = {
    {LAMP_ON, LAMP_OFF, LAMP_OFF, LAMP_OFF},
    {LAMP_ON, LAMP_OFF, LAMP_OFF, LAMP_ON},
    {LAMP_OFF, LAMP_ON, LAMP_ON, LAMP_ON},
    {LAMP_ON, LAMP_ON, LAMP_ON, LAMP_ON},
};

static void draw_lamp(u8g2_t *u8g2, uint16_t lamp_index, LampStatus lamp_status)
{
    u8g2_uint_t x0 = LAMP_BOUNDBOX / 2;
    u8g2_uint_t y0 = LAMP_BOUNDBOX / 2;
    switch (lamp_index)
    {
    case 1:
        x0 += LAMP_BOUNDBOX;
        break;
    case 2:
        y0 += LAMP_BOUNDBOX;
        break;
    case 3:
        x0 += LAMP_BOUNDBOX;
        y0 += LAMP_BOUNDBOX;
        break;
    case 0:
    default:
        break;
    }

    u8g2_uint_t rad_outer = (LAMP_BOUNDBOX - LAMP_OUTER_PADDING * 2) / 2;
    u8g2_uint_t rad_inner = rad_outer - LAMP_INNER_PADDING;
    u8g2_DrawCircle(u8g2, x0, y0, rad_outer, U8G2_DRAW_ALL);
    if (lamp_status == LAMP_ON)
        u8g2_DrawDisc(u8g2, x0, y0, rad_inner, U8G2_DRAW_ALL);
}

static void draw_lampcount(u8g2_t *u8g2, uint16_t lampcount)
{
    u8g2_SetFontPosBaseline(u8g2);
    u8g2_SetFont(u8g2, LAMPCOUNT_FONT);

    char lampcount_text[GUI_TEXT_LEN];
    sprintf(lampcount_text, "%d", lampcount);

    u8g2_DrawStr(u8g2, LAMPCOUNT_LEFT, LAMPCOUNT_TOP, lampcount_text);
}

void render_gui_lampcount(u8g2_t *u8g2)
{
    u8g2_ClearBuffer(u8g2);

    uint16_t lampcount = spacelight_worker_get_lampcount();

    for (uint16_t i_lamp = 0; i_lamp < LAMP_TOTAL; i_lamp++)
    {
        draw_lamp(u8g2, i_lamp, lampCountStatusMapping[lampcount - 1][i_lamp]);
    }
    draw_lampcount(u8g2, lampcount);

    u8g2_SendBuffer(u8g2);
}
