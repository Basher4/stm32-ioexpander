#include "expander/gpio.h"

#include "stm32f1xx_hal.h"

void ExpanderGpioWrite(uint16_t value)
{
    uint32_t cmd;

    // OUT0 to OUT7 live on GPIOA
    cmd = value & 0xFF;
    GPIOA->BSRR = cmd;                  // Set GPIO
    GPIOA->BSRR = (~cmd & 0xFF) << 16u; // Reset GPIO

    // OUT8 and OUT9 live on GPIOB
    cmd = (value >> 8) & 0x3;
    GPIOB->BSRR = cmd;                  // Set GPIO
    GPIOB->BSRR = (~cmd & 0x3) << 16u;  // Reset GPIO
}

void ExpanderGpioRead(uint16_t *value)
{
    *value |= GPIOA->IDR & 0xFF;
    *value |= (GPIOB->IDR & 0x3) << 8u;
}
