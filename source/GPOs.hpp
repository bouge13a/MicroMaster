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
#include "display_task.hpp"
#include "OLED_GFX.h"

void set_power_supplies(uint32_t index);
void sel_power_supply(uint32_t index);
void set_pullup_en(uint32_t index);

typedef enum {
    PSU_OFF,
    PSU_ON,
}ps_state_e;

class GpoObj : public DisplayUpdate {
public:
    GpoObj(uint32_t power_idx, OLED_GFX* oled_gfx);
    gpio_pin_t* get_config(const char* name);
    void set(gpio_pin_t* config, uint32_t value);
    uint32_t get(gpio_pin_t* config);
    gpos_t* get_struct(void);
    void set_supply_display(ps_state_e state);
    void set_pull_display(ps_state_e state);
private:
    gpos_t* gpo_info;
    void update_display(void);
    ps_state_e ps_state;
    ps_state_e pu_state;
    OLED_GFX* oled_gfx;
};

#endif /* GPOS_HPP_ */
