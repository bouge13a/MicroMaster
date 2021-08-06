/*
 * GPI_pins.hpp
 *
 *  Created on: Aug 3, 2021
 *      Author: steph
 */

#ifndef PINS_GPI_HPP_
#define PINS_GPI_HPP_


#include "board_pin_defs.hpp"

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"


static gpio_pin_t gpi_1 = {
    .name       = "GPI 1",
    .pin        = GPIO_PIN_4,
    .peripheral = SYSCTL_PERIPH_GPIOF,
    .port       = GPIO_PORTF_AHB_BASE,
    .direction  = GPIO_DIR_MODE_IN,
    .displayed  = true,
};

static gpio_pin_t gpi_2 = {
    .name       = "GPI 2",
    .pin        = GPIO_PIN_7,
    .peripheral = SYSCTL_PERIPH_GPIOD,
    .port       = GPIO_PORTD_AHB_BASE,
    .direction  = GPIO_DIR_MODE_IN,
    .displayed  = true,
};

static gpio_pin_t gpi_3 = {
    .name       = "GPI 3",
    .pin        = GPIO_PIN_6,
    .peripheral = SYSCTL_PERIPH_GPIOC,
    .port       = GPIO_PORTC_AHB_BASE,
    .direction  = GPIO_DIR_MODE_IN,
    .displayed  = true,
};

static gpio_pin_t gpi_4 = {
    .name       = "GPI 4",
    .pin        = GPIO_PIN_7,
    .peripheral = SYSCTL_PERIPH_GPIOC,
    .port       = GPIO_PORTC_AHB_BASE,
    .direction  = GPIO_DIR_MODE_IN,
    .displayed  = true,
};

static gpio_pin_t gpi_5 = {
    .name       = "GPI 5",
    .pin        = GPIO_PIN_6,
    .peripheral = SYSCTL_PERIPH_GPIOD,
    .port       = GPIO_PORTD_AHB_BASE,
    .direction  = GPIO_DIR_MODE_IN,
    .displayed  = true,
};


static gpio_pin_t* board_gpi_pins[] = {
    &gpi_1,
    &gpi_2,
//    &gpi_3,
//    &gpi_4,
//    &gpi_5,
};

gpis_t board_gpi_info = {
    .gpis     = board_gpi_pins,
    .num_gpis = sizeof(board_gpi_pins)/sizeof(board_gpi_pins[0])
};



#endif /* PINS_GPI_HPP_ */
