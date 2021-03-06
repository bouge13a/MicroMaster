/*
 * pins_PWM.hpp
 *
 *  Created on: Aug 5, 2021
 *      Author: steph
 */

#ifndef PINS_PWM_HPP_
#define PINS_PWM_HPP_

#include "board_pin_defs.hpp"

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "uartstdio.h"

static pwm_pin_t pwm_0 = {
    .name            =  "PWM 0",
    .gpio_peripheral = SYSCTL_PERIPH_GPIOB,
    .gpio_base       = GPIO_PORTB_BASE,
    .gpio_pin        = GPIO_PIN_6,
    .pwm_peripheral  = SYSCTL_PERIPH_PWM0,
    .pwm_base        = PWM0_BASE,
    .pwm_pin         = GPIO_PB6_M0PWM0,
    .pwm_generator   = PWM_GEN_0,
    .pwm_out         = PWM_OUT_0,
    .pwm_out_bit     = PWM_OUT_0_BIT,
};


static pwm_pin_t* pwm_pins[] = {
    &pwm_0,
};

pwm_pins_t board_pwm_info = {
    .pwm_pins     = pwm_pins,
    .num_pwm_pins = sizeof(pwm_pins)/sizeof(pwm_pins[0]),
};


#endif /* PINS_PWM_HPP_ */
