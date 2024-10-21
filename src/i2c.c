#include "i2c.h"
#include "expander/gpio.h"
#include "expander/pins.h"
#include "i2c/register.h"
#include "i2c/state_machine.h"

I2C_HandleTypeDef hi2c1;

/* I2C1 init function */
void MX_I2C1_Init(void)
{
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 400000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 38;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
        Error_Handler();
    }
}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    if (i2cHandle->Instance == I2C1) {
        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**I2C1 GPIO Configuration
        PB8     ------> I2C1_SCL
        PB9     ------> I2C1_SDA
        */
        GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        __HAL_AFIO_REMAP_I2C1_ENABLE();

        /* I2C1 clock enable */
        __HAL_RCC_I2C1_CLK_ENABLE();

        /* I2C1 interrupt Init */
        HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
        HAL_NVIC_SetPriority(I2C1_ER_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
    }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{
    if (i2cHandle->Instance == I2C1) {
        /* USER CODE BEGIN I2C1_MspDeInit 0 */

        /* USER CODE END I2C1_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_I2C1_CLK_DISABLE();

        /**I2C1 GPIO Configuration
        PB8     ------> I2C1_SCL
        PB9     ------> I2C1_SDA
        */
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8);

        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_9);

        /* I2C1 interrupt Deinit */
        HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);
        HAL_NVIC_DisableIRQ(I2C1_ER_IRQn);
        /* USER CODE BEGIN I2C1_MspDeInit 1 */

        /* USER CODE END I2C1_MspDeInit 1 */
    }
}

enum I2CState i2c_state = STATE_INIT;

uint8_t reg = 0;
uint8_t data[8];

void HAL_I2C_AddrCallback(I2C_HandleTypeDef* hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode)
{
    if (TransferDirection == I2C_DIRECTION_TRANSMIT) { // Master wants to transmit
        if (i2c_state == STATE_INIT) {
            HAL_I2C_Slave_Seq_Receive_IT(hi2c, &reg, 1, I2C_NEXT_FRAME);
            i2c_state = STATE_ACQUIRED_REGISTER;
        }
    } else {
        if (i2c_state == STATE_RX_DATA) {
            if (reg == 0) {
                ExpanderGpioRead((uint16_t*)data);
                HAL_I2C_Slave_Seq_Transmit_IT(hi2c, data, 2, I2C_FIRST_AND_LAST_FRAME);
                i2c_state = STATE_TX_DATA;
            }
        }
    }
}

void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef* hi2c)
{
    i2c_state = STATE_INIT;
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef* hi2c)
{
    switch (i2c_state) {
    case STATE_ACQUIRED_REGISTER:
        if (reg == 0) {
            HAL_I2C_Slave_Seq_Receive_IT(hi2c, data, 2, I2C_LAST_FRAME);
        } else if (reg == 1) {
            HAL_I2C_Slave_Seq_Receive_IT(hi2c, data, 4, I2C_LAST_FRAME);
        } else {
            goto err;
        }

        i2c_state = STATE_RX_DATA;
        return;
    case STATE_RX_DATA:
        if (reg == 0) {
            ExpanderGpioWrite(*(uint16_t*)data);    
        } else if (reg == 1) {
            for (int i = 0; i < EXPANDER_IN_COUNT; i++) {
                GPIO_TypeDef *port = EXPANDER_IN_PORTS[i];
                uint16_t pin = EXPANDER_IN_PINS[i];
                uint8_t value = (data[i / 4] >> ((i % 4) * 2)) & 0b11;
                ExpanderGpioSetPullResistor(port, pin, (GpioPull)value);
            }
        } else {
            goto err;
        }

        i2c_state = STATE_INIT;
        return;
    default:
        goto err;
    }

err:
    i2c_state = STATE_INIT;
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef* hi2c)
{
    HAL_I2C_DeInit(hi2c);
    HAL_I2C_Init(hi2c);
    HAL_I2C_EnableListen_IT(hi2c);
}

void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef* hi2c) { HAL_I2C_EnableListen_IT(hi2c); }

void HAL_I2C_AbortCpltCallback(I2C_HandleTypeDef* hi2c) { HAL_I2C_EnableListen_IT(hi2c); }
