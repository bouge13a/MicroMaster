/*
 * I2C_sniffer.hpp
 *
 *  Created on: Aug 2, 2021
 *      Author: steph
 */

#ifndef I2C_SEARCH_HPP_
#define I2C_SEARCH_HPP_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "I2C_task.hpp"
#include "console_task.hpp"

#ifdef __cplusplus
extern "C" {
#endif

    class I2cSnifferTask : public ConsolePage {
    public:
        I2cSnifferTask(I2cTask* i2c);
    private :
        void task(I2cSnifferTask* this_ptr);
        static void taskfunwrapper(void* parm);
        TaskHandle_t task_handle;

        I2cTask* i2c;
        I2cMsg* i2c_msg;
        uint8_t* i2c_tx_data;

        SemaphoreHandle_t start_semphr;

        void draw_page(void);
        void draw_data(void);
        void draw_input(int character);
        void draw_reset(void);
        void draw_help(void);
    };

#ifdef __cplusplus
}
#endif




#endif /* I2C_SEARCH_HPP_ */
