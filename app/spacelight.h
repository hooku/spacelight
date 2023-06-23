#ifndef __SPACELIGHT_H_
#define __SPACELIGHT_H_

#include "main.h"

#include "app_azure_rtos.h"

#include "u8g2.h"

#define VER_HW_MAJOR 1
#define VER_HW_MINOR 0
#define VER_BT_MAJOR 1
#define VER_BT_MINOR 0
#define VER_SW_MAJOR 1
#define VER_SW_MINOR 0
#define VER_SN "12345678"

#define SPACELIGHT_GUI_STACK_SIZE 1024
#define SPACELIGHT_WORKER_STACK_SIZE 512
#define SPACELIGHT_INPUT_STACK_SIZE 512

#define SPACELIGHT_GUI_QUEUE_SIZE 32
#define SPACELIGHT_WORKER_QUEUE_SIZE 32
#define SPACELIGHT_INPUT_QUEUE_SIZE 32

#define GUI_SCREEN_WIDTH 128
#define GUI_SCREEN_HEIGHT 64
#define GUI_TEXT_LEN 32

#define MENU_MAIN_ITEM_COUNT 9
#define MENU_EFFECT_MODE_ITEM_COUNT 8
#define MENU_DMX_MODE_ITEM_COUNT 3
#define MENU_WIRELESS_ITEM_COUNT 2
#define MENU_VER_ITEM_COUNT 6

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
    BTN_UNKNOWN
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
    MENU_MAIN,
    MENU_EFFECT_MODE,
    CFG_LAMP_COUNT,
    CFG_DMX_ADDR,
    CFG_DMX_MODE,
    CFG_WIRELESS,
    CFG_LOCK_TIME,
    CFG_VERSION,
    GUI_UNINITIALIZED,
} GuiStage;

extern SPI_HandleTypeDef hspi1;

extern void spacelight_entry(TX_BYTE_POOL tx_app_byte_pool);

extern void spacelight_controller(ButtonType button_type, void **gui_message, void **worker_message);

extern void spacelight_gui_init();
extern void spacelight_gui_update(void *gui_message);

extern void render_gui_main(u8g2_t *u8g2, GuiStage gui_stage);

extern void init_gui_menu();
extern void render_gui_menu(u8g2_t *u8g2, GuiStage gui_stage);

extern void render_gui_dmxaddr(u8g2_t *u8g2);
extern void render_gui_lampcount(u8g2_t *u8g2);
extern void render_gui_locktime(u8g2_t *u8g2);

#endif // __SPACELIGHT_H_
