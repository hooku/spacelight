// handler that adapt threadx and stm32f4xx_hal

#include "spacelight.h"

#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_dma.h"

TX_QUEUE qu_gui;
TX_QUEUE qu_worker;
TX_QUEUE qu_input;

TX_THREAD th_gui;
TX_THREAD th_worker;
TX_THREAD th_controller;

GPIO_PinState last_sw2 = GPIO_PIN_SET;
ULONG last_sw2_tick = 0;

extern TIM_HandleTypeDef htim2;

extern UART_HandleTypeDef huart4;
extern DMA_HandleTypeDef hdma_uart4_tx;

extern UART_HandleTypeDef huart5;
extern DMA_HandleTypeDef hdma_uart5_rx;

void HAL_DMA_TransferComplete_Callback(DMA_HandleTypeDef *hdma);

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

void setup_dmx512()
{
    /* dmx512 input, RE# = LOW */
    HAL_GPIO_WritePin(E485_1_GPIO_Port, E485_1_Pin, GPIO_PIN_RESET);

    /* setup dma for uart5 */
    HAL_DMA_RegisterCallback(&hdma_uart5_rx, HAL_DMA_XFER_HALFCPLT_CB_ID,
                             &HAL_DMA_TransferComplete_Callback);

    /* dmx512 output, DE = HIGH */
    HAL_GPIO_WritePin(E485_2_GPIO_Port, E485_2_Pin, GPIO_PIN_SET);

    /* setup dma for uart4 */
    HAL_DMA_RegisterCallback(&hdma_uart4_tx, HAL_DMA_XFER_HALFCPLT_CB_ID,
                             &HAL_DMA_TransferComplete_Callback);

    /* PIN77 */
    HAL_GPIO_WritePin(PIN77_GPIO_Port, PIN77_Pin, GPIO_PIN_SET);
}

void entry(TX_BYTE_POOL tx_app_byte_pool)
{
    void *ptr;

    /* create queues */
    tx_byte_allocate(&tx_app_byte_pool, &ptr, SL_GUI_QUEUE_SIZE * sizeof(ULONG), TX_NO_WAIT);
    tx_queue_create(&qu_gui, "gui", TX_1_ULONG, ptr, SL_GUI_QUEUE_SIZE * sizeof(ULONG));

    tx_byte_allocate(&tx_app_byte_pool, &ptr, SL_WORKER_QUEUE_SIZE * sizeof(ULONG), TX_NO_WAIT);
    tx_queue_create(&qu_gui, "worker", TX_1_ULONG, ptr, SL_WORKER_QUEUE_SIZE * sizeof(ULONG));

    tx_byte_allocate(&tx_app_byte_pool, &ptr, SL_INPUT_QUEUE_SIZE * sizeof(ULONG), TX_NO_WAIT);
    tx_queue_create(&qu_input, "key input", TX_1_ULONG, ptr, SL_INPUT_QUEUE_SIZE * sizeof(ULONG));

    /* create threads*/
    tx_byte_allocate(&tx_app_byte_pool, &ptr, SL_GUI_STACK_SIZE, TX_NO_WAIT);
    tx_thread_create(&th_gui, "gui", thread_gui, 0,
                     ptr, SL_GUI_STACK_SIZE,
                     1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

    tx_byte_allocate(&tx_app_byte_pool, &ptr, SL_WORKER_STACK_SIZE, TX_NO_WAIT);
    tx_thread_create(&th_worker, "worker", thread_worker, 0,
                     ptr, SL_WORKER_STACK_SIZE,
                     1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

    tx_byte_allocate(&tx_app_byte_pool, &ptr, SL_INPUT_STACK_SIZE, TX_NO_WAIT);
    tx_thread_create(&th_controller, "controller", thread_controller, 0,
                     ptr, SL_INPUT_STACK_SIZE,
                     1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* enable TIM2 CH1 as SW1 */
    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);

    setup_dmx512();

    worker_init();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
#define IS_TUNER(btn_type) ((btn_type == BTN_DIM_INC) || \
                            (btn_type == BTN_DIM_DEC) || \
                            (btn_type == BTN_CCT_INC) || \
                            (btn_type == BTN_CCT_DEC))
#define IS_REV_TUNER(current_tuner, last_tuner)                         \
    (((current_tuner == BTN_DIM_INC) && (last_tuner == BTN_DIM_DEC)) || \
     ((current_tuner == BTN_DIM_DEC) && (last_tuner == BTN_DIM_INC)) || \
     ((current_tuner == BTN_CCT_INC) && (last_tuner == BTN_CCT_DEC)) || \
     ((current_tuner == BTN_CCT_DEC) && (last_tuner == BTN_CCT_INC)))

    ButtonType btn_type;
    switch (GPIO_Pin)
    {
    case KEY1_Pin:
        btn_type = BTN_MENU;
        break;
    case KEY2_Pin:
        btn_type = BTN_5600K;
        break;
    case KEY3_Pin:
        btn_type = BTN_BACK;
        break;
    case KEY4_Pin:
        btn_type = BTN_3200K;
        break;
    case P1_A_Pin:
        btn_type = BTN_DIM_INC;
        break;
    case P1_B_Pin:
        btn_type = BTN_DIM_DEC;
        break;
    case P3_A_Pin:
        btn_type = BTN_CCT_INC;
        break;
    case P3_B_Pin:
        btn_type = BTN_CCT_DEC;
        break;
    default:
        btn_type = BTN_UNKNOWN;
        break;
    }

    ULONG current_event_tick = tx_time_get();
    static ULONG last_event_tick = 0;
    static ButtonType last_tuner = BTN_UNKNOWN;

    if (IS_TUNER(btn_type))
    {
        if ((current_event_tick - last_event_tick) < MS_TO_TICK(DEBOUNCE_TUNER_MS))
            return;
        if (IS_REV_TUNER(btn_type, last_tuner) && ((current_event_tick - last_event_tick) < MS_TO_TICK(DEBOUNCE_REV_TUNER_MS)))
            return;
        last_tuner = btn_type;
    }
    else
    {
        if ((current_event_tick - last_event_tick) < MS_TO_TICK(DEBOUNCE_BUTTON_MS))
            return;
    }
    last_event_tick = current_event_tick;

    UINT status;
    status = tx_queue_send(&qu_input, &btn_type, TX_NO_WAIT);
    assert_param(status == TX_SUCCESS);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    ULONG current_event_tick = tx_time_get();
    static ULONG last_event_tick = 0;

    if ((current_event_tick - last_event_tick) < MS_TO_TICK(DEBOUNCE_BUTTON_MS))
        return;
    last_event_tick = current_event_tick;

    UINT status;
    ButtonType btn_type = BTN_DIM_PRESS;
    status = tx_queue_send(&qu_input, &btn_type, TX_NO_WAIT);
    assert_param(status == TX_SUCCESS);
}

void HAL_DMA_TransferComplete_Callback(DMA_HandleTypeDef *hdma)
{
    if (hdma == (&hdma_uart5_rx))
    {
    }
    else if (hdma == (&hdma_uart4_tx))
    {
    }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
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
