/*
 * I2C_sniffer.cpp
 *
 *  Created on: Sep 7, 2021
 *      Author: steph
 */


#include "I2C_sniffer.hpp"
#include "driverlib/inc/hw_gpio.h"
#include "driverlib/inc/hw_types.h"
#include "driverlib/inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "uartstdio.h"

static GpoObj* gpo_object = NULL;
static gpio_pin_t* scl_pin;
static gpio_pin_t* data_pin;


static void scl_int_handler(void) {

} // End scl_int_handler

static void data_int_handler(void) {

} // End data_int_handler


I2cSniffer::I2cSniffer(GpoObj* gpo_obj) : ConsolePage("Test page",
                                                      portMAX_DELAY,
                                                      false) {

    gpo_object = gpo_obj;

    scl_pin = gpo_obj->get_config("GPI 3");
    data_pin = gpo_obj->get_config("GPI 4");

    GPIOIntDisable(scl_pin->port, scl_pin->pin);
    GPIOIntClear(scl_pin->port, scl_pin->pin);
    GPIOIntRegister(scl_pin->port, scl_int_handler);
    GPIOIntTypeSet(scl_pin->port, scl_pin->pin, GPIO_FALLING_EDGE); // Configure PF4 for falling-edge trigger


    GPIOIntDisable(data_pin->port, data_pin->pin);
    GPIOIntClear(data_pin->port, data_pin->pin);
    GPIOIntRegister(data_pin->port, data_int_handler);
    GPIOIntTypeSet(data_pin->port, data_pin->pin, GPIO_FALLING_EDGE); // Configure PF4 for falling-edge trigger


} // End I2cSniffer


void I2cSniffer::draw_page(void) {

    GPIOIntEnable(scl_pin->port, scl_pin->pin);
    GPIOIntEnable(data_pin->port, data_pin->pin);

}
void I2cSniffer::draw_data(void) {

}
void I2cSniffer::draw_input(int character) {

}

void I2cSniffer::draw_reset(void) {

    GPIOIntDisable(scl_pin->port, scl_pin->pin);
    GPIOIntDisable(data_pin->port, data_pin->pin);

}

void I2cSniffer::draw_help(void) {

    UARTprintf("THIS IS A HELP PAGE\r\n");

}
