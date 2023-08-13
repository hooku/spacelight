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

#define GUI_INDEP_CTL_BAR_WIDTH (GUI_SCREEN_WIDTH / 2)
#define GUI_INDEP_LCTL_BAR_LEFT 0
#define GUI_INDEP_RCTL_BAR_LEFT (GUI_INDEP_LCTL_BAR_LEFT + GUI_INDEP_CTL_BAR_WIDTH + GUI_LINE_WIDTH)

#define GUI_CTL_SQUEEZE_HEIGHT 2

#define GUI_TEXT_CTL_MAX_LINE 4

/* GUI element fonts */
#define GUI_TITLE_FONT u8g2_font_6x10_tr

#define GUI_LCTL_FONT_TWO_LINE_0 u8g2_font_pxplusibmvga8_tr
#define GUI_LCTL_FONT_TWO_LINE_1 u8g2_font_crox4h_tr
#define GUI_LCTL_FONT_THREE_LINE u8g2_font_profont11_mr
#define GUI_LCTL_FONT_FOUR_LINE u8g2_font_profont11_mr

#define GUI_STATUS_FONT u8g2_font_profont11_tr

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
    uint8_t count;
    SlParaName name[GUI_TEXT_CTL_MAX_LINE];
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
        if (str[i_ch] == '\002') /* start of text */
            highlight_start = i_ch;
        if (highlight_start >= 0)
        {
            if (str[i_ch] == '\003') /* end of text */
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
    u8g2_SetFontMode(u8g2, 0);
    u8g2_uint_t str_width = u8g2_GetStrWidth(u8g2, str);
    u8g2_uint_t center_x = pos->left + (pos->right - pos->left) / 2;
    u8g2_DrawStr(u8g2, center_x - str_width / 2, pos->bottom, str);

    /* draw high light text over normal text */
    char highlight_text[MAX_TEXT_LEN_LONG] = {0};
    int16_t highlight_start = get_highlight_text(str, highlight_text);
    // int16_t highlight_start = 1;
    if (highlight_start >= 0)
    {
        u8g2_SetDrawColor(u8g2, 0);
        str[highlight_start] = 0;
        u8g2_uint_t hightlight_left_width = u8g2_GetStrWidth(u8g2, str);
        u8g2_DrawStr(u8g2, center_x - str_width / 2 + hightlight_left_width, pos->bottom, highlight_text);
        u8g2_SetDrawColor(u8g2, 1);
    }
}

static void draw_title_bar(u8g2_t *u8g2, GuiParam *gui_param)
{
    u8g2_SetFont(u8g2, GUI_TITLE_FONT);
    Rect title_bar_pos = {
        0, 0, GUI_TITLE_BAR_WIDTH, GUI_TITLE_BAR_HEIGHT + u8g2_GetDescent(u8g2)};
    draw_text(u8g2, &title_bar_pos, gui_param->title_text);

    /* bottom line */
    u8g2_DrawLine(u8g2, 0, GUI_TITLE_BAR_TOP + GUI_TITLE_BAR_HEIGHT + GUI_LINE_WIDTH, GUI_SCREEN_WIDTH, GUI_TITLE_BAR_TOP + GUI_TITLE_BAR_HEIGHT + GUI_LINE_WIDTH);
}

static void draw_status_bar(u8g2_t *u8g2, GuiParam *gui_param)
{
    u8g2_SetFont(u8g2, GUI_STATUS_FONT);
    Rect status_bar_pos = {
        GUI_STATUS_BAR_LEFT, GUI_STATUS_BAR_TOP, GUI_STATUS_BAR_WIDTH, GUI_STATUS_BAR_TOP + GUI_STATUS_BAR_HEIGHT + u8g2_GetDescent(u8g2)};
    draw_text(u8g2, &status_bar_pos, gui_param->status_text);

    /* top line */
    u8g2_DrawLine(u8g2, 0, GUI_LCTL_BAR_TOP + GUI_LCTL_BAR_HEIGHT + GUI_LINE_WIDTH, GUI_SCREEN_WIDTH, GUI_LCTL_BAR_TOP + GUI_LCTL_BAR_HEIGHT + GUI_LINE_WIDTH);
}

