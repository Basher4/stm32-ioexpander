STM32 IO Expander
=================

Designed for the cheapest blue pill clone with STM32F103C6 I could find on AliExpress.


# Features

- 10 input pins
  - Use internal pull up and pull down resistors
  - Optionally invert the output
- 10 output pins
  - Get state of output pins
- Interrupt pin when input changes
  - Set interrupt polarity
- Address selectable with IO pins

# Pinout

- I2C
  - SCL: PB8
  - SDA: PB9
- Output (12 total)
  - Expander: [A0 A1 A2 A3 A4 A5 A6 A7 B0 B1]
  - Interrupt: B11
  - LED: PC13
- Input (12 total)
  - Expander: [B12 B13 B14 B15 A9 A10 A11 A12 A15]
  - Address Selectors: C14 C15

# I2C

## Address

To change pull address pins C14 and C15 low. They are pulled high via an internal
pull-up resistor. Addresses are as follows:

  | C14 | C15 | I2C Address |
  | --- | --- | ----------- |
  |  1  |  1  |      19     |
  |  1  |  0  |      20     |
  |  0  |  1  |      21     |
  |  0  |  0  |      22     |

## Registers

All register values are little endian.

- Register `0x00` (16 bits / 2B) - GPIO
  - READ : Read value of input pins.
  - WRITE: Set value of output pins.
- Register `0x01` (32 bits / 4B)
  - READ : Always 0
  - WRITE: Set internal pull up / pull down resistor for input pins. Each input pin
           is described using 2 bits

    | Bit 2N | Bit 2N+1 | Description                                                     |
    | :----- | :------- | --------------------------------------------------------------- |
    | 0      | X        | Do not use any internal pull up or pull down resistors on pin N |
    | 1      | 0        | Use an internal pull down resistor on pin N                     |
    | 1      | 1        | Use an internal pull up resistor on pin N                       |

# Interrupt

Every time the values on input pins change, the interrupt pin is pulsed high.
During initialisation interrupt pin may fire randomly. If pins are not pulled
high or low via internal or external resistor, interrupts will probably fire
somewhat randomly too.

Currently LED will flash every time interrupt pin fires.

# TODO

- Sanity check pin assignments

# Future work

## Make I2C interface robust to incomplete messages

## Enable interrupt via I2C message

## Do most of the work in a loop and not I2C interrupt

## Make reading, writing, and configuring GPIO pins more efficient
Though it doesn't really matter. I2C runs at 400kHz at best, internal clock
is 72MHz or so. These inefficiencies are probably tiny.

## Input latch (was the pin pressed since the last read)
When an input pin is asserted (or deasserted when pull up is turned on), hold
the value high until input is read via I2C.

## Input pulse mode
While an input is asserted (or deasserted when pull up is turned on), only the
first I2C read will return 1.

## PWM support
STM32F103C6 supports 10 PWM pins. I want to be able to set the duty cycle
and perhaps the frequency too. This would require changing the IO map
as the pins now were selected for convenience and not PWM output pins (there are
12 PWM outputs in total).

MCU won't do any real work here, PWM can likely be just bitbanged and it will
work fine.

## Trade between input and output pins
Let the user decide how many input, output, and PWM pins they want.

## Store config in EEPROM.
When MCU gets reset no need to configure it again.
