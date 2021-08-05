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

typedef enum {
    HEX_FORMAT,
    BIN_FORMAT,
    MSB_DEC_FORMAT,
    LSB_DEC_FORMAT,
    NUM_OF_FORMATS,
}msg_format_e;

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
        msg_format_e* message_format;
        uint32_t format_index;

        void print_format_row(uint32_t row);
        bool print_data(uint32_t inner_index, uint32_t index);

        void draw_page(void);
        void draw_data(void);
        void draw_input(int character);
        void draw_help(void);
        void draw_reset(void);
    };

#ifdef __cplusplus
}
#endif




#endif /* I2C_MONITOR_HPP_ */
