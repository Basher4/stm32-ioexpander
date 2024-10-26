#include "expander.h"

#include "expander/gpio.h"
#include "expander/pins.h"
#include "i2c.h"
#include "i2c/register.h"

void reg0_set_gpio(uint8_t *data);
void reg0_get_gpio(uint8_t *data);
void reg1_set_pull_resistors(uint8_t *data);

void Expander_Init()
{
    // Init I2C Registers
    I2CRegisterCreate(0, 2, &reg0_get_gpio, &reg0_set_gpio);
    I2CRegisterCreate(1, 4, NULL, &reg1_set_pull_resistors);

    // Init I2C Peripheral
    uint8_t i2c_addr_mod = 0;
    if (!HAL_GPIO_ReadPin(I2C_ADDR_0_GPIO_Port, I2C_ADDR_0_Pin)) {
        i2c_addr_mod += 1;
    }
    if (!HAL_GPIO_ReadPin(I2C_ADDR_1_GPIO_Port, I2C_ADDR_1_Pin)) {
        i2c_addr_mod += 2;
    }
    MX_I2C1_Init(i2c_addr_mod);
    HAL_I2C_EnableListen_IT(&hi2c1);

    // Init interrupt pin.
    HAL_GPIO_WritePin(OUT_LED_GPIO_Port, OUT_LED_Pin, GPIO_PIN_SET);
}

void reg0_set_gpio(uint8_t *data)
{
    ExpanderGpioWrite(*(uint16_t*)data);
}

void reg0_get_gpio(uint8_t *data)
{
    ExpanderGpioRead((uint16_t*)data);
}

void reg1_set_pull_resistors(uint8_t *data)
{
    for (int i = 0; i < EXPANDER_IN_COUNT; i++) {
        GPIO_TypeDef *port = EXPANDER_IN_PORTS[i];
        uint16_t pin = EXPANDER_IN_PINS[i];
        uint8_t value = (data[i / 4] >> ((i % 4) * 2)) & 0b11;
        ExpanderGpioSetPullResistor(port, pin, (GpioPull)value);
    }
}
