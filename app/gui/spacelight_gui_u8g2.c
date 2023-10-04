#include "spacelight_gui.h"

static u8g2_t u8g2;

static uint8_t u8x8_byte_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,
                                      void *arg_ptr)
{
    switch (msg)
    {
    case U8X8_MSG_BYTE_SEND:
        HAL_SPI_Transmit(&hspi1, (uint8_t *)arg_ptr, arg_int, 10000);
        break;
    case U8X8_MSG_BYTE_INIT:
        break;
    case U8X8_MSG_BYTE_SET_DC:
        HAL_GPIO_WritePin(D_C_GPIO_Port, D_C_Pin, (GPIO_PinState)arg_int);
        break;
    case U8X8_MSG_BYTE_START_TRANSFER:
        u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_enable_level);
        break;
    case U8X8_MSG_BYTE_END_TRANSFER:
        u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);
        break;
    default:
        return 0;
    }
    return 1;
}

static uint8_t u8x8_stm32_gpio_and_delay(U8X8_UNUSED u8x8_t *u8x8,
                                         uint8_t msg, uint8_t arg_int,
                                         U8X8_UNUSED void *arg_ptr)
{
    switch (msg)
    {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        HAL_Delay(1);
        break;
    case U8X8_MSG_DELAY_MILLI:
        HAL_Delay(arg_int);
        break;
    case U8X8_MSG_GPIO_CS:
        HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, (GPIO_PinState)arg_int);
        break;
    case U8X8_MSG_GPIO_DC:
        HAL_GPIO_WritePin(D_C_GPIO_Port, D_C_Pin, (GPIO_PinState)arg_int);
        break;
    case U8X8_MSG_GPIO_RESET:
        HAL_GPIO_WritePin(RES_GPIO_Port, RES_Pin, (GPIO_PinState)arg_int);
        break;
    }
    return 1;
}

static void get_visible_str(const char *str, char *visible_str)
{
    for (int i_ch = 0, i_visible = 0; (i_ch < MAX_TEXT_LEN_LONG) && (str[i_ch] != 0); i_ch++)
    {
        if (str[i_ch] >= '\040') /* space char */
            visible_str[i_visible++] = str[i_ch];
    }
}

static int16_t get_highlight_text(const char *str, char *highlight)
{
    int16_t visible_ch_count = 0;
    bool highlight_found = false;
    for (int i_ch = 0, i_highlight = 0; (i_ch < MAX_TEXT_LEN_LONG) && (str[i_ch] != 0); i_ch++)
    {
        if (highlight_found)
        {
            if (str[i_ch] == '\003') /* end of text */
                break;

            highlight[i_highlight++] = str[i_ch];
        }
        else
        {
            visible_ch_count += (str[i_ch] >= '\040') ? 1 : 0;
        }
        if (str[i_ch] == '\002') /* start of text */
            highlight_found = true;
    }

    return highlight_found ? visible_ch_count : -1;
}

/* Draw text on screen
 *
 * @param u8g2 u8g2 handle ptr
 * @param pos Text region rectangle
 * @param str Text to draw, highlight
 */
void gui_text(u8g2_t *u8g2, Rect *pos, const char *str)
{
    char visible_str[MAX_TEXT_LEN_LONG] = {0};
    get_visible_str(str, visible_str);

    u8g2_SetFontMode(u8g2, 0);
    u8g2_uint_t str_width = u8g2_GetStrWidth(u8g2, visible_str);
    u8g2_uint_t center_x = pos->left + (pos->right - pos->left) / 2;
    u8g2_DrawStr(u8g2, center_x - str_width / 2, pos->bottom, visible_str);

    /* draw high light text over normal text */
    char highlight_text[MAX_TEXT_LEN_LONG] = {0};
    int16_t highlight_start = get_highlight_text(str, highlight_text);
    if (highlight_start >= 0)
    {
        u8g2_SetDrawColor(u8g2, 0);
        visible_str[highlight_start] = 0;
        u8g2_uint_t hightlight_left_width = u8g2_GetStrWidth(u8g2, visible_str);
        u8g2_DrawStr(u8g2, center_x - str_width / 2 + hightlight_left_width, pos->bottom, highlight_text);
        u8g2_SetDrawColor(u8g2, 1);
    }
}

void gui_init()
{
#define GUI_CONTRAST 1

    u8g2_Setup_ssd1309_128x64_noname0_f(&u8g2, U8G2_R2, u8x8_byte_4wire_hw_spi,
                                        u8x8_stm32_gpio_and_delay);
    u8g2_InitDisplay(&u8g2);     /* send init sequence to the display, display is in
                                    sleep mode after this */
    u8g2_SetPowerSave(&u8g2, 0); /* wake up display */

    u8g2_SetContrast(&u8g2, GUI_CONTRAST);

    u8g2_ClearDisplay(&u8g2);

    init_gui_menu();
}

void gui_clear()
{
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFontPosBaseline(&u8g2);
}

void gui_update(uint16_t stage, GuiMsg msg)
{
    static uint16_t last_stage = GUI_UNINITIALIZED;
    stage = MSG_BOX;
    switch (stage)
    {
    case MENU_MAIN:
    case MENU_EFFECT_MODE:
    case CFG_DMX_MODE:
    case CFG_WIRELESS:
    case CFG_VERSION:
        render_gui_menu(&u8g2, stage, last_stage);
        break;
    case CFG_LAMP_COUNT:
        render_gui_lampcount(&u8g2, stage, last_stage);
        break;
    case CFG_DMX_ADDR:
        render_gui_dmxaddr(&u8g2, stage, last_stage);
        break;
    case CFG_LOCK_TIME:
        render_gui_locktime(&u8g2, stage, last_stage);
        break;
    case MSG_BOX:
        render_gui_msgbox(&u8g2, stage, last_stage);
        break;
    case MAIN_CCT:
    case MAIN_BLINK:
    case MAIN_BREATHE:
    case MAIN_ROTATE:
    case MAIN_LIGHTNING:
    case MAIN_CCT_DRIFT:
    case MAIN_FIRE:
    case MAIN_INDEP:
    default:
        GuiMsg gui_msg = (GuiMsg)msg;
        render_gui_main(&u8g2, stage, gui_msg);
        break;
    }

    last_stage = stage;
}
