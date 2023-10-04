#include "effect/spacelight_effect.h"
#include "spacelight.h"

TX_QUEUE qu_gui;
TX_QUEUE qu_effect;
TX_QUEUE qu_input;

TX_THREAD th_gui;
TX_THREAD th_effect;
TX_THREAD th_controller;

/*
 * Gui thread updates only when received a msg
 */
void thread_gui(ULONG param)
{
    gui_init();

    while (1)
    {
        ULONG gui_message;
        UINT status;
        status = tx_queue_receive(&qu_gui, &gui_message, TX_WAIT_FOREVER);
        assert_param(status == TX_SUCCESS);

        gui_update(GUI_MSG_STAGE(gui_message), GUI_MSG_MSG(gui_message));
    }
}

/*
 * Effect thread always loop until it received a msg
 */
void thread_effect(ULONG param)
{
    while (1)
    {
        ULONG effect_message;
        UINT status;
        status = tx_queue_receive(&qu_effect, &effect_message, TX_NO_WAIT);
        assert_param(status == TX_SUCCESS || status == TX_QUEUE_EMPTY);

        MainStage stage = (status == TX_SUCCESS) ? STAGE_EMPTY : EFFECT_MSG_STAGE(effect_message);
        spacelight_effect(stage);
    }
}

/*
 * Controller thread processes only when received a key/dmx512 input
 */
void thread_controller(ULONG param)
{
    ButtonType button_type;
    void *gui_message = NULL,
         *worker_message = NULL;
    UINT status;

    // draw initial GUI
    controller(BTN_BACK, &gui_message, worker_message);
    gui_refresh(*(uint16_t *)gui_message, MSG_NONE);

    while (1)
    {
        // receive the event that generated from GPIO interrupt
        status = tx_queue_receive(&qu_input, &button_type, TX_WAIT_FOREVER);
        assert_param(status == TX_SUCCESS);

        gui_message = NULL;
        controller(button_type, &gui_message, &worker_message);
        if (gui_message != NULL)
        {
            gui_refresh(*(uint16_t *)gui_message, MSG_NONE);
        }
    }
}

void gui_refresh(uint16_t stage, GuiMsg msg)
{
    ULONG gui_message = GUI_MSG(stage, msg);
    UINT status;
    status = tx_queue_send(&qu_gui, &gui_message, TX_NO_WAIT);
    assert_param(status == TX_SUCCESS);
}

void entry(TX_BYTE_POOL tx_app_byte_pool)
{
    void *ptr;

    /* create queues */
    tx_byte_allocate(&tx_app_byte_pool, &ptr, TH_GUI_QUEUE_SIZE * sizeof(ULONG), TX_NO_WAIT);
    tx_queue_create(&qu_gui, "gui", TX_1_ULONG, ptr, TH_GUI_QUEUE_SIZE * sizeof(ULONG));

    tx_byte_allocate(&tx_app_byte_pool, &ptr, TH_EFFECT_QUEUE_SIZE * sizeof(ULONG), TX_NO_WAIT);
    tx_queue_create(&qu_gui, "worker", TX_1_ULONG, ptr, TH_EFFECT_QUEUE_SIZE * sizeof(ULONG));

    tx_byte_allocate(&tx_app_byte_pool, &ptr, TH_INPUT_QUEUE_SIZE * sizeof(ULONG), TX_NO_WAIT);
    tx_queue_create(&qu_input, "key input", TX_1_ULONG, ptr, TH_INPUT_QUEUE_SIZE * sizeof(ULONG));

    /* create threads*/
    tx_byte_allocate(&tx_app_byte_pool, &ptr, TH_GUI_STACK_SIZE, TX_NO_WAIT);
    tx_thread_create(&th_gui, "gui", thread_gui, 0,
                     ptr, TH_GUI_STACK_SIZE,
                     1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

    tx_byte_allocate(&tx_app_byte_pool, &ptr, TH_EFFECT_STACK_SIZE, TX_NO_WAIT);
    tx_thread_create(&th_effect, "effect", thread_effect, 0,
                     ptr, TH_EFFECT_STACK_SIZE,
                     1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

    tx_byte_allocate(&tx_app_byte_pool, &ptr, TH_INPUT_STACK_SIZE, TX_NO_WAIT);
    tx_thread_create(&th_controller, "controller", thread_controller, 0,
                     ptr, TH_INPUT_STACK_SIZE,
                     1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* init hals */
    hal_tmr_init();
    hal_uart_dma_init();

    worker_init();
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
