/*
 * current_monitor_task.hpp
 *
 *  Created on: Jul 16, 2022
 *      Author: steph
 */

#ifndef CURRENT_MONITOR_TASK_HPP_
#define CURRENT_MONITOR_TASK_HPP_


#include "console_task.hpp"
#include "I2C_task.hpp"
#include "I2C_aux.hpp"

#ifdef __cplusplus
extern "C" {
#endif

    class CurrentMonitorTask : public ConsolePage {
    public:
        CurrentMonitorTask(I2cAux* i2c);
    private :
        void task(CurrentMonitorTask* this_ptr);
        static void taskfunwrapper(void* parm);

        I2cAux* i2c;
        I2cMsgAux* calibration_msg;
        I2cMsgAux* config_msg;
        I2cMsgAux* current_msg;

        uint8_t raw_current[2];

        void draw_page(void);
        void draw_data(void);
        void draw_input(int character);
        void draw_help(void);
        void draw_reset(void);
    };

#ifdef __cplusplus
}
#endif


#endif /* CURRENT_MONITOR_TASK_HPP_ */