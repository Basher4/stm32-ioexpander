#pragma once

#include "stm32f1xx_hal.h"

void ExpanderGpioWrite(uint16_t value);
void ExpanderGpioRead(uint16_t* value);

typedef enum { NO_PULL = 0, PULL_DOWN = 2, PULL_UP = 3 } GpioPull;
void ExpanderGpioSetPullResistor(GPIO_TypeDef* port, uint16_t pin, GpioPull pull);
