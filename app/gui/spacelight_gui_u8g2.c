#include "spacelight.h"

static u8g2_t u8g2;

uint8_t u8x8_byte_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,
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

uint8_t u8x8_stm32_gpio_and_delay(U8X8_UNUSED u8x8_t *u8x8,
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

void spacelight_gui_init()
{
#define SPACELIGHT_CONTRAST 1

    u8g2_Setup_ssd1309_128x64_noname0_f(&u8g2, U8G2_R2, u8x8_byte_4wire_hw_spi,
                                        u8x8_stm32_gpio_and_delay);
    u8g2_InitDisplay(&u8g2);     // send init sequence to the display, display is in
                                 // sleep mode after this,
    u8g2_SetPowerSave(&u8g2, 0); // wake up display

    u8g2_SetContrast(&u8g2, SPACELIGHT_CONTRAST);

    u8g2_ClearDisplay(&u8g2);

    init_gui_menu();
}

void spacelight_gui_update(void *gui_message)
{
    GuiStage *gui_stage = (GuiStage *)gui_message;
    static GuiStage last_gui_stage = GUI_UNINITIALIZED;
    
    switch (*gui_stage)
    {
    case MENU_MAIN:
    case MENU_EFFECT_MODE:
    case CFG_DMX_MODE:
    case CFG_WIRELESS:
    case CFG_VERSION:
        render_gui_menu(&u8g2, *gui_stage, last_gui_stage);
        break;
    case CFG_LAMP_COUNT:
        render_gui_lampcount(&u8g2);
        break;
    case CFG_DMX_ADDR:
        render_gui_dmxaddr(&u8g2);
        break;
    case CFG_LOCK_TIME:
        render_gui_locktime(&u8g2);
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
        render_gui_main(&u8g2, *gui_stage);
        break;
    }
    
    last_gui_stage = *gui_stage;
}
