#pragma once

#include <stdint.h>

typedef struct I2CRegister {
    void (*write)(uint8_t *data);
    uint8_t* (*read)();
    uint8_t address;
    uint8_t size;
} I2CRegister_t;
