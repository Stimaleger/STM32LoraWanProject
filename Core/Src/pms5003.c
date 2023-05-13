#include "stdbool.h"
#include "stddef.h"
#include "string.h"
#include "pms5003.h"
#include "main.h"
#include "utilities_def.h"
#include "stm32_seq.h"
#include "stm32wlxx_hal_uart.h"

#define START_CHAR_1 (0x42)
#define START_CHAR_2 (0x4D)
#define PMS5003_DEFAULT_SIZE (32)

static void cbUartRxCmplt(UART_HandleTypeDef *huart);
static bool isDataValid(uint8_t *data, size_t size);
static void cbUartError(UART_HandleTypeDef *huart);
static uint32_t calculateChecksum(uint8_t *data, size_t size);

static bool isDataReady = false;
static UART_HandleTypeDef *uart = NULL;

void PMS5003_Init(UART_HandleTypeDef *pUart)
{
    uart = pUart;
    UART_InitCallbacksToDefault(uart);
    HAL_GPIO_WritePin(PMS5003_SET_GPIO_Port, PMS5003_SET_Pin, GPIO_PIN_RESET);
    HAL_UART_RegisterCallback(uart, HAL_UART_RX_COMPLETE_CB_ID, cbUartRxCmplt);
    HAL_UART_RegisterCallback(uart, HAL_UART_ERROR_CB_ID, cbUartError);
}

int32_t PMS5003_Read(struct PMS5003Data *data)
{
    int32_t ret = -1;
    uint8_t tmpBuffer[PMS5003_DEFAULT_SIZE] = {0};
    uint32_t timeout = 500;

    isDataReady = false;
    HAL_GPIO_WritePin(PMS5003_SET_GPIO_Port, PMS5003_SET_Pin, GPIO_PIN_SET);
    HAL_UART_Receive_IT(uart, tmpBuffer, PMS5003_DEFAULT_SIZE);
    while (!isDataReady && timeout != 0)
    {
        timeout--;
        HAL_Delay(100);
    }
    if (isDataReady)
    {
        if (isDataValid(tmpBuffer, PMS5003_DEFAULT_SIZE))
        {
            data->pm1_0_cf = (tmpBuffer[4] << 8) + tmpBuffer[5];
            data->pm2_5_cf = (tmpBuffer[6] << 8) + tmpBuffer[7];
            data->pm_10_cf = (tmpBuffer[8] << 8) + tmpBuffer[9];
            data->pm1_0_atm = (tmpBuffer[10] << 8) + tmpBuffer[11];
            data->pm2_5_atm = (tmpBuffer[12] << 8) + tmpBuffer[13];
            data->pm_10_atm = (tmpBuffer[14] << 8) + tmpBuffer[15];

            data->nbParticles0_3um = (tmpBuffer[16] << 8) + tmpBuffer[17];
            data->nbParticles0_5um = (tmpBuffer[18] << 8) + tmpBuffer[19];
            data->nbParticles1_0um = (tmpBuffer[20] << 8) + tmpBuffer[21];
            data->nbParticles2_5um = (tmpBuffer[21] << 8) + tmpBuffer[23];
            data->nbParticles5_0um = (tmpBuffer[22] << 8) + tmpBuffer[25];
            data->nbParticles10um = (tmpBuffer[24] << 8) + tmpBuffer[27];
            ret = 0;
        }
    }
    else
    {
        __NOP();
    }
    HAL_GPIO_WritePin(PMS5003_SET_GPIO_Port, PMS5003_SET_Pin, GPIO_PIN_RESET);
    return ret;
}



static void cbUartRxCmplt(UART_HandleTypeDef *huart)
{
    isDataReady = true;
}

static void cbUartError(UART_HandleTypeDef *huart)
{
    __NOP();
}

static uint32_t calculateChecksum(uint8_t *data, size_t size)
{
    int i;
    uint32_t calculateChecksum = 0;

    for (i = 0 ; i < size ; i++)
    {
        calculateChecksum += data[i];
    }

    return calculateChecksum;
}

static bool isDataValid(uint8_t *data, size_t size)
{
    bool b_ret = false;
    int16_t checkCode, calculatedCheckCode = 0;

    // Make sure size match
    if (size == PMS5003_DEFAULT_SIZE)
    {
        // Check starting characters
        if ((data[0] == START_CHAR_1) && (data[1] == START_CHAR_2))
        {
            checkCode = (data[30] << 8) + data[31];

            calculatedCheckCode = calculateChecksum(data, 30);
            if (calculatedCheckCode == checkCode)
            {
                b_ret = true;
            }
            else
            {
                __NOP();
            }
        }
    }
    return b_ret;
}
