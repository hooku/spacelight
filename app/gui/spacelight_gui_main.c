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

#define GUI_TITLE_BAR_LEFT 0
#define GUI_TITLE_BAR_TOP 0
#define GUI_TITLE_BAR_WIDTH GUI_SCREEN_WIDTH
#define GUI_TITLE_BAR_HEIGHT 8

#define GUI_STATUS_BAR_LEFT 0
#define GUI_STATUS_BAR_TOP (GUI_LCTL_BAR_TOP + GUI_LCTL_BAR_HEIGHT + GUI_LINE_WIDTH)
#define GUI_STATUS_BAR_WIDTH GUI_SCREEN_WIDTH
#define GUI_STATUS_BAR_HEIGHT 9

#define GUI_LCTL_BAR_LEFT 0
#define GUI_LCTL_BAR_TOP (GUI_TITLE_BAR_TOP + GUI_TITLE_BAR_HEIGHT + GUI_LINE_WIDTH)
#define GUI_LCTL_BAR_WIDTH 48
#define GUI_LCTL_BAR_HEIGHT (GUI_SCREEN_HEIGHT - GUI_TITLE_BAR_HEIGHT - GUI_STATUS_BAR_HEIGHT - GUI_LINE_WIDTH * 2)

#define GUI_RCTL_BAR_LEFT (GUI_LCTL_BAR_LEFT + GUI_LCTL_BAR_WIDTH + GUI_LINE_WIDTH)
#define GUI_RCTL_BAR_TOP GUI_LCTL_BAR_TOP
#define GUI_RCTL_BAR_WIDTH (GUI_SCREEN_WIDTH - GUI_LCTL_BAR_WIDTH - GUI_LINE_WIDTH)
#define GUI_RCTL_BAR_HEIGHT GUI_LCTL_BAR_HEIGHT

#define GUI_CTL_SQUEEZE_HEIGHT 6

#define GUI_TEXT_CTL_MAX_LINE 4

/* GUI element fonts */
#define GUI_TITLE_FONT u8g2_font_6x12_tr

#define GUI_LCTL_FONT_TWO_LINE_0 u8g2_font_pxplusibmvga8_tr
#define GUI_LCTL_FONT_TWO_LINE_1 u8g2_font_crox4h_tr
#define GUI_LCTL_FONT_THREE_LINE u8g2_font_5x8_tr
#define GUI_LCTL_FONT_FOUR_LINE u8g2_font_5x8_tr

#define GUI_STATUS_FONT u8g2_font_profont11_tr

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
} DrawCtlParam;

typedef struct
{
    DrawCtlParam l_ctl;
    DrawCtlParam r_ctl;
    char title_text[GUI_TEXT_LEN];
    char status_text[GUI_TEXT_LEN];
} GuiParam;

typedef struct
{
    u8g2_uint_t left;
    u8g2_uint_t top;
    u8g2_uint_t right;
    u8g2_uint_t bottom;
} Rect;

static void draw_text(u8g2_t *u8g2, Rect *pos, const char *str)
{
    u8g2_uint_t str_width = u8g2_GetStrWidth(u8g2, str);
    u8g2_uint_t center_x = pos->left + (pos->right - pos->left) / 2;

    u8g2_DrawStr(u8g2, center_x - str_width / 2, pos->bottom, str);
}

static void draw_ctl(u8g2_t *u8g2, DrawCtlParam *ctl, Rect *pos)
{
    uint16_t text_gap = (pos->bottom - pos->top) / ctl->draw_mode;

    uint8_t i_line;
    for (i_line = 0; i_line < ctl->draw_mode; i_line++)
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
            switch (i_line)
            {
            case 0:
                u8g2_SetFont(u8g2, GUI_LCTL_FONT_TWO_LINE_0);
                break;
            case 1:
            default:
                u8g2_SetFont(u8g2, GUI_LCTL_FONT_TWO_LINE_1);
                break;
            }
            break;
        }

        Rect font_pos = {
            pos->left, pos->top + text_gap * i_line,
            pos->right, pos->top + text_gap / 2 + (u8g2_GetAscent(u8g2) - u8g2_GetDescent(u8g2)) / 2 + text_gap * i_line};
        draw_text(u8g2, &font_pos, ctl->text[i_line]);
    }
}

