#include "i2c/register.h"
#include "stm32f1xx.h"

I2CRegister_t _registers[I2C_REGISTER_COUNT] = {0};
uint8_t _validRegisters[(I2C_REGISTER_COUNT + 7) / 8] = {0};

void _setValidRegister(uint8_t idx) {
    _validRegisters[idx / 8] |= 1 << (idx % 8);
}

uint8_t _isRegisterValid(uint8_t idx) {
    return _validRegisters[idx / 8] & 1 << (idx % 8);
}

void I2CRegisterCreate(uint8_t address, uint8_t size, I2CRegisterFn read, I2CRegisterFn write)
{
    assert_param(address < I2C_REGISTER_COUNT);
    assert_param(size < I2C_REGISTER_MAX_LENGTH);

    _registers[address].write = write;
    _registers[address].read = read;
    _registers[address].size = size;
    _registers[address].address = address;

    _setValidRegister(address);
}

I2CRegister_t *I2CRegisterFind(uint8_t address)
{
    if (address > I2C_REGISTER_COUNT || !_isRegisterValid(address)) {
        return NULL;
    }

    return &_registers[address];
}
