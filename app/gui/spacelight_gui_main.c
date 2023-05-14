// draw the main gui

#include "spacelight.h"

/*
    title bar (number of head + title)
    ------------------------
                |
     left ctl   | right ctl
                |
    ------------------------
    stauts bar (address + mode)
*/

/* GUI element size, units in px */
#define GUI_LINE_WIDTH 1

#define GUI_TITLE_BAR_WIDTH GUI_SCREEN_WIDTH
#define GUI_TITLE_BAR_HEIGHT 12

#define GUI_STATUS_BAR_LEFT 0
#define GUI_STATUS_BAR_TOP (GUI_TITLE_BAR_HEIGHT + GUI_CTL_BAR_HEIGHT + GUI_LINE_WIDTH * 2)
#define GUI_STATUS_BAR_WIDTH GUI_SCREEN_WIDTH
#define GUI_STATUS_BAR_HEIGHT 12

#define GUI_CTL_BAR_TOP (GUI_TITLE_BAR_HEIGHT + GUI_LINE_WIDTH)
#define GUI_CTL_BAR_WIDTH 64
#define GUI_CTL_BAR_HEIGHT (GUI_SCREEN_HEIGHT - GUI_TITLE_BAR_HEIGHT - GUI_STATUS_BAR_HEIGHT)

#define GUI_TEXT_CTL_MAX_LINE 4

/* GUI element fonts */
#define GUI_TITLE_FONT u8g2_font_7x13B_tr

#define GUI_LCTL_FONT_TWO_LINE u8g2_font_9x18B_tr
#define GUI_LCTL_FONT_THREE_LINE u8g2_font_5x8_tr
#define GUI_LCTL_FONT_FOUR_LINE u8g2_font_5x8_tr

#define GUI_STATUS_FONT u8g2_font_6x12_tr

typedef enum
{
    TWO_LINE = 2,
    THREE_LINE = 3,
    FOUR_LINE = 4,
} CtlDrawMode;

typedef struct
{
    CtlDrawMode draw_mode;
    char text[GUI_TEXT_CTL_MAX_LINE][GUI_TEXT_LEN];
} DrawCtlParams;

typedef struct
{
    DrawCtlParams l_ctl;
    DrawCtlParams r_ctl;
    char title_text[GUI_TEXT_LEN];
    char status_text[GUI_TEXT_LEN];
} DrawParams;

void draw_text(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, const char *str)
{
    u8g2_uint_t str_width = u8g2_GetStrWidth(u8g2, str);
    u8g2_DrawStr(u8g2, x - str_width / 2, y, str);
}

void draw_ctl(u8g2_t *u8g2, DrawCtlParams *ctl, uint16_t ctl_left)
{
    switch (ctl->draw_mode)
    {
    case THREE_LINE:
        u8g2_SetFont(u8g2, GUI_LCTL_FONT_THREE_LINE);
        break;
    case FOUR_LINE:
        u8g2_SetFont(u8g2, GUI_LCTL_FONT_FOUR_LINE);
        break;
    case TWO_LINE:
    default:
        u8g2_SetFont(u8g2, GUI_LCTL_FONT_TWO_LINE);
        break;
    }

    uint16_t font_height = GUI_CTL_BAR_HEIGHT / ctl->draw_mode;
    uint16_t font_top = (GUI_CTL_BAR_TOP + font_height / 2);

    uint8_t i_line;
    for (i_line = 0; i_line < ctl->draw_mode; i_line++)
    {
        draw_text(u8g2, ctl_left + GUI_CTL_BAR_WIDTH / 2, font_top, ctl->text[i_line]);
        font_top += font_height;
    }
}

