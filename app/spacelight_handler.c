// handler that adapt threadx and stm32f4xx_hal

#include "spacelight.h"

TX_QUEUE qu_gui;
TX_QUEUE qu_worker;
TX_QUEUE qu_input;

TX_THREAD th_gui;
TX_THREAD th_worker;
TX_THREAD th_controller;

void thread_gui(ULONG param)
{
    spacelight_gui_init();

    while (1)
    {
        GuiStage gui_message;
        UINT status;
        status = tx_queue_receive(&qu_gui, &gui_message, TX_WAIT_FOREVER);
        assert_param(status == TX_SUCCESS);

        spacelight_gui_update(&gui_message);
    }
}

void thread_worker(ULONG param)
{
    while (1)
    {
        // void *worker_message = NULL;
        // UINT status;
        // status = tx_queue_receive(&qu_worker, worker_message, TX_NO_WAIT);
        // assert_param(status == TX_SUCCESS);

        // spacelight_worker(worker_message);

        tx_thread_sleep(50);
    }
}

void thread_controller(ULONG param)
{
    ButtonType button_type;
    void *gui_message = NULL,
         *worker_message = NULL;
    UINT status;

    // draw initial GUI
    spacelight_controller(BTN_BACK, &gui_message, worker_message);
    status = tx_queue_send(&qu_gui, gui_message, TX_WAIT_FOREVER);
    assert_param(status == TX_SUCCESS);

    while (1)
    {
        // receive the event that generated from GPIO interrupt
        status = tx_queue_receive(&qu_input, &button_type, TX_WAIT_FOREVER);
        assert_param(status == TX_SUCCESS);

        spacelight_controller(button_type, &gui_message, &worker_message);
        if (gui_message != NULL)
        {
            status = tx_queue_send(&qu_gui, gui_message, TX_WAIT_FOREVER);
            assert_param(status == TX_SUCCESS);
        }
        if (worker_message != NULL)
        {
            status = tx_queue_send(&qu_worker, worker_message, TX_WAIT_FOREVER);
            assert_param(status == TX_SUCCESS);
        }
    }
}

void spacelight_entry(TX_BYTE_POOL tx_app_byte_pool)
{
    void *ptr;

    /* create queues */
    tx_byte_allocate(&tx_app_byte_pool, &ptr, SPACELIGHT_GUI_QUEUE_SIZE * sizeof(ULONG), TX_NO_WAIT);
    tx_queue_create(&qu_gui, "gui", TX_1_ULONG, ptr, SPACELIGHT_GUI_QUEUE_SIZE * sizeof(ULONG));

    tx_byte_allocate(&tx_app_byte_pool, &ptr, SPACELIGHT_WORKER_QUEUE_SIZE * sizeof(ULONG), TX_NO_WAIT);
    tx_queue_create(&qu_gui, "worker", TX_1_ULONG, ptr, SPACELIGHT_WORKER_QUEUE_SIZE * sizeof(ULONG));

    tx_byte_allocate(&tx_app_byte_pool, &ptr, SPACELIGHT_INPUT_QUEUE_SIZE * sizeof(ULONG), TX_NO_WAIT);
    tx_queue_create(&qu_input, "key input", TX_1_ULONG, ptr, SPACELIGHT_INPUT_QUEUE_SIZE * sizeof(ULONG));

    /* create threads*/
    tx_byte_allocate(&tx_app_byte_pool, &ptr, SPACELIGHT_GUI_STACK_SIZE, TX_NO_WAIT);
    tx_thread_create(&th_gui, "gui", thread_gui, 0,
                     ptr, SPACELIGHT_GUI_STACK_SIZE,
                     1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

    tx_byte_allocate(&tx_app_byte_pool, &ptr, SPACELIGHT_WORKER_STACK_SIZE, TX_NO_WAIT);
    tx_thread_create(&th_worker, "worker", thread_worker, 0,
                     ptr, SPACELIGHT_WORKER_STACK_SIZE,
                     1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

    tx_byte_allocate(&tx_app_byte_pool, &ptr, SPACELIGHT_INPUT_STACK_SIZE, TX_NO_WAIT);
    tx_thread_create(&th_controller, "controller", thread_controller, 0,
                     ptr, SPACELIGHT_INPUT_STACK_SIZE,
                     1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
#define DEBOUNCE_TICK 10

    ULONG current_event_tick = tx_time_get();
    static ULONG last_event_tick = 0;
    if (current_event_tick - last_event_tick >= DEBOUNCE_TICK)
    {
        last_event_tick = current_event_tick;
    }
    else
    {
        return;
    }

    ButtonType button_type;
    switch (GPIO_Pin)
    {
    case KEY1_Pin:
        button_type = BTN_MENU;
        break;
    case KEY2_Pin:
        button_type = BTN_5600K;
        break;
    case KEY3_Pin:
        button_type = BTN_BACK;
        break;
    case KEY4_Pin:
        button_type = BTN_3200K;
        break;
    case P1_A_Pin:
        button_type = BTN_DIM_UP;
        break;
    case P1_B_Pin:
        button_type = BTN_DIM_DOWN;
        break;
    case P3_A_Pin:
        button_type = BTN_CCT_UP;
        break;
    case P3_B_Pin:
        button_type = BTN_CCT_DOWN;
        break;
    }

    UINT status;
    status = tx_queue_send(&qu_input, &button_type, TX_NO_WAIT);
    assert_param(status == TX_SUCCESS);
}
