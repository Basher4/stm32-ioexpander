#include "expander/gpio.h"
#include "expander/pins.h"

#include "gpio.h"
#include "stm32f1xx_hal.h"

uint16_t EXPANDER_IN_PINS[] = {
    IN0_Pin, IN1_Pin, IN2_Pin, IN3_Pin, IN4_Pin,
    IN5_Pin, IN6_Pin, IN7_Pin, IN8_Pin, IN9_Pin
};

GPIO_TypeDef* EXPANDER_IN_PORTS[] = {
    IN0_GPIO_Port, IN1_GPIO_Port, IN2_GPIO_Port, IN3_GPIO_Port, IN4_GPIO_Port,
    IN5_GPIO_Port, IN6_GPIO_Port, IN7_GPIO_Port, IN8_GPIO_Port, IN9_GPIO_Port 
};

void ExpanderGpioWrite(uint16_t value)
{
    uint32_t cmd;

    // OUT0 to OUT7 live on GPIOA
    cmd = value & 0xFF;
    GPIOA->BSRR = cmd; // Set GPIO
    GPIOA->BSRR = (~cmd & 0xFF) << 16u; // Reset GPIO

    // OUT8 and OUT9 live on GPIOB
    cmd = (value >> 8) & 0x3;
    GPIOB->BSRR = cmd; // Set GPIO
    GPIOB->BSRR = (~cmd & 0x3) << 16u; // Reset GPIO
}

void ExpanderGpioRead(uint16_t* value)
{
    uint16_t data = 0;
    for (int i = 0; i < EXPANDER_IN_COUNT; i++) {
        data |= (HAL_GPIO_ReadPin(EXPANDER_IN_PORTS[i], EXPANDER_IN_PINS[i]) << i);
    }
    *value = data;
}

void ExpanderGpioSetPullResistor(GPIO_TypeDef* port, uint16_t pin, GpioPull pull)
{
    GPIO_InitTypeDef gpio_init = { 0 };
    gpio_init.Pin = pin;
    gpio_init.Mode = GPIO_MODE_INPUT;

    if (pull == PULL_DOWN) {
        gpio_init.Pull = GPIO_PULLDOWN;
    } else if (pull == PULL_UP) {
        gpio_init.Pull = GPIO_PULLUP;
    } else { // Nothing needs to happen for no pull
        gpio_init.Pull = GPIO_NOPULL;
    }

    HAL_GPIO_Init(port, &gpio_init);
}
