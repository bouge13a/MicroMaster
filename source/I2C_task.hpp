/*
 * I2C_task.hpp
 *
 *  Created on: Jul 27, 2021
 *      Author: steph
 */

#ifndef I2C_TASK_HPP_
#define I2C_TASK_HPP_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "console_task.hpp"
#include "board_pin_defs.hpp"
#include "error_logger.hpp"

#include <stdint.h>
#include <stdbool.h>
#include <vector>
#include "I2C_typedefs.hpp"

const uint32_t NUM_OF_MONITORED_MSGS = 5;

class I2cMsg {
public :
    I2cMsg(i2c_msg_type_t type);
    uint8_t address;
    uint8_t* tx_data;
    uint32_t num_tx_bytes;
    uint32_t bytes_txed;
    volatile uint8_t* rx_data;
    uint32_t num_rx_bytes;
    uint32_t bytes_rxed;
    i2c_msg_state_e state;
    uint32_t tx_time;
    i2c_msg_type_t type;
    i2c_errors_e errors;
    bool monitored;
    bool active;
};

///////////////////////////////////////////////////
//              Menu Functions
////////////////////////////////////////////////////

void set_i2c_clock_speed(uint32_t index);

/////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

    class I2cTask : public ConsolePage {
    public:
        I2cTask(i2c_config_t* config);
        std::vector<I2cMsg*>* get_vector(void);
        bool add_i2c_msg(I2cMsg* i2c_msg);
        void reset_monitor_index(void);

    private :
        void task(I2cTask* this_ptr);
        static void taskfunwrapper(void* parm);
        TaskHandle_t task_handle;

        void set_timer(uint32_t useconds);

        uint32_t nine_clk_count;

        bool log_errors(I2cTask* this_ptr);
        void print_errors(I2cTask* this_ptr);

        i2c_config_t* config;
        QueueHandle_t i2c_msg_queue;
        i2c_state_e i2c_state;
        i2c_cmd_state_t i2c_cmd_state;
        uint8_t byte_buffer;
        uint32_t byte_buffer_index;
        uint32_t byte_counter;

        volatile I2cMsg* i2c_msg;
        I2cMsg* i2c_cmd_msg;
        uint32_t i2c_monitor_index;
        bool monitored;

        std::vector<I2cMsg*> i2c_monitor_msgs;

        ErrorLogger* logger;

        error_t* addr_ack_err;
        error_t* data_ack_err;
        error_t* arb_lost_err;
        error_t* clk_tout_err;
        error_t* pull_up_err;

        void draw_page(void);
        void draw_data(void);
        void draw_input(int character);
        void draw_reset(void);
        void draw_help(void);
    };

#ifdef __cplusplus
}
#endif



#endif /* I2C_TASK_HPP_ */
