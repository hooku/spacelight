#ifndef __SPACELIGHT_H_
#define __SPACELIGHT_H_

#include "main.h"

#include "app_azure_rtos.h"

#include <stdbool.h>
#include <stdint.h>

#define VER_HW_MAJOR 1
#define VER_HW_MINOR 0
#define VER_BT_MAJOR 1
#define VER_BT_MINOR 0
#define VER_SW_MAJOR 1
#define VER_SW_MINOR 0
#define VER_SN "12345678"

#define SL_GUI_STACK_SIZE 1024
#define SL_WORKER_STACK_SIZE 512
#define SL_INPUT_STACK_SIZE 512

#define SL_GUI_QUEUE_SIZE 32
#define SL_WORKER_QUEUE_SIZE 32
#define SL_INPUT_QUEUE_SIZE 32

#define GUI_SCREEN_WIDTH 128
#define GUI_SCREEN_HEIGHT 64

#define MAX_TEXT_LEN_LONG 32
#define MAX_TEXT_LEN_SHORT 16

#define MENU_MAIN_ITEM_COUNT 9
#define MENU_EFFECT_MODE_ITEM_COUNT 8
#define MENU_DMX_MODE_ITEM_COUNT 3
#define MENU_WIRELESS_ITEM_COUNT 2
#define MENU_VER_ITEM_COUNT 6

#define DEBOUNCE_BUTTON_MS 250U
#define DEBOUNCE_TUNER_MS 60U
#define DEBOUNCE_REV_TUNER_MS 120U

#define CCT_3200K 3200U
#define CCT_5600K 5600U

/* string const */
#define STR_UNLOCK "Press BACK to unlock"

#define STR_CCT ""
#define STR_BLINK "Blink"
#define STR_BREATHE "Breathe"
#define STR_ROTATE "Rotate"
#define STR_LIGHTNING "Lightning"
#define STR_CCTDRIFT "CCT Drift"
#define STR_FIRE "Fire"
#define STR_INDEP "Indep"

#define STR_2CH "DIM & CCT"
#define STR_8CH "Independent"
#define STR_11CH "All param"

#define MS_TO_SEC(ms) (ms / 1000)
#define SEC_TO_MS(ms) (ms * 1000)
#define MS_TO_TICK(ms) (ms * TX_TIMER_TICKS_PER_SECOND / 1000)
#define TICK_TO_MS(tick) (tick * 1000 / TX_TIMER_TICKS_PER_SECOND)

#define IS_MAIN_GUI(stage) ((stage >= MAIN_CCT) && (stage <= MAIN_INDEP))
#define IS_SUB_MENU_CFG(stage) ((stage >= MENU_EFFECT_MODE) && (stage <= CFG_VERSION))
#define GET_INC_DEC(button) (((button == BTN_DIM_INC) || (button == BTN_CCT_INC)) ? INCREASE : DECREASE)

#define GUI_MSG(stage, msg) ((((uint32_t)stage & 0xFFU) << 8) | ((uint32_t)msg & 0xFFU))
#define GUI_MSG_STAGE(gui_msg) (((uint32_t)gui_msg >> 8) & 0xFF)
#define GUI_MSG_MSG(gui_msg) ((uint32_t)gui_msg & 0xFFU)

typedef enum
{
    BTN_MENU,
    BTN_BACK,
    BTN_3200K,
    BTN_5600K,
    BTN_DIM_INC,
    BTN_DIM_DEC,
    BTN_DIM_PRESS,
    BTN_CCT_INC,
    BTN_CCT_DEC,
    BTN_CCT_PRESS,
    BTN_UNKNOWN,
} ButtonType;

typedef enum
{
    MAIN_CCT,
    MAIN_BLINK,
    MAIN_BREATHE,
    MAIN_ROTATE,
    MAIN_LIGHTNING,
    MAIN_CCT_DRIFT,
    MAIN_FIRE,
    MAIN_INDEP,
    MAIN_END,
} MainStage;

/* GuiStage extends MainStage */
typedef enum
{
    GUI_MAIN,
    MENU_MAIN = MAIN_END,
    MENU_EFFECT_MODE,
    CFG_LAMP_COUNT,
    CFG_DMX_ADDR,
    CFG_DMX_MODE,
    CFG_WIRELESS,
    CFG_LOCK_TIME,
    CFG_VERSION,
    GUI_UNINITIALIZED,
} GuiStage;

typedef enum
{
    MSG_LOCK_UNLOCK_TXT,
    MSG_NONE,
} GuiMsg;

typedef enum
{
    DMX_2CH,
    DMX_8CH,
    DMX_11CH,
} DmxMode;

typedef enum
{
    WIRELESS_ON,
    WIRELESS_OFF,
} WirelessMode;

extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim2;

/* controller */
extern void sl_controller(ButtonType button_type, void **gui_message, void **worker_message);

/* handler */
extern void sl_entry(TX_BYTE_POOL tx_app_byte_pool);
extern void sl_gui_refresh(uint16_t stage, GuiMsg msg);

extern TX_QUEUE qu_input;
extern GPIO_PinState last_sw2;
extern ULONG last_sw2_tick;

/* worker */
extern void sl_worker_init();

inline void spacelight_tim_cb(TIM_HandleTypeDef *htim)
{
    GPIO_PinState current_sw2 = HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin);
    if (current_sw2 != last_sw2)
    {
        if (current_sw2 == GPIO_PIN_RESET)
        {
            ULONG current_sw2_tick = tx_time_get();
            if ((current_sw2_tick - last_sw2_tick) >= MS_TO_TICK(DEBOUNCE_BUTTON_MS))
            {
                UINT status;
                ButtonType button_type = BTN_CCT_PRESS;
                status = tx_queue_send(&qu_input, &button_type, TX_NO_WAIT);
                assert_param(status == TX_SUCCESS);

                last_sw2_tick = current_sw2_tick;
            }
        }
        last_sw2 = current_sw2;
    }
}

#endif // __SPACELIGHT_H_
