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
    ENTER_PIN,
    ENTER_DUTY_CYCLE,
    ENTER_PERIOD,
    ENTER_STATE,
}pwm_cmd_states_e;

class PWMpage : public ConsolePage {
public:
    PWMpage(void);

private :

    void set_pulse_width(uint32_t duty_cycle,
                         uint32_t period,
                         uint32_t pin_index,
                         bool on);

    pwm_pins_t* pwm_info;
    pwm_cmd_states_e pwm_cmd_state;

    uint8_t* cmd_buffer;
    uint32_t cmd_buffer_index;

    uint32_t pin_buffer;
    uint32_t duty_cycle_buffer;
    uint32_t period_buffer;

    void draw_page(void);
    void draw_data(void);
    void draw_input(int character);
    void draw_reset(void);
    void draw_help(void);
};




#endif /* PWM_PAGE_HPP_ */
