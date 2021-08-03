/*
 * board_pin_defs.hpp
 *
 *  Created on: Jul 27, 2021
 *      Author: steph
 */

#ifndef BOARD_PIN_DEFS_HPP_
#define BOARD_PIN_DEFS_HPP_


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


#endif /* BOARD_PIN_DEFS_HPP_ */
