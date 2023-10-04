#include "spacelight.h"

#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_gpio.h>
#include <stm32f4xx_hal_tim.h>
#include <stm32f4xx_hal_dma.h>

extern TIM_HandleTypeDef htim2;

extern UART_HandleTypeDef huart4;
extern DMA_HandleTypeDef hdma_uart4_tx;

extern UART_HandleTypeDef huart5;
extern DMA_HandleTypeDef hdma_uart5_rx;

void HAL_DMA_TransferComplete_Callback(DMA_HandleTypeDef *hdma);

GPIO_PinState last_sw2 = GPIO_PIN_SET;
ULONG last_sw2_tick = 0;

/* GPIO */
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

/* TIMER */
void hal_tmr_init()
{
    /* enable TIM2 CH1 as SW1 */
    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
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

/* UART */
void hal_uart_dma_init()
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

inline void hal_uart_dma_send(uint8_t *tx_buffer, uint16_t tx_len)
{
    HAL_UART_Transmit_DMA(&huart4, tx_buffer, tx_len);
}

void HAL_DMA_TransferComplete_Callback(DMA_HandleTypeDef *hdma)
{
    if (hdma == (&hdma_uart5_rx))
    {
    }

    // doesn't care about tx dma cb since we always assumpt the tx is success
    // if (hdma == (&hdma_uart4_tx))
    //{
    //}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
}
