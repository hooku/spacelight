#include "spacelight_param.h"
#include "spacelight.h"

#define LOCKTIME_BOUNDBOX (GUI_SCREEN_HEIGHT / 2)

#define LOCKTIME_LEFT (GUI_SCREEN_WIDTH / 2)
#define LOCKTIME_TOP_LINE_0 (LOCKTIME_BOUNDBOX / 2)
#define LOCKTIME_TOP_LINE_1 (LOCKTIME_TOP_LINE_0 + LOCKTIME_BOUNDBOX)

#define LOCKTIME_LINE_0_FONT u8g2_font_pxplusibmvga8_tr
#define LOCKTIME_LINE_1_FONT u8g2_font_profont11_tr

#define LOCKTIME_PROGRESS_PADDING 5
#define LOCKTIME_PROGRESS_HEIGHT 10

static void draw_locktime(u8g2_t *u8g2)
{
    u8g2_SetFontPosBaseline(u8g2);

    u8g2_SetFont(u8g2, LOCKTIME_LINE_0_FONT);
    static const char locktime_title[] = {"Lock Time"};
    u8g2_uint_t title_width = u8g2_GetStrWidth(u8g2, locktime_title);
    u8g2_DrawStr(u8g2, LOCKTIME_LEFT - title_width / 2, LOCKTIME_TOP_LINE_0, locktime_title);

    u8g2_SetFont(u8g2, LOCKTIME_LINE_1_FONT);
    char locktime_text[MAX_TEXT_LEN_LONG];
    sprintf(locktime_text, "%d Sec", spacelight_worker_get_new(PARAM_LOCKTIME));
    u8g2_uint_t locktime_width = u8g2_GetStrWidth(u8g2, locktime_text);
    u8g2_DrawStr(u8g2, LOCKTIME_LEFT - locktime_width / 2, LOCKTIME_TOP_LINE_1 + LOCKTIME_BOUNDBOX / 2, locktime_text);
}

static void draw_progress(u8g2_t *u8g2)
{
    Rect pos = {
        LOCKTIME_PROGRESS_PADDING,
        LOCKTIME_TOP_LINE_1 - LOCKTIME_BOUNDBOX / 2,
        GUI_SCREEN_WIDTH - LOCKTIME_PROGRESS_PADDING,
        LOCKTIME_TOP_LINE_1 - LOCKTIME_BOUNDBOX / 2 + LOCKTIME_PROGRESS_HEIGHT};

    u8g2_DrawFrame(u8g2, pos.left, pos.top, pos.right - pos.left, pos.bottom - pos.top);

    u8g2_uint_t progress_width = (pos.right - pos.left) *
                                 (spacelight_worker_get_new(PARAM_LOCKTIME) - spacelight_worker_get_min(PARAM_LOCKTIME)) /
                                 (spacelight_worker_get_max(PARAM_LOCKTIME) - spacelight_worker_get_min(PARAM_LOCKTIME));
    u8g2_DrawBox(u8g2, pos.left, pos.top, progress_width, pos.bottom - pos.top);
}

void render_gui_locktime(u8g2_t *u8g2, GuiStage gui_stage, GuiStage last_gui_stage)
{
    u8g2_ClearBuffer(u8g2);

    draw_locktime(u8g2);
    draw_progress(u8g2);

    u8g2_SendBuffer(u8g2);
}
