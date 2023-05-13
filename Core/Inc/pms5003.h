#pragma once

#include <stdint.h>
#include "stm32wlxx_hal.h"

struct PMS5003Data
{
    uint16_t pm1_0_cf;
    uint16_t pm2_5_cf;
    uint16_t pm_10_cf;
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

void PMS5003_Init(UART_HandleTypeDef *uart);
int32_t PMS5003_Read(struct PMS5003Data *data);
