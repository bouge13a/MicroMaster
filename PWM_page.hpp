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

class PWMpage : public ConsolePage {
public:
    PWMpage(pwm_pins_t* pwm_info);

private :

    pwm_input_errs_e set_pulse_width(uint32_t duty_cycle, uint32_t period, uint32_t pin_index);

    pwm_pins_t* pwm_info;

    void draw_page(void);
    void draw_data(void);
    void draw_input(int character);
    void draw_reset(void);
    void draw_help(void);
};




#endif /* PWM_PAGE_HPP_ */
