/*
 * I2C_scripter.hpp
 *
 *  Created on: Aug 2, 2021
 *      Author: steph
 */

#ifndef I2C_SCRIPTER_HPP_
#define I2C_SCRIPTER_HPP_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "I2C_task.hpp"
#include "console_task.hpp"

typedef enum {
    START_SCRIPTER,
    GET_NIBBLE_0,
    GET_NIBBLE_1,
    GET_SPACE_BAR,
}buffer_state_e;

#ifdef __cplusplus
extern "C" {
#endif

    class I2cScripterTask : public ConsolePage {
    public:
        I2cScripterTask(I2cTask* i2c);
    private :
        void task(I2cScripterTask* this_ptr);
        static void taskfunwrapper(void* parm);
        TaskHandle_t task_handle;

        bool ascii_to_hex(char* character);
        void print_message(I2cMsg* i2c_msg);

        I2cTask* i2c;
        I2cMsg* i2c_msg;
        uint8_t* i2c_tx_data;

        uint32_t buffer_idx;
        buffer_state_e buffer_state;

        SemaphoreHandle_t send_semphr;

        void draw_page(void);
        void draw_data(void);
        void draw_input(int character);
    };

#ifdef __cplusplus
}
#endif

#endif /* I2C_SCRIPTER_HPP_ */
