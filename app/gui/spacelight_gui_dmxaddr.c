#include "spacelight_param.h"
#include "spacelight.h"

#define DMXADDR_BOUNDBOX (GUI_SCREEN_HEIGHT / 2)

#define DMXADDR_LEFT (GUI_SCREEN_WIDTH / 2)
#define DMXADDR_TOP_LINE_0 (DMXADDR_BOUNDBOX / 2)
#define DMXADDR_TOP_LINE_1 (DMXADDR_TOP_LINE_0 + DMXADDR_BOUNDBOX)

#define DMXADDR_LINE_0_FONT u8g2_font_pxplusibmvga8_tr
#define DMXADDR_LINE_1_FONT u8g2_font_crox4h_tr

void render_gui_dmxaddr(u8g2_t *u8g2, GuiStage gui_stage, GuiStage last_gui_stage)
{
    u8g2_ClearBuffer(u8g2);

    u8g2_SetFontPosBaseline(u8g2);

    u8g2_SetFont(u8g2, DMXADDR_LINE_0_FONT);
    static const char dmxaddr_title[] = {"DMX Addr"};
    u8g2_uint_t title_width = u8g2_GetStrWidth(u8g2, dmxaddr_title);
    u8g2_DrawStr(u8g2, DMXADDR_LEFT - title_width / 2, DMXADDR_TOP_LINE_0, dmxaddr_title);

    char dmxaddr_text[MAX_TEXT_LEN_LONG];
    uint16_t dmxaddr = spacelight_worker_get_new(PARAM_DMXADDR);
    sprintf(dmxaddr_text, "%d", dmxaddr);
    u8g2_SetFont(u8g2, DMXADDR_LINE_1_FONT);
    u8g2_DrawStr(u8g2, DMXADDR_LEFT, DMXADDR_TOP_LINE_1, dmxaddr_text);

    u8g2_SendBuffer(u8g2);
}
