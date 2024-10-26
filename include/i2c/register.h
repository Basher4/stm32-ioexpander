#pragma once

#include <stdint.h>

#define I2C_REGISTER_COUNT      2   // The number of I2C registers allowed.
#define I2C_REGISTER_MAX_LENGTH 8   // Max size of an I2C register in bytes.

typedef void (*I2CRegisterFn)(uint8_t*);

typedef struct I2CRegister {
    I2CRegisterFn write;
    I2CRegisterFn read;
    uint8_t address;
    uint8_t size;
    uint8_t pad[2];
} I2CRegister_t;

/// Create a new register. If a register exists at the given address it will be overwritten.
/// Registers are stored in a pre-allocated array so even unused registers consume memory.
void I2CRegisterCreate(uint8_t address, uint8_t size, I2CRegisterFn read, I2CRegisterFn write);

///
I2CRegister_t *I2CRegisterFind(uint8_t address);
