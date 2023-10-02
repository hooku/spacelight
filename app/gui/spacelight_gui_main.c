#include "spacelight_gui.h"
#include "../worker/spacelight_worker.h"
#include "spacelight_param.h"

#include <stdio.h>

/*
    title bar (number of head + title)
    ------------------------
                |
     left ctl   | right ctl
                |
    ------------------------
    stauts bar (address + mode)
*/

/* units in px */
#define GUI_ICON_WIDTH 8
#define GUI_ICON_HEIGHT 8

#define GUI_LINE_WIDTH 1

#define GUI_TITLE_BAR_LEFT 0
#define GUI_TITLE_BAR_TOP 0
#define GUI_TITLE_BAR_WIDTH GUI_SCREEN_WIDTH
#define GUI_TITLE_BAR_HEIGHT 9

#define GUI_TITLE_BAR_ICON_LEFT 100
#define GUI_TITLE_BAR_ICON_TOP 0
#define GUI_TITLE_BAR_ICON_GAP 2
#define GUI_TITLE_BAR_ICON_COUNT 3

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

typedef struct
{
    const uint8_t *icon;
    bool (*func)();
} IconFuncMap;

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

/* XBM ICON */
static const uint8_t icon_lock[] = {0x18, 0x24, 0x24, 0x7e, 0x7e, 0x66, 0x7e, 0x7e};
static const uint8_t icon_upload[] = {0x18, 0x3c, 0x7e, 0xff, 0x18, 0x18, 0x18, 0xff};
static const uint8_t icon_dowload[] = {0x18, 0x18, 0x18, 0xff, 0x7e, 0x3c, 0x18, 0xff};
static const IconFuncMap icon_map[] = {
    {icon_lock, worker_locktime_is_locked},
    {icon_upload, worker_locktime_is_locked},
    {icon_dowload, worker_locktime_is_locked}};

extern SlParaName get_cursor();

static void draw_title_bar_icon(u8g2_t *u8g2)
{
    for (int i_ico = 0; i_ico < GUI_TITLE_BAR_ICON_COUNT; i_ico++)
    {
        u8g2_uint_t ico_left = GUI_TITLE_BAR_ICON_LEFT + (GUI_ICON_WIDTH + GUI_TITLE_BAR_ICON_GAP) * i_ico;
        if (icon_map[i_ico].func())
            u8g2_DrawXBM(u8g2, ico_left, GUI_TITLE_BAR_ICON_TOP, GUI_ICON_WIDTH, GUI_ICON_HEIGHT, icon_map[i_ico].icon);
    }
}

static void draw_title_bar(u8g2_t *u8g2, GuiParam *gui_param)
{
    /* bottom line */
    u8g2_DrawLine(u8g2, 0, GUI_TITLE_BAR_TOP + GUI_TITLE_BAR_HEIGHT + GUI_LINE_WIDTH, GUI_SCREEN_WIDTH, GUI_TITLE_BAR_TOP + GUI_TITLE_BAR_HEIGHT + GUI_LINE_WIDTH);

    u8g2_SetFont(u8g2, GUI_TITLE_FONT);
    Rect title_bar_pos = {
        0, 0, GUI_TITLE_BAR_WIDTH, GUI_TITLE_BAR_HEIGHT + u8g2_GetDescent(u8g2)};
    gui_text(u8g2, &title_bar_pos, gui_param->title_text);

    draw_title_bar_icon(u8g2);
}

static void draw_status_bar(u8g2_t *u8g2, GuiParam *gui_param)
{
    /* top line */
    u8g2_DrawLine(u8g2, 0, GUI_LCTL_BAR_TOP + GUI_LCTL_BAR_HEIGHT + GUI_LINE_WIDTH, GUI_SCREEN_WIDTH, GUI_LCTL_BAR_TOP + GUI_LCTL_BAR_HEIGHT + GUI_LINE_WIDTH);

    u8g2_SetFont(u8g2, GUI_STATUS_FONT);
    Rect status_bar_pos = {
        GUI_STATUS_BAR_LEFT, GUI_STATUS_BAR_TOP, GUI_STATUS_BAR_WIDTH, GUI_STATUS_BAR_TOP + GUI_STATUS_BAR_HEIGHT + u8g2_GetDescent(u8g2)};
    gui_text(u8g2, &status_bar_pos, gui_param->status_text);
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
        SlParaName cursor = get_cursor();
        SlParaName item1_name = ctl->name[index_start];
        uint16_t item1_val = worker_get(item1_name);

        if (HAS_TWO_ITEM(index_start, ctl->count))
        {
            SlParaName item2_name = ctl->name[index_start + 1];
            uint16_t item2_val = worker_get(item2_name);

            if (indepIndex)
            {
                uint8_t indepRealIndex = (indepIndex + line - 1);
                sprintf(line_text, "%d %c%03d%c %c%4d%c", indepRealIndex,
                        item1_name == cursor ? '\002' : '\003', item1_val,
                        item1_name == cursor ? '\003' : '\003',
                        item2_name == cursor ? '\002' : '\003', item2_val,
                        item2_name == cursor ? '\003' : '\003');
            }
            else
            {
                sprintf(line_text, "%c%6d%c%c%6d%c",
                        item1_name == cursor ? '\002' : '\003', item1_val,
                        item1_name == cursor ? '\003' : '\003',
                        item2_name == cursor ? '\002' : '\003', item2_val,
                        item2_name == cursor ? '\003' : '\003');
            }
        }
        else
        {
            char modifier = '\000';
            if (ctl->count == 1)
            {
                modifier = (item1_name == PARAM_DIM) ? '%' : 'K';
                sprintf(line_text, "%d%c", item1_val, modifier);
            }
            else
            {
                sprintf(line_text, "%c%6d%c      ",
                        item1_name == cursor ? '\002' : '\003', item1_val,
                        item1_name == cursor ? '\003' : '\003');
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
        gui_text(u8g2, &font_pos, line_text);
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
                worker_get(PARAM_DMXADDR),
                dmx_mode_map[worker_get(PARAM_DMXMODE)].status_text);
        lock_unlock_txt_shown = 0;
    }
}

void render_gui_main(u8g2_t *u8g2, GuiStage stage, Msg msg)
{
    GuiParam gui_param = {0};

    if ((stage >= MAIN_CCT) && (stage <= MAIN_INDEP))
    {
        if (stage == MAIN_CCT)
        {
            sprintf(gui_param.title_text, "[%d]%*c",
                    worker_get(PARAM_LAMPCOUNT),
                    18, ' ');
        }
        else
        {
            sprintf(gui_param.title_text, "[%d] [%s]%*c",
                    worker_get(PARAM_LAMPCOUNT),
                    stage_title_map[stage].title_text,
                    15 - strlen(stage_title_map[stage].title_text), ' ');
        }

        setup_gui_status(&gui_param, (msg == MSG_LOCK_UNLOCK_TXT));
    }

    gui_clear();
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
