/*
 * I2C_aux.hpp
 *
 *  Created on: Jul 10, 2022
 *      Author: steph
 */

#ifndef SOURCE_I2C_AUX_HPP_
#define SOURCE_I2C_AUX_HPP_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "I2C_task.hpp"

class I2cMsgAux {
public :
    I2cMsgAux(void);
    uint8_t address;
    uint8_t* tx_data;
    uint32_t num_tx_bytes;
    volatile uint8_t* rx_data;
    uint32_t num_rx_bytes;
    i2c_msg_state_e state;
    i2c_errors_e errors;
    SemaphoreHandle_t semphr;
};

#ifdef __cplusplus
extern "C" {
#endif

    class I2cAux {
    public:
        I2cAux(i2c_config_t* config);
        bool add_i2c_msg(I2cMsgAux* i2c_msg);

    private :
        void task(I2cAux* this_ptr);
        static void taskfunwrapper(void* parm);
        TaskHandle_t task_handle;

        uint32_t nine_clk_count;

        bool log_errors(I2cAux* this_ptr);

        I2cMsgAux* i2c_msg;

        i2c_config_t* config;
        QueueHandle_t i2c_msg_queue;
        i2c_state_e i2c_state;

        ErrorLogger* logger;

        uint32_t bytes_rxed;
        uint32_t bytes_txed;

        error_t* addr_ack_err;
        error_t* data_ack_err;
        error_t* arb_lost_err;
        error_t* clk_tout_err;
        error_t* pull_up_err;
    };

#ifdef __cplusplus
}
#endif

typedef struct {
    SemaphoreHandle_t i2c_sem;
    I2cAux* i2c;
}display_tools_t;

#endif /* SOURCE_I2C_AUX_HPP_ */
