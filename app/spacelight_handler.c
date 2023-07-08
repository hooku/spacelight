// handler that adapt threadx and stm32f4xx_hal

#include "spacelight.h"

TX_QUEUE qu_gui;
TX_QUEUE qu_worker;
TX_QUEUE qu_input;

TX_THREAD th_gui;
TX_THREAD th_worker;
TX_THREAD th_controller;

GPIO_PinState last_sw2 = GPIO_PIN_SET;
ULONG last_sw2_tick = 0;

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
            status = tx_queue_send(&qu_gui, gui_message, TX_NO_WAIT);
            assert_param(status == TX_SUCCESS);
        }
        // if (worker_message != NULL)
        //{
        //     status = tx_queue_send(&qu_worker, worker_message, TX_NO_WAIT);
        //     assert_param(status == TX_SUCCESS);
        //}
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

    /* enable TIM2 CH1 as SW1 */
    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
#define IS_TUNER(button_type) ((button_type == BTN_DIM_INC) || \
                               (button_type == BTN_DIM_DEC) || \
                               (button_type == BTN_CCT_INC) || \
                               (button_type == BTN_CCT_DEC))
#define IS_REV_TUNER(current_tuner, last_tuner)                         \
    (((current_tuner == BTN_DIM_INC) && (last_tuner == BTN_DIM_DEC)) || \
     ((current_tuner == BTN_DIM_DEC) && (last_tuner == BTN_DIM_INC)) || \
     ((current_tuner == BTN_CCT_INC) && (last_tuner == BTN_CCT_DEC)) || \
     ((current_tuner == BTN_CCT_DEC) && (last_tuner == BTN_CCT_INC)))

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
        button_type = BTN_DIM_INC;
        break;
    case P1_B_Pin:
        button_type = BTN_DIM_DEC;
        break;
    case P3_A_Pin:
        button_type = BTN_CCT_INC;
        break;
    case P3_B_Pin:
        button_type = BTN_CCT_DEC;
        break;
    default:
        button_type = BTN_UNKNOWN;
        break;
    }

    ULONG current_event_tick = tx_time_get();
    static ULONG last_event_tick = 0;
    static ButtonType last_tuner = BTN_UNKNOWN;

    if (IS_TUNER(button_type))
    {
        if ((current_event_tick - last_event_tick) < DEBOUNCE_TUNER_TICK)
            return;
        if (IS_REV_TUNER(button_type, last_tuner) && ((current_event_tick - last_event_tick) < DEBOUNCE_REV_TUNER_TICK))
            return;
        last_tuner = button_type;
    }
    else
    {
        if ((current_event_tick - last_event_tick) < DEBOUNCE_BUTTON_TICK)
            return;
    }
    last_event_tick = current_event_tick;

    UINT status;
    status = tx_queue_send(&qu_input, &button_type, TX_NO_WAIT);
    assert_param(status == TX_SUCCESS);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    ULONG current_event_tick = tx_time_get();
    static ULONG last_event_tick = 0;

    if ((current_event_tick - last_event_tick) < DEBOUNCE_BUTTON_TICK)
        return;
    last_event_tick = current_event_tick;

    UINT status;
    ButtonType button_type = BTN_DIM_PRESS;
    status = tx_queue_send(&qu_input, &button_type, TX_NO_WAIT);
    assert_param(status == TX_SUCCESS);
}
