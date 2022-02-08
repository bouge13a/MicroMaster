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


GpoObj* gpo_obj = NULL;

gpio_pin_t* power_on_pin = NULL;
gpio_pin_t* psel_1v8_pin = NULL;
gpio_pin_t* psel_3v3_pin = NULL;
gpio_pin_t* psel_5v_pin = NULL;
gpio_pin_t* pullup_en_pin = NULL;

void set_power_supplies(uint32_t index) {

    switch(index) {
    case 0 :
        gpo_obj->set(power_on_pin, 0);
        break;
    case 1 :
        gpo_obj->set(power_on_pin, 1);
        break;
    default :
        assert(0);
        break;
    }

} // End set_power_supplies

void sel_power_supply(uint32_t index) {

    switch(index) {
    case 0 :
        gpo_obj->set(psel_1v8_pin, 0);
        gpo_obj->set(psel_3v3_pin, 1);
        gpo_obj->set(psel_5v_pin, 0);
        break;
    case 1:
        gpo_obj->set(psel_1v8_pin, 0);
        gpo_obj->set(psel_3v3_pin, 0);
        gpo_obj->set(psel_5v_pin, 1);
        break;
    case 2:
        gpo_obj->set(psel_1v8_pin, 1);
        gpo_obj->set(psel_3v3_pin, 0);
        gpo_obj->set(psel_5v_pin, 0);
        break;
    default :
        assert(0);
        break;
    }

} // End sel_power_supply

void set_pullup_en(uint32_t index) {

    switch(index) {
    case 0 :
        gpo_obj->set(pullup_en_pin, 1);
        break;
    case 1 :
        gpo_obj->set(pullup_en_pin, 0);
        break;
    default :
        assert(0);
        break;
    }

} // End set_pullup_en

GpoObj::GpoObj(uint32_t power_idx) {

    this->gpo_info = &board_gpo_info;

    gpo_obj = this;

    power_on_pin = this->get_config("PWR ON");
    psel_1v8_pin = this->get_config("PSEL 1V8");
    psel_3v3_pin = this->get_config("PSEL 3V3");
    psel_5v_pin  = this->get_config("PSEL 5V");
    pullup_en_pin  = this->get_config("Pullup En");

    if (0 != this->gpo_info->num_gpos) {

        for(uint32_t idx=0; idx<this->gpo_info->num_gpos; idx++) {

            MAP_SysCtlPeripheralEnable(this->gpo_info->gpos[idx]->peripheral);

            if ((this->gpo_info->gpos[idx]->port == GPIO_PORTE_AHB_BASE) || (this->gpo_info->gpos[idx]->port == GPIO_PORTF_AHB_BASE) ) {
                SysCtlGPIOAHBEnable(this->gpo_info->gpos[idx]->peripheral);
            }

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
                                 this->gpo_info->gpos[idx]->pad_config);

            if (this->gpo_info->gpos[idx]->pad_config == GPIO_PIN_TYPE_OD) {
                GPIOPinTypeGPIOOutputOD(this->gpo_info->gpos[idx]->port, this->gpo_info->gpos[idx]->pin);
            }
        }

    }

    gpo_obj->set(psel_1v8_pin, 0);
    gpo_obj->set(psel_3v3_pin, 0);
    gpo_obj->set(psel_5v_pin, 0);
    gpo_obj->set(pullup_en_pin, 1);

    sel_power_supply(power_idx);

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


    uint32_t return_val;

    if (config->pad_config ==  GPIO_PIN_TYPE_OD) {

        MAP_GPIODirModeSet(config->port,
                           config->pin,
                           GPIO_DIR_MODE_IN);

        MAP_GPIOPadConfigSet(config->port,
                             config->pin,
                             GPIO_STRENGTH_12MA,
                             GPIO_PIN_TYPE_STD);

        if (0 == MAP_GPIOPinRead(config->port, config->pin)) {
            return_val = 0;
        } else {
            return_val = 1;
        }


        MAP_GPIODirModeSet(config->port,
                           config->pin,
                           config->direction);

        MAP_GPIOPadConfigSet(config->port,
                             config->pin,
                             GPIO_STRENGTH_12MA,
                             config->pad_config);

        MAP_GPIOPinWrite(config->port,
                         config->pin,
                         config->pin);

    } else {

        if (0 == MAP_GPIOPinRead(config->port, config->pin)) {
            return 0;
        } else {
            return 1;
        }

    }

    return return_val;


} // End GpoObj::get

gpos_t* GpoObj::get_struct(void) {
    return this->gpo_info;
} // End GpoObj::get_struct
