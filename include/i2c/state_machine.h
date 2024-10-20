#pragma once

#include <stdint.h>

enum I2CState { STATE_INIT, STATE_ACQUIRED_REGISTER, STATE_RX_DATA, STATE_TX_DATA };
