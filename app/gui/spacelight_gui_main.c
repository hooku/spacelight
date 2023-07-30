#include "../worker/spacelight_worker.h"
#include "spacelight_gui.h"
#include "spacelight_param.h"

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
#define GUI_TITLE_BAR_HEIGHT 9

#define GUI_STATUS_BAR_LEFT 0
#define GUI_STATUS_BAR_TOP (GUI_LCTL_BAR_TOP + GUI_LCTL_BAR_HEIGHT + GUI_LINE_WIDTH)
#define GUI_STATUS_BAR_WIDTH GUI_SCREEN_WIDTH
#define GUI_STATUS_BAR_HEIGHT 11

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
#define GUI_TITLE_FONT u8g2_font_6x10_tr

#define GUI_LCTL_FONT_TWO_LINE_0 u8g2_font_pxplusibmvga8_tr
#define GUI_LCTL_FONT_TWO_LINE_1 u8g2_font_crox4h_tr
#define GUI_LCTL_FONT_THREE_LINE u8g2_font_profont11_tr
#define GUI_LCTL_FONT_FOUR_LINE u8g2_font_profont11_tr

#define GUI_STATUS_FONT u8g2_font_profont11_tr

typedef enum
{
    TWO_LINE = 2,
    THREE_LINE = 3,
    FOUR_LINE = 4,
} CtlDrawMode;

typedef struct
{
    uint16_t stage;
    char title_text[MAX_TEXT_LEN_LONG];
} StageTitleMap;

typedef struct
{
    DmxMode dmx_mode;
    char status_text[MAX_TEXT_LEN_LONG];
} DmxStatusMap;

typedef struct
{
    CtlDrawMode draw_mode;
    char text[GUI_TEXT_CTL_MAX_LINE][MAX_TEXT_LEN_LONG];
} DrawCtlParam;

typedef struct
{
    DrawCtlParam l_ctl;
    DrawCtlParam r_ctl;
    char title_text[MAX_TEXT_LEN_LONG];
    char status_text[MAX_TEXT_LEN_LONG];
} GuiParam;

StageTitleMap stage_title_map[] = {
    {MAIN_CCT, STR_CCT},
    {MAIN_BLINK, STR_BLINK},
    {MAIN_BREATHE, STR_BREATHE},
    {MAIN_ROTATE, STR_ROTATE},
    {MAIN_LIGHTNING, STR_LIGHTNING},
    {MAIN_CCT_DRIFT, STR_CCTDRIFT},
    {MAIN_FIRE, STR_FIRE},
    {MAIN_INDEP, STR_INDEP},
};

DmxStatusMap dmx_mode_map[] = {
    {DMX_2CH, STR_2CH},
    {DMX_8CH, STR_8CH},
    {DMX_11CH, STR_11CH},
};

static int16_t get_highlight_text(const char *str, char *highlight)
{
    int16_t highlight_start = -1;
    bool highlight_end = false;
    for (int i_ch = 0; (i_ch < MAX_TEXT_LEN_LONG) && (str[i_ch] != 0); i_ch++)
    {
        if (str[i_ch] == '\2') /* start of text */
            highlight_start = i_ch;
        if (highlight_start >= 0)
        {
            if (str[i_ch] == '\3') /* end of text */
                highlight_end = true;

            if (!highlight_end)
                highlight[i_ch - highlight_start] = str[i_ch];
        }
    }

    return highlight_start;
}

/* Draw text on screen
 *
 * @param u8g2 u8g2 handle ptr
 * @param pos Text region rectangle
 * @param str Text to draw, highlight
 */
