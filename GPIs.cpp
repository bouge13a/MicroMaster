/*
 * GPIs.cpp
 *
 *  Created on: Aug 3, 2021
 *      Author: steph
 */

#include "GPIs.hpp"

#include <string.h>
#include <assert.h>
#include "GPIs.hpp"
#include "pins_GPI.hpp"

#include "driverlib/inc/hw_gpio.h"
#include "driverlib/inc/hw_types.h"
#include "driverlib/inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"

GpiObj::GpiObj(void) {

    this->gpi_info = &board_gpi_info;

    if (0 !=this->gpi_info->num_gpis) {

        for(uint32_t idx=0; idx<this->gpi_info->num_gpis; idx++) {

            MAP_SysCtlPeripheralEnable(gpi_info->gpis[idx]->peripheral);

            if ((this->gpi_info->gpis[idx]->port == GPIO_PORTE_AHB_BASE) || (this->gpi_info->gpis[idx]->port == GPIO_PORTF_AHB_BASE)) {
                SysCtlGPIOAHBEnable(this->gpi_info->gpis[idx]->peripheral);
            }

            while(!SysCtlPeripheralReady(this->gpi_info->gpis[idx]->peripheral));

            // Unlock port so we can change it to a GPIO input
            // Once we have enabled (unlocked) the commit register then re-lock it
            // to prevent further changes.  PF0 is muxed with NMI thus a special case.
            HWREG(this->gpi_info->gpis[idx]->port + GPIO_O_LOCK) = GPIO_LOCK_KEY;
            HWREG(this->gpi_info->gpis[idx]->port + GPIO_O_CR) |= gpi_info->gpis[idx]->pin;
            HWREG(this->gpi_info->gpis[idx]->port + GPIO_O_LOCK) = 0;

            MAP_GPIODirModeSet(this->gpi_info->gpis[idx]->port,
                               this->gpi_info->gpis[idx]->pin,
                               this->gpi_info->gpis[idx]->direction);

            MAP_GPIOPadConfigSet(this->gpi_info->gpis[idx]->port,
                                 this->gpi_info->gpis[idx]->pin,
                                 GPIO_STRENGTH_2MA,
                                 GPIO_PIN_TYPE_STD);
        }
    }

} // End GpiObj::GpiObj

gpio_pin_t* GpiObj::get_config(const char* name) {

    int idx;

    for(idx=0; idx<this->gpi_info->num_gpis; idx++) {

        if(0 == strcmp(name, this->gpi_info->gpis[idx]->name)){
            return this->gpi_info->gpis[idx];
        }

    }

    assert(0);
    return 0;

} // End GpoObj::get_config


uint32_t GpiObj::get(gpio_pin_t* config) {

    if (0 == MAP_GPIOPinRead(config->port, config->pin)) {
        return 0;
    } else {
        return 1;
    }

} // End GpoObj::get

gpis_t* GpiObj::get_struct(void) {
    return this->gpi_info;
} // End GpoObj::get_struct
