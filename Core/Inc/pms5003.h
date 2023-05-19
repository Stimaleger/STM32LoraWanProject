#pragma once

#include <stdint.h>
#include "stm32wlxx_hal.h"

struct PMS5003Data
{
    uint16_t pm1_0_atm;
    uint16_t pm2_5_atm;
    uint16_t pm_10_atm;
    uint16_t nbParticles0_3um;
    uint16_t nbParticles0_5um;
    uint16_t nbParticles1_0um;
    uint16_t nbParticles2_5um;
    uint16_t nbParticles5_0um;
    uint16_t nbParticles10um;
} __attribute__((packed));

typedef void (*callbackDataReady)(struct PMS5003Data *data);

void PMS5003_Init(UART_HandleTypeDef *uart);
void PMS5003_StartReceiving(callbackDataReady cb);
void PMS5003_StopReceiving(void);