static void draw_text(u8g2_t *u8g2, Rect *pos, char *str)
{
    u8g2_uint_t str_width = u8g2_GetStrWidth(u8g2, str);
    u8g2_uint_t center_x = pos->left + (pos->right - pos->left) / 2;
    u8g2_DrawStr(u8g2, center_x - str_width / 2, pos->bottom, str);

    /* draw high light text over normal text */
    char highlight_text[MAX_TEXT_LEN_LONG] = {0};
    int16_t highlight_start = get_highlight_text(str, highlight_text);
    if (highlight_start >= 0)
    {
        u8g2_SetDrawColor(u8g2, 0);
        str[highlight_start] = 0;
        u8g2_uint_t hightlight_left_width = u8g2_GetStrWidth(u8g2, str);
        u8g2_DrawStr(u8g2, center_x - str_width / 2 + hightlight_left_width, pos->bottom, highlight_text);
        u8g2_SetDrawColor(u8g2, 1);
    }
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
            pos->right, pos->top + text_gap * i_line + text_gap / 2 + (u8g2_GetAscent(u8g2) - u8g2_GetDescent(u8g2)) / 2};
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
        0, 0, GUI_TITLE_BAR_WIDTH, GUI_TITLE_BAR_HEIGHT + u8g2_GetDescent(u8g2)};
    draw_text(u8g2, &title_bar_pos, gui_param->title_text);

    /* status bar */
    u8g2_SetFont(u8g2, GUI_STATUS_FONT);
    Rect status_bar_pos = {
        GUI_STATUS_BAR_LEFT, GUI_STATUS_BAR_TOP, GUI_STATUS_BAR_WIDTH, GUI_STATUS_BAR_TOP + GUI_STATUS_BAR_HEIGHT + u8g2_GetDescent(u8g2)};
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

static void setup_gui_cct(GuiParam *gui_param)
{
    gui_param->l_ctl.draw_mode = TWO_LINE;
    gui_param->r_ctl.draw_mode = TWO_LINE;
    strcpy(gui_param->l_ctl.text[0], "DIM");
    sprintf(gui_param->l_ctl.text[1], "%d%%", sl_worker_get(PARAM_DIM));
    strcpy(gui_param->r_ctl.text[0], "CCT");
    sprintf(gui_param->r_ctl.text[1], "%dK", sl_worker_get(PARAM_CCT));
}

static void setup_gui_blink(GuiParam *gui_param)
{
    gui_param->l_ctl.draw_mode = TWO_LINE;
    gui_param->r_ctl.draw_mode = FOUR_LINE;
    strcpy(gui_param->l_ctl.text[0], "DIM");
    sprintf(gui_param->l_ctl.text[1], "%d%%", sl_worker_get(PARAM_DIM));
    strcpy(gui_param->r_ctl.text[0], "CCT   Spd");
    strcpy(gui_param->r_ctl.text[1], "6000  1  ");
    strcpy(gui_param->r_ctl.text[2], "Duty     ");
    strcpy(gui_param->r_ctl.text[3], "46       ");
}

static void setup_gui_breathe(GuiParam *gui_param)
{
    gui_param->l_ctl.draw_mode = TWO_LINE;
    gui_param->r_ctl.draw_mode = FOUR_LINE;
    strcpy(gui_param->l_ctl.text[0], "DIM");
    sprintf(gui_param->l_ctl.text[1], "%d%%", sl_worker_get(PARAM_DIM));
    strcpy(gui_param->r_ctl.text[0], "CCT   Spd");
    strcpy(gui_param->r_ctl.text[1], "6000  1  ");
}

static void setup_gui_rotate(GuiParam *gui_param)
{
    gui_param->l_ctl.draw_mode = TWO_LINE;
    gui_param->r_ctl.draw_mode = FOUR_LINE;
    strcpy(gui_param->l_ctl.text[0], "DIM");
    sprintf(gui_param->l_ctl.text[1], "%d%%", sl_worker_get(PARAM_DIM));
    strcpy(gui_param->r_ctl.text[0], "CCT   Spd");
    strcpy(gui_param->r_ctl.text[1], "6000  1  ");
    strcpy(gui_param->r_ctl.text[2], "Duty     ");
    strcpy(gui_param->r_ctl.text[3], "46       ");
}

static void setup_gui_lightning(GuiParam *gui_param)
{
    gui_param->l_ctl.draw_mode = TWO_LINE;
    gui_param->r_ctl.draw_mode = FOUR_LINE;
    strcpy(gui_param->l_ctl.text[0], "DIM");
    sprintf(gui_param->l_ctl.text[1], "%d%%", sl_worker_get(PARAM_DIM));
    strcpy(gui_param->r_ctl.text[0], "CCT   Spd");
    strcpy(gui_param->r_ctl.text[1], "6000  1  ");
    strcpy(gui_param->r_ctl.text[2], "Duty     ");
    strcpy(gui_param->r_ctl.text[3], "46       ");
}