void draw_main(u8g2_t *u8g2, DrawParams *param)
{
    u8g2_ClearBuffer(u8g2);

    u8g2_SetFontPosCenter(u8g2);

    /* line */
    u8g2_DrawLine(u8g2, 0, GUI_TITLE_BAR_HEIGHT, GUI_SCREEN_WIDTH, GUI_TITLE_BAR_HEIGHT);
    u8g2_DrawLine(u8g2, GUI_CTL_BAR_WIDTH, GUI_TITLE_BAR_HEIGHT, GUI_CTL_BAR_WIDTH, GUI_TITLE_BAR_HEIGHT + GUI_CTL_BAR_HEIGHT);
    u8g2_DrawLine(u8g2, 0, GUI_TITLE_BAR_HEIGHT + GUI_CTL_BAR_HEIGHT, GUI_SCREEN_WIDTH, GUI_TITLE_BAR_HEIGHT + GUI_CTL_BAR_HEIGHT);

    /* title bar */
    u8g2_SetFont(u8g2, GUI_TITLE_FONT);
    draw_text(u8g2, GUI_TITLE_BAR_WIDTH / 2, GUI_TITLE_BAR_HEIGHT / 2, param->title_text);

    /* left control */
    draw_ctl(u8g2, &param->l_ctl, 0);

    /* right control */
    draw_ctl(u8g2, &param->r_ctl, GUI_CTL_BAR_WIDTH);

    /* status bar */
    u8g2_SetFont(u8g2, GUI_STATUS_FONT);
    draw_text(u8g2, (GUI_STATUS_BAR_LEFT + GUI_STATUS_BAR_WIDTH / 2), (GUI_STATUS_BAR_TOP + GUI_STATUS_BAR_HEIGHT / 2), param->status_text);
}

void setup_gui_cct(DrawParams *draw_param)
{
    draw_param->l_ctl.draw_mode = TWO_LINE;
    draw_param->r_ctl.draw_mode = TWO_LINE;
    strcpy(draw_param->l_ctl.text[0], "DIM");
    sprintf(draw_param->l_ctl.text[1], "%d%%", spacelight_worker_get_dim());
    strcpy(draw_param->r_ctl.text[0], "CCT");
    sprintf(draw_param->r_ctl.text[1], "%dK", spacelight_worker_get_cct());
    strcpy(draw_param->title_text, "CCT");
    strcpy(draw_param->status_text, "DMX: 001  DIM & CCT");
}

void setup_gui_lightning(DrawParams *draw_param)
{
    draw_param->l_ctl.draw_mode = TWO_LINE;
    draw_param->r_ctl.draw_mode = FOUR_LINE;
    strcpy(draw_param->l_ctl.text[0], "DIM");
    sprintf(draw_param->l_ctl.text[1], "%d%%", spacelight_worker_get_dim());
    strcpy(draw_param->r_ctl.text[0], "CCT   Spd");
    strcpy(draw_param->r_ctl.text[1], "6000  1  ");
    strcpy(draw_param->r_ctl.text[2], "Duty     ");
    strcpy(draw_param->r_ctl.text[3], "46       ");
    strcpy(draw_param->title_text, "Lightning");
    strcpy(draw_param->status_text, "DMX: 001  DIM & CCT");
}

void setup_gui_indep(DrawParams *draw_param)
{
    draw_param->l_ctl.draw_mode = THREE_LINE;
    draw_param->r_ctl.draw_mode = THREE_LINE;
    strcpy(draw_param->l_ctl.text[0], "DIM    CCT");
    strcpy(draw_param->l_ctl.text[1], "#1 025 2600");
    strcpy(draw_param->l_ctl.text[2], "#2 025 2600");
    strcpy(draw_param->r_ctl.text[0], "DIM    CCT");
    strcpy(draw_param->r_ctl.text[1], "#3 025 2600");
    strcpy(draw_param->r_ctl.text[2], "#4 025 2600");
    strcpy(draw_param->title_text, "Indep");
    strcpy(draw_param->status_text, "DMX: 001  DIM & CCT");
}

void render_gui_main(u8g2_t *u8g2, GuiStage gui_stage)
{
    DrawParams draw_param;

    switch (gui_stage)
    {
    case MAIN_CCT:
        setup_gui_cct(&draw_param);
        break;
    case MAIN_BLINK:
    case MAIN_BREATHE:
    case MAIN_ROTATE:
    case MAIN_LIGHTNING:
        setup_gui_lightning(&draw_param);
        break;
    case MAIN_CCT_DRIFT:
    case MAIN_FIRE:
    case MAIN_INDEP:
        setup_gui_indep(&draw_param);
        break;
    default:
        break;
    }
    draw_main(u8g2, &draw_param);
    u8g2_SendBuffer(u8g2);
}
