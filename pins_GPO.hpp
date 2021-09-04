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


static gpio_pin_t gpo_0 = {
    .name       = "GPO 0",
    .pin        = GPIO_PIN_3,
    .peripheral = SYSCTL_PERIPH_GPIOF,
    .port       = GPIO_PORTF_AHB_BASE,
    .direction  = GPIO_DIR_MODE_OUT,
    .pad_config = GPIO_PIN_TYPE_STD,
    .displayed  = true,
};

static gpio_pin_t gpo_1 = {
    .name       = "GPO 1",
    .pin        = GPIO_PIN_1,
    .peripheral = SYSCTL_PERIPH_GPIOF,
    .port       = GPIO_PORTF_AHB_BASE,
    .direction  = GPIO_DIR_MODE_OUT,
    .pad_config = GPIO_PIN_TYPE_STD,
    .displayed  = true,
};

static gpio_pin_t gpo_2 = {
    .name       = "GPO 2",
    .pin        = GPIO_PIN_2,
    .peripheral = SYSCTL_PERIPH_GPIOF,
    .port       = GPIO_PORTF_AHB_BASE,
    .direction  = GPIO_DIR_MODE_OUT,
    .pad_config = GPIO_PIN_TYPE_STD,
    .displayed  = true,
};

static gpio_pin_t gpo_3 = {
    .name       = "GPO 3",
    .pin        = GPIO_PIN_0,
    .peripheral = SYSCTL_PERIPH_GPIOF,
    .port       = GPIO_PORTF_AHB_BASE,
    .direction  = GPIO_DIR_MODE_OUT,
    .pad_config = GPIO_PIN_TYPE_STD,
    .displayed  = true,
};

static gpio_pin_t one_wire = {
    .name       = "one wire",
    .pin        = GPIO_PIN_4,
    .peripheral = SYSCTL_PERIPH_GPIOE,
    .port       = GPIO_PORTE_AHB_BASE,
    .direction  = GPIO_DIR_MODE_OUT,
    .pad_config = GPIO_PIN_TYPE_OD,
    .displayed  = false,
};

static gpio_pin_t pwr_on = {
    .name       = "PWR ON",
    .pin        = GPIO_PIN_3,
    .peripheral = SYSCTL_PERIPH_GPIOD,
    .port       = GPIO_PORTD_BASE,
    .direction  = GPIO_DIR_MODE_OUT,
    .pad_config = GPIO_PIN_TYPE_STD,
    .displayed  = false,
};

static gpio_pin_t psel_1v8 = {
    .name       = "PSEL 1V8",
    .pin        = GPIO_PIN_2,
    .peripheral = SYSCTL_PERIPH_GPIOD,
    .port       = GPIO_PORTD_BASE,
    .direction  = GPIO_DIR_MODE_OUT,
    .pad_config = GPIO_PIN_TYPE_STD,
    .displayed  = false,
};

static gpio_pin_t psel_3v3 = {
    .name       = "PSEL 3V3",
    .pin        = GPIO_PIN_6,
    .peripheral = SYSCTL_PERIPH_GPIOD,
    .port       = GPIO_PORTD_BASE,
    .direction  = GPIO_DIR_MODE_OUT,
    .pad_config = GPIO_PIN_TYPE_STD,
    .displayed  = false,
};

static gpio_pin_t psel_5v = {
    .name       = "PSEL 5V",
    .pin        = GPIO_PIN_5,
    .peripheral = SYSCTL_PERIPH_GPIOE,
    .port       = GPIO_PORTE_AHB_BASE,
    .direction  = GPIO_DIR_MODE_OUT,
    .pad_config = GPIO_PIN_TYPE_STD,
    .displayed  = false,
};

static gpio_pin_t pullup_en = {
    .name       = "Pullup En",
    .pin        = GPIO_PIN_4,
    .peripheral = SYSCTL_PERIPH_GPIOE,
    .port       = GPIO_PORTE_AHB_BASE,
    .direction  = GPIO_DIR_MODE_OUT,
    .pad_config = GPIO_PIN_TYPE_STD,
    .displayed  = false,
};

static gpio_pin_t* gpos[] = {
    &gpo_0,
    &gpo_1,
    &gpo_2,
    &gpo_3,
    &pwr_on,
    &psel_1v8,
    &psel_3v3,
    &psel_5v,
    &pullup_en,
    &one_wire,
};

gpos_t board_gpo_info = {
    .gpos     = gpos,
    .num_gpos = sizeof(gpos)/sizeof(gpos[0]),
};


#endif /* PINS_GPO_HPP_ */
