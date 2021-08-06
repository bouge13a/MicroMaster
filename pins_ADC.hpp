/*
 * ADC_pins.hpp
 *
 *  Created on: Aug 3, 2021
 *      Author: steph
 */

#ifndef PINS_ADC_HPP_
#define PINS_ADC_HPP_


#include <stdbool.h>
#include <stdint.h>
#include "driverlib/inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

#include "board_pin_defs.hpp"

static adc_pin_t adc1 = {
    .name            = "ADC 1",
    .gpio_peripheral = SYSCTL_PERIPH_GPIOE,
    .gpio_base       = GPIO_PORTE_AHB_BASE,
    .gpio_pin        = GPIO_PIN_3,
    .sequencer       = 0,
    .channel         = 0,
    .step            = 0,
    .adc_base        = ADC0_BASE,
};

static adc_pin_t adc2 = {
    .name            = "ADC 2",
    .gpio_peripheral = SYSCTL_PERIPH_GPIOE,
    .gpio_base       = GPIO_PORTE_AHB_BASE,
    .gpio_pin        = GPIO_PIN_2,
    .sequencer       = 0,
    .channel         = 1,
    .step            = 1,
    .adc_base        = ADC0_BASE,
};

static adc_pin_t adc3 = {
    .name            = "ADC 3",
    .gpio_peripheral = SYSCTL_PERIPH_GPIOE,
    .gpio_base       = GPIO_PORTE_AHB_BASE,
    .gpio_pin        = GPIO_PIN_1,
    .sequencer       = 0,
    .channel         = 2,
    .step            = 2,
    .adc_base        = ADC0_BASE,
};

static adc_pin_t adc4 = {
    .name            = "ADC 4",
    .gpio_peripheral = SYSCTL_PERIPH_GPIOE,
    .gpio_base       = GPIO_PORTE_AHB_BASE,
    .gpio_pin        = GPIO_PIN_0,
    .sequencer       = 0,
    .channel         = 3,
    .step            = 3,
    .adc_base        = ADC0_BASE,
};

static adc_pin_t adc5 = {
    .name            = "ADC 5",
    .gpio_peripheral = SYSCTL_PERIPH_GPIOD,
    .gpio_base       = GPIO_PORTD_AHB_BASE,
    .gpio_pin        = GPIO_PIN_3,
    .sequencer       = 0,
    .channel         = 4,
    .step            = 4,
    .adc_base        = ADC0_BASE,
};

static adc_pin_t* board_adc_pins[] = {
    &adc1,
    &adc2,
//    &adc3,
//    &adc4,
//    &adc5,

};

adc_pins_t adc_pin_struct = {
    .adc_pins     = board_adc_pins,
    .num_adc_pins = sizeof(board_adc_pins)/sizeof(board_adc_pins[0]),
};



#endif /* PINS_ADC_HPP_ */
