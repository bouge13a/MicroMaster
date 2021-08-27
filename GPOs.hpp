/*
 * GPOs.hpp
 *
 *  Created on: Aug 3, 2021
 *      Author: steph
 */

#ifndef GPOS_HPP_
#define GPOS_HPP_

#include <stdint.h>
#include "board_pin_defs.hpp"

void set_power_supplies(uint32_t index);
void sel_power_supply(uint32_t index);
void set_pullup_en(uint32_t index);

class GpoObj {
public:
    GpoObj(void);
    gpio_pin_t* get_config(const char* name);
    void set(gpio_pin_t* config, uint32_t value);
    uint32_t get(gpio_pin_t* config);
    gpos_t* get_struct(void);

private:
    gpos_t* gpo_info;
};

#endif /* GPOS_HPP_ */