static void getLineText(DrawCtlParam *ctl, int line, char *line_text, uint8_t indepIndex)
{
#define IS_DESC_LINE(lin) ((!indepIndex && (lin & 0x1U) == 0) || (indepIndex && (lin == 0)))
#define HAS_TWO_ITEM(index, count) (count >= (index + 2))

    int index_start = (ctl->count == 1) ? 0 : (line & 0xEU); /* remove last bit to calculate index start */

    if (IS_DESC_LINE(line))
    {
        if (HAS_TWO_ITEM(index_start, ctl->count))
        {
            sprintf(line_text, (indepIndex ? "  %3s%5s" : "%6s%6s"), name_param_map[ctl->name[index_start]].desc, name_param_map[ctl->name[index_start + 1]].desc);
        }
        else
        {
            sprintf(line_text, (ctl->count == 1 ? "%s" : "%6s      "), name_param_map[ctl->name[index_start]].desc);
        }
    }
    else // value line
    {
        if (HAS_TWO_ITEM(index_start, ctl->count))
        {
            if (indepIndex)
            {
                uint8_t indepRealIndex = (indepIndex + line - 1);
                sprintf(line_text, "%d \002%3d\003%5d", indepRealIndex, sl_worker_get(ctl->name[index_start]), sl_worker_get(ctl->name[index_start + 1]));
            }
            else
            {
                sprintf(line_text, "\002%6d\003%6d", sl_worker_get(ctl->name[index_start]), sl_worker_get(ctl->name[index_start + 1]));
            }
        }
        else
        {
            char modifier = '\000';
            if (ctl->count == 1)
            {
                modifier = (ctl->name[index_start] == PARAM_DIM) ? '%' : 'K';
                sprintf(line_text, "%d%c", sl_worker_get(ctl->name[index_start]), modifier);
            }
            else
            {
                sprintf(line_text, "%6d      ", sl_worker_get(ctl->name[index_start]));
            }
        }
    }
}

static void draw_ctl(u8g2_t *u8g2, DrawCtlParam *ctl, Rect *pos, uint8_t indepIndex)
{
    int line_count = (ctl->count == 1) ? 2 : (indepIndex ? 3 : 4);
    const uint16_t text_gap = (pos->bottom - pos->top) / line_count;

    if (line_count >= 3)
    {
        u8g2_SetFont(u8g2, (line_count == 3 ? GUI_LCTL_FONT_THREE_LINE : GUI_LCTL_FONT_FOUR_LINE));
    }

    int i_line;
    for (i_line = 0; i_line < line_count; i_line++)
    {
        if (line_count == 2)
        {
            u8g2_SetFont(u8g2, (i_line == 0 ? GUI_LCTL_FONT_TWO_LINE_0 : GUI_LCTL_FONT_TWO_LINE_1));
        }
        else if (line_count == 4)
        {
            if ((i_line & 0xEU) >= ctl->count)
                break;
        }

        char line_text[MAX_TEXT_LEN_LONG] = {0};
        getLineText(ctl, i_line, line_text, indepIndex);

        Rect font_pos = {
            pos->left, pos->top + text_gap * i_line,
            pos->right, pos->top + text_gap * i_line + text_gap / 2 + (u8g2_GetAscent(u8g2) - u8g2_GetDescent(u8g2)) / 2};
        draw_text(u8g2, &font_pos, line_text);
    }
}

static void draw_main(u8g2_t *u8g2, GuiParam *gui_param)
{
    /* center line */
    u8g2_DrawLine(u8g2, GUI_LCTL_BAR_WIDTH + GUI_LINE_WIDTH, GUI_TITLE_BAR_TOP + GUI_TITLE_BAR_HEIGHT + GUI_LINE_WIDTH, GUI_LCTL_BAR_WIDTH + GUI_LINE_WIDTH, GUI_TITLE_BAR_TOP + GUI_TITLE_BAR_HEIGHT + GUI_LCTL_BAR_HEIGHT + GUI_LINE_WIDTH);

    /* left control */
    Rect l_control_pos = {
        GUI_LCTL_BAR_LEFT, GUI_LCTL_BAR_TOP + GUI_CTL_SQUEEZE_HEIGHT / 2,
        GUI_LCTL_BAR_LEFT + GUI_LCTL_BAR_WIDTH,
        GUI_LCTL_BAR_TOP + GUI_LCTL_BAR_HEIGHT - GUI_CTL_SQUEEZE_HEIGHT / 2};
    draw_ctl(u8g2, &gui_param->l_ctl, &l_control_pos, 0U);

    /* right control */
    Rect r_control_pos = {
        GUI_RCTL_BAR_LEFT, GUI_RCTL_BAR_TOP + GUI_CTL_SQUEEZE_HEIGHT / 2,
        GUI_RCTL_BAR_LEFT + GUI_RCTL_BAR_WIDTH,
        GUI_RCTL_BAR_TOP + GUI_RCTL_BAR_HEIGHT - GUI_CTL_SQUEEZE_HEIGHT / 2};
    draw_ctl(u8g2, &gui_param->r_ctl, &r_control_pos, 0U);
}

