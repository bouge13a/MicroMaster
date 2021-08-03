/*
 * board_pin_defs.hpp
 *
 *  Created on: Jul 27, 2021
 *      Author: steph
 */

#ifndef BOARD_PIN_DEFS_HPP_
#define BOARD_PIN_DEFS_HPP_

#include <stdint.h>

typedef struct {
    const char* name;
    uint32_t i2c_peripheral;
    uint32_t gpio_peripheral;
    uint32_t i2c_scl_pin;
    uint32_t i2c_data_pin;
    uint32_t base;
    uint32_t gpio_scl_pin;
    uint32_t gpio_data_pin;
    uint32_t gpio_base;
}i2c_config_t;

typedef struct  {
    const char* name;
    const uint32_t pin;
    const uint32_t peripheral;
    const uint32_t port;
    const uint32_t direction;
    const bool displayed;
} gpio_pin_t;

typedef struct {
    gpio_pin_t** gpis;
    uint32_t num_gpis;
}gpis_t;

typedef struct {
    gpio_pin_t** gpos;
    uint32_t num_gpos;
}gpos_t;

typedef struct {
    const char* name;
    const uint32_t gpio_peripheral;
    const uint32_t gpio_base;
    const uint32_t gpio_pin;
    const uint32_t sequencer;
    const uint32_t channel;
    const uint32_t step;
    const uint32_t adc_base;
}adc_pin_t;

typedef struct {
    adc_pin_t** adc_pins;
    uint32_t    num_adc_pins;
}adc_pins_t;


#endif /* BOARD_PIN_DEFS_HPP_ */
