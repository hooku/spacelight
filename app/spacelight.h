#ifndef __SPACELIGHT_H_
#define __SPACELIGHT_H_

#include "main.h"

#include "app_azure_rtos.h"

#include "u8g2.h"

#define SPACELIGHT_GUI_STACK_SIZE 1024
#define SPACELIGHT_WORKER_STACK_SIZE 512
#define SPACELIGHT_INPUT_STACK_SIZE 512

#define SPACELIGHT_GUI_QUEUE_SIZE 32
#define SPACELIGHT_WORKER_QUEUE_SIZE 32
#define SPACELIGHT_INPUT_QUEUE_SIZE 32

#define GUI_SCREEN_WIDTH 128
#define GUI_SCREEN_HEIGHT 64
#define GUI_TEXT_LEN 32

typedef enum
{
    BTN_MENU,
    BTN_BACK,
    BTN_3200K,
    BTN_5600K,
    BTN_DIM_UP,
    BTN_DIM_DOWN,
    BTN_CCT_UP,
    BTN_CCT_DOWN,
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
    MENU,
    CFG_LAMP_COUNT,
    CFG_DMX_ADDR,
    CFG_DMX_MODE,
    CFG_WIRELESS,
    CFG_LOCK_TIME,
    CFG_VERSION,
} GuiStage;

extern SPI_HandleTypeDef hspi1;

extern void spacelight_entry(TX_BYTE_POOL tx_app_byte_pool);

extern void spacelight_controller(ButtonType button_type, void **gui_message, void **worker_message);

extern void spacelight_gui_init();
extern void spacelight_gui_update(void *gui_message);

extern void render_gui_main(u8g2_t *u8g2, GuiStage gui_stage);
extern void render_gui_menu(u8g2_t *u8g2, GuiStage gui_stage);

#endif // __SPACELIGHT_H_
