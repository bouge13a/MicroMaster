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

static const uint32_t CLK_1_FREQ = 10000;
static const uint32_t CLK_2_FREQ = 5500;
static const uint32_t CLK_4_FREQ = 1250;
static const uint32_t CLK_8_FREQ = 650;
static const uint32_t CLK_16_FREQ = 300;
static const uint32_t CLK_32_FREQ = 100;


static pwm_modes_e pwm_mode = PWM_SIGNAL_MODE;

void set_pwm_mode(uint32_t index) {

    PWMOutputState(PWM0_BASE,
                   PWM_OUT_0_BIT,
                   false);

    switch (index) {
    case 0 :
        pwm_mode = PWM_SIGNAL_MODE;
        break;
    case 1 :
        pwm_mode = PWM_SERVO_MODE;
        break;
    default :
        break;
    }
}

PWMpage::PWMpage(void) : ConsolePage("PWM Module",
                                     portMAX_DELAY,
                                     false) {

    this->pwm_info = &board_pwm_info;

    SysCtlPWMClockSet(SYSCTL_PWMDIV_8);

    for (uint32_t index=0; index<pwm_info->num_pwm_pins; index++) {

        SysCtlPeripheralEnable(pwm_info->pwm_pins[index]->gpio_peripheral);

        while(!SysCtlPeripheralReady(pwm_info->pwm_pins[index]->gpio_peripheral));

        SysCtlPeripheralEnable(pwm_info->pwm_pins[index]->pwm_peripheral);

        while(!SysCtlPeripheralReady(pwm_info->pwm_pins[index]->pwm_peripheral));

        MAP_GPIOPinConfigure(pwm_info->pwm_pins[index]->pwm_pin);
        MAP_GPIOPinTypePWM(pwm_info->pwm_pins[index]->gpio_base, pwm_info->pwm_pins[index]->gpio_pin);

        MAP_PWMGenConfigure(pwm_info->pwm_pins[index]->pwm_base,
                            pwm_info->pwm_pins[index]->pwm_generator,
                            PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);


    }


    this->pwm_cmd_state = ENTER_STATE;
    this->cmd_buffer = new uint8_t[SIZE_OF_CMD_BUFFER];
    this->cmd_buffer_index = 0;

    this->servo_duty_cycle = 10;



} // End PWMpage::PWMpage

void PWMpage::set_pulse_width(uint32_t duty_cycle,
                                          uint32_t period,
                                          uint32_t pin_index,
                                          bool on) {

    uint32_t clock_div = 0;

    PWMGenDisable(pwm_info->pwm_pins[pin_index]->pwm_base,
                  pwm_info->pwm_pins[pin_index]->pwm_generator);

    if (period > CLK_1_FREQ) {
        SysCtlPWMClockSet(SYSCTL_PWMDIV_1);
        clock_div = 1;
    } else if(period > CLK_2_FREQ) {
        SysCtlPWMClockSet(SYSCTL_PWMDIV_2);
        clock_div = 2;
    }  else if(period > CLK_4_FREQ) {
        SysCtlPWMClockSet(SYSCTL_PWMDIV_4);
        clock_div = 4;
    }  else if(period > CLK_8_FREQ) {
        SysCtlPWMClockSet(SYSCTL_PWMDIV_8);
        clock_div = 8;
    } else if(period > CLK_16_FREQ){
        SysCtlPWMClockSet(SYSCTL_PWMDIV_16);
        clock_div = 16;
    } else if(period > CLK_32_FREQ){
        SysCtlPWMClockSet(SYSCTL_PWMDIV_32);
        clock_div = 32;
    } else {
        SysCtlPWMClockSet(SYSCTL_PWMDIV_64);
        clock_div = 64;
    }

    period = (80000000/clock_div)/period;

    PWMGenPeriodSet(pwm_info->pwm_pins[pin_index]->pwm_base,
                    pwm_info->pwm_pins[pin_index]->pwm_generator,
                    period);


    PWMPulseWidthSet(pwm_info->pwm_pins[pin_index]->pwm_base,
                     pwm_info->pwm_pins[pin_index]->pwm_out,
                     (uint32_t)((duty_cycle/100.0)*period));

    PWMOutputState(pwm_info->pwm_pins[pin_index]->pwm_base,
                   pwm_info->pwm_pins[pin_index]->pwm_out_bit,
                   on);

    if (on) {
        PWMGenEnable(pwm_info->pwm_pins[pin_index]->pwm_base,
                     pwm_info->pwm_pins[pin_index]->pwm_generator);
    }


} // End PWMpage::set_pulse_width


void PWMpage::draw_page(void) {

    if (pwm_mode == PWM_SIGNAL_MODE) {

        TextCtl::cursor_pos(START_ROW, 0);
        UARTprintf("Turn on (y/n) : ");
    } else if (pwm_mode == PWM_SERVO_MODE) {

        UARTprintf("Use left and right arrow keys to move servo");
        this->draw_servo_duty(10);
    }

} // End PWMpage::draw_page

