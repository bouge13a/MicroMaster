/*
 * PWM_page.cpp
 *
 *  Created on: Aug 4, 2021
 *      Author: steph
 */

#include "PWM_page.hpp"
#include "text_controls.hpp"

#include <stdlib.h>

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "uartstdio.h"
#include "pins_PWM.hpp"

static const uint32_t START_ROW = 5;
static const uint32_t SIZE_OF_CMD_BUFFER = 10;


PWMpage::PWMpage(void) : ConsolePage("PWM Module",
                                     portMAX_DELAY,
                                     false) {

    this->pwm_info = &board_pwm_info;

    for (uint32_t index=0; index<pwm_info->num_pwm_pins; index++) {

        SysCtlPeripheralEnable(pwm_info->pwm_pins[index]->gpio_peripheral);

        while(!SysCtlPeripheralReady(pwm_info->pwm_pins[index]->gpio_peripheral));

        SysCtlPeripheralEnable(pwm_info->pwm_pins[index]->pwm_peripheral);

        while(!SysCtlPeripheralReady(pwm_info->pwm_pins[index]->pwm_peripheral));

        MAP_GPIOPinConfigure(pwm_info->pwm_pins[index]->pwm_pin);
        MAP_GPIOPinTypePWM(pwm_info->pwm_pins[index]->gpio_base, pwm_info->pwm_pins[index]->gpio_pin);

        MAP_PWMGenConfigure(pwm_info->pwm_pins[index]->pwm_base,
                            pwm_info->pwm_pins[index]->pwm_generator,
                            PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);


    }

    this->pwm_cmd_state = ENTER_PIN;
    this->cmd_buffer = new uint8_t[SIZE_OF_CMD_BUFFER];
    this->cmd_buffer_index = 0;

} // End PWMpage::PWMpage

void PWMpage::set_pulse_width(uint32_t duty_cycle,
                                          uint32_t period,
                                          uint32_t pin_index,
                                          bool on) {

    PWMGenDisable(pwm_info->pwm_pins[pin_index]->pwm_base,
                  pwm_info->pwm_pins[pin_index]->pwm_generator);

    PWMGenPeriodSet(pwm_info->pwm_pins[pin_index]->pwm_base,
                    pwm_info->pwm_pins[pin_index]->pwm_generator,
                    period);


    PWMPulseWidthSet(pwm_info->pwm_pins[pin_index]->pwm_base,
                     pwm_info->pwm_pins[pin_index]->pwm_out,
                     period*(duty_cycle/100.0));

    PWMOutputState(pwm_info->pwm_pins[pin_index]->pwm_base,
                   pwm_info->pwm_pins[pin_index]->pwm_out_bit,
                   true);

    if (on) {
        PWMGenEnable(pwm_info->pwm_pins[pin_index]->pwm_base,
                     pwm_info->pwm_pins[pin_index]->pwm_generator);
    }


} // End PWMpage::set_pulse_width


void PWMpage::draw_page(void) {

    TextCtl::cursor_pos(START_ROW, 0);
    UARTprintf("Enter pin to control: ");

} // End PWMpage::draw_page

void PWMpage::draw_data(void) {



} // End PWMpage::draw_data

void PWMpage::draw_input(int character) {

    switch(this->pwm_cmd_state) {
    case ENTER_PIN :

        pin_buffer = atoi((const char*)&character);
        if (pin_buffer == 0 || pin_buffer > this->pwm_info->num_pwm_pins) {
            break;
        }
        this->pin_buffer--;
        this->pwm_cmd_state = ENTER_DUTY_CYCLE;
        UARTprintf("%c\r\n", character);
        UARTprintf("Enter Duty Cycle: ");

        break;
    case ENTER_DUTY_CYCLE :

        if ((character >= '0') && (character <= '9')) {

            this->cmd_buffer[this->cmd_buffer_index] = (uint8_t)character;
            this->cmd_buffer_index++;
            UARTprintf("%c", (uint8_t)character);
        } else if (character == '\r') {
            UARTprintf("%c\r\n", (uint8_t)character);
            this->cmd_buffer[this->cmd_buffer_index] = '\0';
            this->duty_cycle_buffer = atoi((const char*)this->cmd_buffer);
            this->cmd_buffer_index = 0;
            this->pwm_cmd_state = ENTER_PERIOD;
            UARTprintf("Enter period : ");
        }

        break;
    case ENTER_PERIOD :

        if ((character >= '0') && (character <= '9')) {

            this->cmd_buffer[this->cmd_buffer_index] = (uint8_t)character;
            this->cmd_buffer_index++;
            UARTprintf("%c", (uint8_t)character);
        } else if (character == '\r') {
            UARTprintf("%c", (uint8_t)character);
            this->cmd_buffer[this->cmd_buffer_index] = '\0';
            this->period_buffer = atoi((const char*)this->cmd_buffer);
            this->cmd_buffer_index = 0;
            this->pwm_cmd_state = ENTER_STATE;
            UARTprintf("\r\nTurn on (y/n) : ");

        }

        break;
    case ENTER_STATE :

        if ('y' == character || 'Y' == character) {
            set_pulse_width(this->duty_cycle_buffer,
                            this->period_buffer,
                            this->pin_buffer,
                            true);

            TextCtl::cursor_pos(START_ROW, 0);
            TextCtl::clear_below_line();
            UARTprintf("Enter pin to control: ");
            this->pwm_cmd_state = ENTER_PIN;

        } else if ('n' == character || 'N' == character) {

            set_pulse_width(this->duty_cycle_buffer,
                            this->period_buffer,
                            this->pin_buffer,
                            false);

            TextCtl::cursor_pos(START_ROW, 0);
            TextCtl::clear_below_line();
            UARTprintf("Enter pin to control: ");
            this->pwm_cmd_state = ENTER_PIN;
        }

        break;
    default :
        break;
    }

} // End PWMpage::draw_input

void PWMpage::draw_reset(void) {


} // End PWMpage::draw_reset

void PWMpage::draw_help(void) {


} // End PWMpage::draw_help