static void draw_main_indep(u8g2_t *u8g2, GuiParam *gui_param)
{
    /* center line */
    u8g2_DrawLine(u8g2, GUI_INDEP_CTL_BAR_WIDTH + GUI_LINE_WIDTH, GUI_TITLE_BAR_TOP + GUI_TITLE_BAR_HEIGHT + GUI_LINE_WIDTH, GUI_INDEP_CTL_BAR_WIDTH + GUI_LINE_WIDTH, GUI_TITLE_BAR_TOP + GUI_TITLE_BAR_HEIGHT + GUI_LCTL_BAR_HEIGHT + GUI_LINE_WIDTH);

    /* left control */
    Rect l_control_pos = {
        0, GUI_LCTL_BAR_TOP + GUI_CTL_SQUEEZE_HEIGHT / 2,
        GUI_INDEP_LCTL_BAR_LEFT + GUI_INDEP_CTL_BAR_WIDTH,
        GUI_LCTL_BAR_TOP + GUI_LCTL_BAR_HEIGHT - GUI_CTL_SQUEEZE_HEIGHT / 2};
    draw_ctl(u8g2, &gui_param->l_ctl, &l_control_pos, 1U);

    /* right control */
    Rect r_control_pos = {
        GUI_INDEP_RCTL_BAR_LEFT, GUI_RCTL_BAR_TOP + GUI_CTL_SQUEEZE_HEIGHT / 2,
        GUI_INDEP_RCTL_BAR_LEFT + GUI_INDEP_CTL_BAR_WIDTH,
        GUI_RCTL_BAR_TOP + GUI_RCTL_BAR_HEIGHT - GUI_CTL_SQUEEZE_HEIGHT / 2};
    draw_ctl(u8g2, &gui_param->r_ctl, &r_control_pos, 3U);
}

static inline void add_param(DrawCtlParam *ctl_param, SlParaName name)
{
    ctl_param->name[ctl_param->count++] = name;
}

static void setup_gui_cct(GuiParam *gui_param)
{
    add_param(&gui_param->l_ctl, PARAM_DIM);
    add_param(&gui_param->r_ctl, PARAM_CCT);
}

static void setup_gui_blink(GuiParam *gui_param)
{
    add_param(&gui_param->l_ctl, PARAM_DIM);
    add_param(&gui_param->r_ctl, PARAM_CCT);
    add_param(&gui_param->r_ctl, PARAM_BLINK_SPD);
    add_param(&gui_param->r_ctl, PARAM_BLINK_DUTY);
}

static void setup_gui_breathe(GuiParam *gui_param)
{
    add_param(&gui_param->l_ctl, PARAM_DIM);
    add_param(&gui_param->r_ctl, PARAM_CCT);
    add_param(&gui_param->r_ctl, PARAM_BREATHE_SPD);
}

static void setup_gui_rotate(GuiParam *gui_param)
{
    add_param(&gui_param->l_ctl, PARAM_DIM);
    add_param(&gui_param->r_ctl, PARAM_CCT);
    add_param(&gui_param->r_ctl, PARAM_ROTATE_SPD);
}

static void setup_gui_lightning(GuiParam *gui_param)
{
    add_param(&gui_param->l_ctl, PARAM_DIM);
    add_param(&gui_param->r_ctl, PARAM_CCT);
    add_param(&gui_param->r_ctl, PARAM_LIGHTNING_SPD);
    add_param(&gui_param->r_ctl, PARAM_LIGHTNING_MAG);
}

static void setup_gui_cctdrift(GuiParam *gui_param)
{
    add_param(&gui_param->l_ctl, PARAM_DIM);
    add_param(&gui_param->r_ctl, PARAM_CCT_DRIFT_SPD);
    add_param(&gui_param->r_ctl, PARAM_CCT_DRIFT_CCT1);
    add_param(&gui_param->r_ctl, PARAM_CCT_DRIFT_CCT2);
}

static void setup_gui_fire(GuiParam *gui_param)
{
    add_param(&gui_param->l_ctl, PARAM_DIM);
    add_param(&gui_param->r_ctl, PARAM_CCT);
    add_param(&gui_param->r_ctl, PARAM_FIRE_SPD);
    add_param(&gui_param->r_ctl, PARAM_FIRE_MAG);
}

static void setup_gui_indep(GuiParam *gui_param)
{
    add_param(&gui_param->l_ctl, PARAM_INDEP_DIM1);
    add_param(&gui_param->l_ctl, PARAM_INDEP_CCT1);
    add_param(&gui_param->l_ctl, PARAM_INDEP_DIM2);
    add_param(&gui_param->l_ctl, PARAM_INDEP_CCT2);
    add_param(&gui_param->r_ctl, PARAM_INDEP_DIM3);
    add_param(&gui_param->r_ctl, PARAM_INDEP_CCT3);
    add_param(&gui_param->r_ctl, PARAM_INDEP_DIM4);
    add_param(&gui_param->r_ctl, PARAM_INDEP_CCT4);
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
    GuiParam gui_param = {0};

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

    sl_gui_clear();
    draw_title_bar(u8g2, &gui_param);
    draw_status_bar(u8g2, &gui_param);

    if (stage != MAIN_INDEP)
    {
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
        default:
            break;
        }
        draw_main(u8g2, &gui_param);
    }
    else
    {
        setup_gui_indep(&gui_param);
        draw_main_indep(u8g2, &gui_param);
    }
    u8g2_SendBuffer(u8g2);
}
