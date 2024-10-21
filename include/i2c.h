#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern I2C_HandleTypeDef hi2c1;

#define I2C_BASE_ADDRESS    0x13

void MX_I2C1_Init(uint8_t addr_mod);

#ifdef __cplusplus
}
#endif
