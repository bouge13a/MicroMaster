/*
 * pins_GPO.hpp
 *
 *  Created on: Aug 3, 2021
 *      Author: steph
 */

#ifndef PINS_GPO_HPP_
#define PINS_GPO_HPP_


#include "board_pin_defs.hpp"

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"


//static gpio_pin_t red_led = {
//    .name       = "red debug",
//    .pin        = GPIO_PIN_0,
//    .peripheral = SYSCTL_PERIPH_GPIOF,
//    .port       = GPIO_PORTF_AHB_BASE,
//    .direction  = GPIO_DIR_MODE_OUT,
//    .displayed  = true,
//};

static gpio_pin_t red_led = {
    .name       = "red debug",
    .pin        = GPIO_PIN_1,
    .peripheral = SYSCTL_PERIPH_GPIOF,
    .port       = GPIO_PORTF_AHB_BASE,
    .direction  = GPIO_DIR_MODE_OUT,
    .displayed  = true,
};

static gpio_pin_t blue_led = {
    .name       = "blue debug",
    .pin        = GPIO_PIN_2,
    .peripheral = SYSCTL_PERIPH_GPIOF,
    .port       = GPIO_PORTF_AHB_BASE,
    .direction  = GPIO_DIR_MODE_OUT,
    .displayed  = true,
};

static gpio_pin_t green_led = {
    .name       = "green debug",
    .pin        = GPIO_PIN_3,
    .peripheral = SYSCTL_PERIPH_GPIOF,
    .port       = GPIO_PORTF_AHB_BASE,
    .direction  = GPIO_DIR_MODE_OUT,
    .displayed  = true,
};


static gpio_pin_t* gpos[] = {
    &green_led,
    &red_led,
    &blue_led,
};

gpos_t board_gpo_info = {
    .gpos     = gpos,
    .num_gpos = sizeof(gpos)/sizeof(gpos[0]),
};


#endif /* PINS_GPO_HPP_ */