void PWMpage::draw_servo_duty(uint32_t angle) {
    TextCtl::cursor_pos(7, 50);
    TextCtl::clear_line();
    UARTprintf("%d %%", angle);
}

void PWMpage::draw_data(void) {



} // End PWMpage::draw_data

void PWMpage::draw_input(int character) {

    if (pwm_mode == PWM_SIGNAL_MODE) {

        if (character == 127) {
            if(this->cmd_buffer_index > 0) {
                this->cmd_buffer_index--;
                UARTprintf("\b");
                TextCtl::clear_in_line();
                return;
            }
        }

        switch(this->pwm_cmd_state) {
        case ENTER_DUTY_CYCLE :

            if ((character >= '0') && (character <= '9')) {

                this->cmd_buffer[this->cmd_buffer_index] = (uint8_t)character;
                this->cmd_buffer_index++;
                UARTprintf("%c", (uint8_t)character);
            } else if (character == '\r') {
                UARTprintf("%c\r\n", (uint8_t)character);
                this->cmd_buffer[this->cmd_buffer_index] = '\0';
                this->duty_cycle_buffer = atoi((const char*)this->cmd_buffer);
                if (this->duty_cycle_buffer > 99 || this->duty_cycle_buffer < 1) {
                    UARTprintf("\r\nError: Duty cycle must be between 1-99");
                    UARTprintf("\r\nEnter duty cycle (0-100) : ");
                    this->cmd_buffer_index = 0;
                    break;
                }
                this->cmd_buffer_index = 0;
                this->pwm_cmd_state = ENTER_FREQUENCY;
                UARTprintf("Enter frequency (Hz) : ");
            }

            break;
        case ENTER_FREQUENCY :

            if ((character >= '0') && (character <= '9')) {

                this->cmd_buffer[this->cmd_buffer_index] = (uint8_t)character;
                this->cmd_buffer_index++;
                UARTprintf("%c", (uint8_t)character);
            } else if (character == '\r') {
                UARTprintf("%c", (uint8_t)character);
                this->cmd_buffer[this->cmd_buffer_index] = '\0';
                this->period_buffer = atoi((const char*)this->cmd_buffer);

                if (this->period_buffer > 40000000 || this->period_buffer < 20) {
                    UARTprintf("\r\nError: Frequency must be between 20Hz and 40MHz");
                    UARTprintf("\r\n\nEnter Frequency (Hz) : ");
                    this->cmd_buffer_index = 0;
                    break;
                }

                set_pulse_width(this->duty_cycle_buffer,
                                this->period_buffer,
                                this->pin_buffer,
                                true);


                this->cmd_buffer_index = 0;

                this->pwm_cmd_state = ENTER_STATE;
                UARTprintf("\r\n\nTurn on (y/n) : ");

            }

            break;
        case ENTER_STATE :

            if ('y' == character || 'Y' == character) {

                UARTprintf("%c\r\n", character);
                UARTprintf("Enter duty cycle (0-100) : ");
                this->pwm_cmd_state = ENTER_DUTY_CYCLE;

            } else if ('n' == character || 'N' == character) {

                set_pulse_width(this->duty_cycle_buffer,
                                this->period_buffer,
                                this->pin_buffer,
                                false);

                UARTprintf("%c\r\n", character);
                UARTprintf("Turn on (y/n) : ");

            }

            break;
        default :
            break;
        }

    } else if (pwm_mode == PWM_SERVO_MODE) {

        switch (character) {
        case ArrowKeys::LEFT :

            if (this->servo_duty_cycle <= 10) {
                this->servo_duty_cycle = 50;
            } else {
                this->servo_duty_cycle--;
            }

            this->draw_servo_duty(this->servo_duty_cycle);

            set_pulse_width(this->servo_duty_cycle,
                            200,
                            0,
                            true);
            break;
        case ArrowKeys::RIGHT :


            if (this->servo_duty_cycle >= 50) {
                this->servo_duty_cycle = 10;
            } else {
                this->servo_duty_cycle++;
            }

            this->draw_servo_duty(this->servo_duty_cycle);

            set_pulse_width(this->servo_duty_cycle,
                            200,
                            0,
                            true);

            break;
        default :
            break;
        }

    }

} // End PWMpage::draw_input

void PWMpage::draw_reset(void) {

    if (pwm_mode == PWM_SIGNAL_MODE) {
        this->cmd_buffer_index = 0;
        this->pwm_cmd_state = ENTER_STATE;

    }  else if (pwm_mode == PWM_SERVO_MODE) {


    }


} // End PWMpage::draw_reset

void PWMpage::draw_help(void) {


} // End PWMpage::draw_help