static void setup_gui_cctdrift(GuiParam *gui_param)
{
    gui_param->l_ctl.draw_mode = TWO_LINE;
    gui_param->r_ctl.draw_mode = FOUR_LINE;
    strcpy(gui_param->l_ctl.text[0], "DIM");
    sprintf(gui_param->l_ctl.text[1], "%d%%", sl_worker_get(PARAM_DIM));
    strcpy(gui_param->r_ctl.text[0], "CCT   Spd");
    strcpy(gui_param->r_ctl.text[1], "6000  1  ");
    strcpy(gui_param->r_ctl.text[2], "Duty     ");
    strcpy(gui_param->r_ctl.text[3], "46       ");
}

static void setup_gui_fire(GuiParam *gui_param)
{
    gui_param->l_ctl.draw_mode = TWO_LINE;
    gui_param->r_ctl.draw_mode = FOUR_LINE;
    strcpy(gui_param->l_ctl.text[0], "DIM");
    sprintf(gui_param->l_ctl.text[1], "%d%%", sl_worker_get(PARAM_DIM));
    strcpy(gui_param->r_ctl.text[0], "CCT   Spd");
    strcpy(gui_param->r_ctl.text[1], "6000  1  ");
    strcpy(gui_param->r_ctl.text[2], "Duty     ");
    strcpy(gui_param->r_ctl.text[3], "46       ");
}

static void setup_gui_indep(GuiParam *gui_param)
{
    gui_param->l_ctl.draw_mode = THREE_LINE;
    gui_param->r_ctl.draw_mode = THREE_LINE;
    strcpy(gui_param->l_ctl.text[0], "DIM    CCT");
    strcpy(gui_param->l_ctl.text[1], "#1 025 2600");
    strcpy(gui_param->l_ctl.text[2], "#2 025 2600");
    strcpy(gui_param->r_ctl.text[0], "DIM    CCT");
    strcpy(gui_param->r_ctl.text[1], "#3 025 2600");
    strcpy(gui_param->r_ctl.text[2], "#4 025 2600");
}

static void setup_gui_status(GuiParam *gui_param, uint8_t update_lock_unlock_txt)
{
    static uint8_t lock_unlock_txt_shown = 0;
    if (update_lock_unlock_txt && (!lock_unlock_txt_shown))
    {
        strcpy(gui_param->status_text, STR_UNLOCK);
        lock_unlock_txt_shown = 1;
    }
    else
    {
        sprintf(gui_param->status_text, "DMX: %03d  %-10s",
                sl_worker_get(PARAM_DMXADDR),
                dmx_mode_map[sl_worker_get(PARAM_DMXMODE)].status_text);
        lock_unlock_txt_shown = 0;
    }
}

void render_gui_main(u8g2_t *u8g2, GuiStage stage, GuiMsg msg)
{
    GuiParam gui_param;

    if ((stage >= MAIN_CCT) && (stage <= MAIN_INDEP))
    {
        if (stage == MAIN_CCT)
        {
            sprintf(gui_param.title_text, "[%d]%*c",
                    sl_worker_get(PARAM_LAMPCOUNT),
                    18, ' ');
        }
        else
        {
            sprintf(gui_param.title_text, "[%d] [%s]%*c",
                    sl_worker_get(PARAM_LAMPCOUNT),
                    stage_title_map[stage].title_text,
                    15 - strlen(stage_title_map[stage].title_text), ' ');
        }

        setup_gui_status(&gui_param, (msg == MSG_LOCK_UNLOCK_TXT));
    }

    switch (stage)
    {
    case MAIN_CCT:
        setup_gui_cct(&gui_param);
        break;
    case MAIN_BLINK:
        setup_gui_blink(&gui_param);
        break;
    case MAIN_BREATHE:
        setup_gui_breathe(&gui_param);
        break;
    case MAIN_ROTATE:
        setup_gui_rotate(&gui_param);
        break;
    case MAIN_LIGHTNING:
        setup_gui_lightning(&gui_param);
        break;
    case MAIN_CCT_DRIFT:
        setup_gui_cctdrift(&gui_param);
        break;
    case MAIN_FIRE:
        setup_gui_fire(&gui_param);
        break;
    case MAIN_INDEP:
        setup_gui_indep(&gui_param);
        break;
    default:
        break;
    }
    draw_main(u8g2, &gui_param);
    u8g2_SendBuffer(u8g2);
}
