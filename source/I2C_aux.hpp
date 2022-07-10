/*
 * I2C_aux.hpp
 *
 *  Created on: Jul 10, 2022
 *      Author: steph
 */

#ifndef SOURCE_I2C_AUX_HPP_
#define SOURCE_I2C_AUX_HPP_

#include "I2C_task.hpp"

#ifdef __cplusplus
extern "C" {
#endif

    class I2cAux {
    public:
        I2cAux(i2c_config_t* config);
        bool add_i2c_msg(I2cMsg* i2c_msg);

    private :
        void task(I2cAux* this_ptr);
        static void taskfunwrapper(void* parm);
        TaskHandle_t task_handle;

        uint32_t nine_clk_count;
        volatile I2cMsg* i2c_msg;

        bool log_errors(I2cAux* this_ptr);

        i2c_config_t* config;
        QueueHandle_t i2c_msg_queue;
        i2c_state_e i2c_state;

        ErrorLogger* logger;

        error_t* addr_ack_err;
        error_t* data_ack_err;
        error_t* arb_lost_err;
        error_t* clk_tout_err;
        error_t* pull_up_err;
    };

#ifdef __cplusplus
}
#endif



#endif /* SOURCE_I2C_AUX_HPP_ */
