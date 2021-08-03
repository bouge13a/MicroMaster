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

typedef enum {
    RESTART_CMD,
    GET_ADDRESS,
    GET_NUM_TX_BYTES,
    GET_TX_BYTES,
    GET_NUM_RX_BYTES,
    SEND_I2C_MSG,
    GET_MONITOR_STATUS,
}i2c_cmd_state_t;

typedef enum {
    I2C_IDLE,
    I2C_SEND,
    I2C_RECEIVE_START,
    I2C_RECEIVE,
    I2C_FINISH,
    I2C_ERROR,
}i2c_state_e;

typedef enum {
    i2c_ready,
    i2c_processing,
    i2c_monitored,
    i2c_finished,
}i2c_msg_state_e;

typedef enum {
    command_msg,
    normal_msg,
}i2c_msg_type_t;

typedef enum {
    NONE,
    ADDR_NACK_ERR,
    DATA_NACK_ERR,
    ARB_LOST_ERR,
    TIMEOUT_ERR,
    PULL_UP_ERR,
}i2c_errors_e;

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


        bool log_errors(I2cTask* this_ptr);
        void print_errors(I2cTask* this_ptr);

        i2c_config_t* config;
        QueueHandle_t i2c_msg_queue;
        i2c_state_e i2c_state;
        i2c_cmd_state_t i2c_cmd_state;
        char byte_buffer[2]; // This doesn't need to be a two byte buffer
        uint32_t byte_buffer_index;
        uint32_t byte_counter;

        volatile I2cMsg* i2c_msg;
        I2cMsg* i2c_cmd_msg;
        uint32_t i2c_monitor_index;
        uint32_t tx_byte_index;
        bool monitored;

        std::vector<I2cMsg*> i2c_monitor_msgs;

        ErrorLogger* logger;

        error_t* addr_ack_err;
        error_t* data_ack_err;
        error_t* arb_lost_err;
        error_t* clk_tout_err;

        void draw_page(void);
        void draw_data(void);
        void draw_input(int character);
    };

#ifdef __cplusplus
}
#endif



#endif /* I2C_TASK_HPP_ */
