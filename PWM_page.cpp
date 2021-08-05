/*
 * PWM_page.cpp
 *
 *  Created on: Aug 4, 2021
 *      Author: steph
 */

#include "PWM_page.hpp"

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

PWMpage::PWMpage(pwm_pins_t* pwm_info) : ConsolePage("PWM Module",
                                                     portMAX_DELAY,
                                                     false) {

    this->pwm_info = pwm_info;

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

} // End PWMpage::PWMpage

pwm_input_errs_e PWMpage::set_pulse_width(uint32_t duty_cycle, uint32_t period, uint32_t pin_index) {

    if (duty_cycle >= 100 || period <= 0) {
        return DUTY_CYCLE_ERR;
    }

    if (period > SysCtlClockGet()) {
        return PERIOD_ERR;
    }

    PWMGenDisable(pwm_info->pwm_pins[pin_index]->pwm_base,
                  pwm_info->pwm_pins[pin_index]->pwm_generator);

    PWMGenConfigure(pwm_info->pwm_pins[pin_index]->pwm_base,
                    pwm_info->pwm_pins[pin_index]->pwm_generator,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    PWMGenPeriodSet(PWM0_BASE,
                    PWM_GEN_0,
                    period);


    PWMPulseWidthSet(pwm_info->pwm_pins[pin_index]->pwm_base,
                     pwm_info->pwm_pins[pin_index]->pwm_out,
                     PWMGenPeriodGet(pwm_info->pwm_pins[pin_index]->pwm_base,
                                     pwm_info->pwm_pins[pin_index]->pwm_generator));

    PWMOutputState(pwm_info->pwm_pins[pin_index]->pwm_base,
                   pwm_info->pwm_pins[pin_index]->pwm_out_bit,
                   true);

    PWMGenEnable(pwm_info->pwm_pins[pin_index]->pwm_base,
                 pwm_info->pwm_pins[pin_index]->pwm_generator);

    return NO_ERR;

} // End PWMpage::set_pulse_width


void PWMpage::draw_page(void) {

} // End PWMpage::draw_page

void PWMpage::draw_data(void) {

} // End PWMpage::draw_data

void PWMpage::draw_input(int character) {

} // End PWMpage::draw_input
