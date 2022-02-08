/*
 * PWM_page.hpp
 *
 *  Created on: Aug 4, 2021
 *      Author: steph
 */

#ifndef PWM_PAGE_HPP_
#define PWM_PAGE_HPP_

#include <stdint.h>
#include "console_task.hpp"
#include "board_pin_defs.hpp"

typedef enum {
    NO_ERR,
    PERIOD_ERR,
    DUTY_CYCLE_ERR,
    PHASE_ERR,
}pwm_input_errs_e;

typedef enum {
    ENTER_DUTY_CYCLE,
    ENTER_FREQUENCY,
    ENTER_STATE,
}pwm_cmd_states_e;

typedef enum {
    PWM_SIGNAL_MODE,
    PWM_SERVO_MODE,
}pwm_modes_e;

void set_pwm_mode(uint32_t index);

class PWMpage : public ConsolePage {
public:
    PWMpage(void);

private :

    void set_pulse_width(uint32_t duty_cycle,
                         uint32_t period,
                         uint32_t pin_index,
                         bool on);

    void print_menu(uint32_t index);

    void draw_servo_duty(uint32_t angle);

    pwm_pins_t* pwm_info;
    pwm_cmd_states_e pwm_cmd_state;

    uint8_t* cmd_buffer;
    uint32_t cmd_buffer_index;

    volatile uint32_t pin_buffer;
    volatile uint32_t duty_cycle_buffer;
    volatile uint32_t period_buffer;

    int32_t servo_duty_cycle;


    void draw_page(void);
    void draw_data(void);
    void draw_input(int character);
    void draw_reset(void);
    void draw_help(void);
};




#endif /* PWM_PAGE_HPP_ */
