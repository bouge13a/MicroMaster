/*
 * I2C_typedefs.hpp
 *
 *  Created on: Jul 17, 2022
 *      Author: steph
 */

#ifndef SOURCE_I2C_TYPEDEFS_HPP_
#define SOURCE_I2C_TYPEDEFS_HPP_

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
    I2C_PRINT,
    I2C_NINE_CLOCK,
}i2c_state_e;

typedef enum {
    i2c_ready,
    i2c_processing,
    i2c_monitored,
    i2c_finished,
}i2c_msg_state_e;

typedef enum {
    search_msg,
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



#endif /* SOURCE_I2C_TYPEDEFS_HPP_ */
