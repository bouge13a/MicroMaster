/*
 * GPOs.cpp
 *
 *  Created on: Aug 3, 2021
 *      Author: steph
 */

#include <string.h>
#include <assert.h>
#include "GPOs.hpp"
#include "pins_GPO.hpp"

#include "driverlib/inc/hw_gpio.h"
#include "driverlib/inc/hw_types.h"
#include "driverlib/inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"


GpoObj::GpoObj(void) {

    this->gpo_info = &board_gpo_info;

    if (0 != this->gpo_info->num_gpos) {

        for(uint32_t idx=0; idx<this->gpo_info->num_gpos; idx++) {

            MAP_SysCtlPeripheralEnable(this->gpo_info->gpos[idx]->peripheral);

            SysCtlGPIOAHBEnable(this->gpo_info->gpos[idx]->peripheral);

            // Unlock port so we can change it to a GPIO input
            // Once we have enabled (unlocked) the commit register then re-lock it
            // to prevent further changes.  PF0 is muxed with NMI thus a special case.
            HWREG(this->gpo_info->gpos[idx]->port + GPIO_O_LOCK) = GPIO_LOCK_KEY;
            HWREG(this->gpo_info->gpos[idx]->port + GPIO_O_CR) |= gpo_info->gpos[idx]->pin;
            HWREG(this->gpo_info->gpos[idx]->port + GPIO_O_LOCK) = 0;

            MAP_GPIODirModeSet(this->gpo_info->gpos[idx]->port,
                               this->gpo_info->gpos[idx]->pin,
                               this->gpo_info->gpos[idx]->direction);

            MAP_GPIOPadConfigSet(this->gpo_info->gpos[idx]->port,
                                 this->gpo_info->gpos[idx]->pin,
                                 GPIO_STRENGTH_12MA,
                                 GPIO_PIN_TYPE_STD);
        }

    }

} // End GpoObj::GpoObj

gpio_pin_t* GpoObj::get_config(const char* name) {

    int idx;

    for(idx=0; idx<this->gpo_info->num_gpos; idx++) {

        if(0 == strcmp(name, this->gpo_info->gpos[idx]->name)){
            return this->gpo_info->gpos[idx];
        }

    }

    assert(0);
    return 0;

} // End GpoObj::get_config

void GpoObj::set(gpio_pin_t* config, uint32_t value) {

    if(value > 0) {
        MAP_GPIOPinWrite(config->port,
                         config->pin,
                         config->pin);
    } else {
        MAP_GPIOPinWrite(config->port,
                         config->pin,
                         0);
    }

} // End GpoObj::set

uint32_t GpoObj::get(gpio_pin_t* config) {

    if (0 == MAP_GPIOPinRead(config->port, config->pin)) {
        return 0;
    } else {
        return 1;
    }

} // End GpoObj::get

gpos_t* GpoObj::get_struct(void) {
    return this->gpo_info;
} // End GpoObj::get_struct
