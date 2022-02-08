/*
 * ADC_task.hpp
 *
 *  Created on: Aug 3, 2021
 *      Author: steph
 */

#ifndef ADC_TASK_HPP_
#define ADC_TASK_HPP_

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include <stdint.h>
#include "board_pin_defs.hpp"


class AdcTask {
public:
    AdcTask(void);
    adc_pin_t* get_adc_config(const char* name);
    uint32_t get_adc_val(adc_pin_t* config, AdcTask* this_ptr);
    adc_pins_t* get_adc_struct(void);
private :
    void task(AdcTask* this_ptr);
    static void taskfunwrapper(void* parm);
    TaskHandle_t task_handle;

    uint32_t adc00_step_values[8];

};


#endif /* ADC_TASK_HPP_ */