static void draw_main(u8g2_t *u8g2, GuiParam *gui_param)
{
    u8g2_ClearBuffer(u8g2);

    u8g2_SetFontPosBaseline(u8g2);

    /* line */
    u8g2_DrawLine(u8g2, 0, GUI_TITLE_BAR_TOP + GUI_TITLE_BAR_HEIGHT + GUI_LINE_WIDTH, GUI_SCREEN_WIDTH, GUI_TITLE_BAR_TOP + GUI_TITLE_BAR_HEIGHT + GUI_LINE_WIDTH);
    u8g2_DrawLine(u8g2, GUI_LCTL_BAR_WIDTH + GUI_LINE_WIDTH, GUI_TITLE_BAR_TOP + GUI_TITLE_BAR_HEIGHT + GUI_LINE_WIDTH, GUI_LCTL_BAR_WIDTH + GUI_LINE_WIDTH, GUI_TITLE_BAR_TOP + GUI_TITLE_BAR_HEIGHT + GUI_LCTL_BAR_HEIGHT + GUI_LINE_WIDTH);
    u8g2_DrawLine(u8g2, 0, GUI_LCTL_BAR_TOP + GUI_LCTL_BAR_HEIGHT + GUI_LINE_WIDTH, GUI_SCREEN_WIDTH, GUI_LCTL_BAR_TOP + GUI_LCTL_BAR_HEIGHT + GUI_LINE_WIDTH);

    /* title bar */
    u8g2_SetFont(u8g2, GUI_TITLE_FONT);
    Rect title_bar_pos = {
        0, 0, GUI_TITLE_BAR_WIDTH, GUI_TITLE_BAR_HEIGHT};
    draw_text(u8g2, &title_bar_pos, gui_param->title_text);

    /* status bar */
    u8g2_SetFont(u8g2, GUI_STATUS_FONT);
    Rect status_bar_pos = {
        GUI_STATUS_BAR_LEFT, GUI_STATUS_BAR_TOP, GUI_STATUS_BAR_WIDTH, GUI_STATUS_BAR_TOP + GUI_STATUS_BAR_HEIGHT};
    draw_text(u8g2, &status_bar_pos, gui_param->status_text);

    /* left control */
    Rect l_control_pos = {
        GUI_LCTL_BAR_LEFT, GUI_LCTL_BAR_TOP + GUI_CTL_SQUEEZE_HEIGHT / 2,
        GUI_LCTL_BAR_LEFT + GUI_LCTL_BAR_WIDTH,
        GUI_LCTL_BAR_TOP + GUI_LCTL_BAR_HEIGHT - GUI_CTL_SQUEEZE_HEIGHT / 2};
    draw_ctl(u8g2, &gui_param->l_ctl, &l_control_pos);

    /* right control */
    Rect r_control_pos = {
        GUI_RCTL_BAR_LEFT, GUI_RCTL_BAR_TOP + GUI_CTL_SQUEEZE_HEIGHT / 2,
        GUI_RCTL_BAR_LEFT + GUI_RCTL_BAR_WIDTH,
        GUI_RCTL_BAR_TOP + GUI_RCTL_BAR_HEIGHT - GUI_CTL_SQUEEZE_HEIGHT / 2};
    draw_ctl(u8g2, &gui_param->r_ctl, &r_control_pos);
}

void setup_gui_cct(GuiParam *gui_param)
{
    gui_param->l_ctl.draw_mode = TWO_LINE;
    gui_param->r_ctl.draw_mode = TWO_LINE;
    strcpy(gui_param->l_ctl.text[0], "DIM");
    sprintf(gui_param->l_ctl.text[1], "%d%%", spacelight_worker_get_dim());
    strcpy(gui_param->r_ctl.text[0], "CCT");
    sprintf(gui_param->r_ctl.text[1], "%dK", spacelight_worker_get_cct());
    strcpy(gui_param->title_text, "CCT");
    strcpy(gui_param->status_text, "DMX:001  DIM & CCT  ");
}

void setup_gui_lightning(GuiParam *gui_param)
{
    gui_param->l_ctl.draw_mode = TWO_LINE;
    gui_param->r_ctl.draw_mode = FOUR_LINE;
    strcpy(gui_param->l_ctl.text[0], "DIM");
    sprintf(gui_param->l_ctl.text[1], "%d%%", spacelight_worker_get_dim());
    strcpy(gui_param->r_ctl.text[0], "CCT   Spd");
    strcpy(gui_param->r_ctl.text[1], "6000  1  ");
    strcpy(gui_param->r_ctl.text[2], "Duty     ");
    strcpy(gui_param->r_ctl.text[3], "46       ");
    strcpy(gui_param->title_text, "Lightning");
    strcpy(gui_param->status_text, "DMX: 001  DIM & CCT");
}

void setup_gui_indep(GuiParam *gui_param)
{
    gui_param->l_ctl.draw_mode = THREE_LINE;
    gui_param->r_ctl.draw_mode = THREE_LINE;
    strcpy(gui_param->l_ctl.text[0], "DIM    CCT");
    strcpy(gui_param->l_ctl.text[1], "#1 025 2600");
    strcpy(gui_param->l_ctl.text[2], "#2 025 2600");
    strcpy(gui_param->r_ctl.text[0], "DIM    CCT");
    strcpy(gui_param->r_ctl.text[1], "#3 025 2600");
    strcpy(gui_param->r_ctl.text[2], "#4 025 2600");
    strcpy(gui_param->title_text, "Indep");
    strcpy(gui_param->status_text, "DMX: 001  DIM & CCT");
}

void render_gui_main(u8g2_t *u8g2, GuiStage gui_stage)
{
    GuiParam gui_param;

    //    printf("hello world!");

    switch (gui_stage)
    {
    case MAIN_CCT:
        setup_gui_cct(&gui_param);
        break;
    case MAIN_BLINK:
    case MAIN_BREATHE:
    case MAIN_ROTATE:
    case MAIN_LIGHTNING:
        setup_gui_lightning(&gui_param);
        break;
    case MAIN_CCT_DRIFT:
    case MAIN_FIRE:
    case MAIN_INDEP:
        setup_gui_indep(&gui_param);
        break;
    default:
        break;
    }
    draw_main(u8g2, &gui_param);
    u8g2_SendBuffer(u8g2);
}

int _write(int file, char *ptr, int len)
{
    int DataIdx;
    for (DataIdx = 0; DataIdx < len; DataIdx++)
    {
        __io_putchar(*ptr++);
    }
    return len;
}
