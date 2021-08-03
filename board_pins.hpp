/*
 * board_pins.hpp
 *
 *  Created on: Jul 27, 2021
 *      Author: steph
 */

#ifndef BOARD_PINS_HPP_
#define BOARD_PINS_HPP_

#include "driverlib/inc/hw_i2c.h"
#include "driverlib/inc/hw_memmap.h"
#include "driverlib/inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

#include "board_pin_defs.hpp"

static i2c_config_t i2c0 = {
    .name            = "I2C1",
    .i2c_peripheral  = SYSCTL_PERIPH_I2C1,
    .gpio_peripheral = SYSCTL_PERIPH_GPIOA,
    .i2c_scl_pin     = GPIO_PA6_I2C1SCL,
    .i2c_data_pin    = GPIO_PA7_I2C1SDA,
    .base            = I2C1_BASE,
    .gpio_scl_pin    = GPIO_PIN_6,
    .gpio_data_pin   = GPIO_PIN_7,
    .gpio_base       = GPIO_PORTA_BASE,
};


#endif /* BOARD_PINS_HPP_ */
