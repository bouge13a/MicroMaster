/*
 * I2C_monitor.hpp
 *
 *  Created on: Jul 30, 2021
 *      Author: steph
 */

#ifndef I2C_MONITOR_HPP_
#define I2C_MONITOR_HPP_

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <vector>


#include "console_task.hpp"
#include "I2C_task.hpp"

#ifdef __cplusplus
extern "C" {
#endif

    class I2cMonitorTask : public ConsolePage {
    public:
        I2cMonitorTask(I2cTask* i2c0, std::vector<I2cMsg*>& i2c_monitor_msgs);
    private :
        void task(I2cMonitorTask* this_ptr);
        static void taskfunwrapper(void* parm);
        TaskHandle_t task_handle;
        uint32_t test;

        I2cTask* i2c0;

        std::vector<I2cMsg*> i2c_monitor_msgs;

        void draw_page(void);
        void draw_data(void);
        void draw_input(int character);
    };

#ifdef __cplusplus
}
#endif




#endif /* I2C_MONITOR_HPP_ */
