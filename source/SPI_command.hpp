/*
 * SPI_command.hpp
 *
 *  Created on: Aug 9, 2021
 *      Author: steph
 */

#ifndef SPI_COMMAND_HPP_
#define SPI_COMMAND_HPP_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "error_logger.hpp"

#include <vector>

#include "console_task.hpp"

typedef enum {
    SPI_IDLE,
    SPI_SEND,
    SPI_RECEIVE,
    SPI_FINISH,
}spi_state_e;

typedef enum {
    spi_processing,
    spi_finished,
    spi_ready,
}spi_msg_state_e;

typedef enum {
    SPI_NO_ERRORS,
    SPI_OVERRUN_ERR,
    SPI_TIMEOUT_ERR,
}spi_errors_e;

typedef enum {
    spi_command_msg,
    spi_normal_msg,
}spi_msg_type_e;

typedef enum {
    SPI_GET_MONITOR_STATUS,
    SPI_GET_SPEED,
    SPI_GET_DATA_WIDTH,
    SPI_GET_NUM_TX_BYTES,
    SPI_TX_BYTES,
    SPI_GET_NUM_RX_BYTES,
    SPI_SEND_MSG,
}spi_cmd_state_e;

class SpiMsg {
public:
    SpiMsg(spi_msg_type_e msg_type);
    uint32_t num_tx_bytes;
    uint32_t num_rx_bytes;
    uint32_t bytes_rxed;
    uint32_t bytes_txed;
    uint32_t* tx_bytes;
    uint32_t* rx_bytes;
    spi_msg_state_e state;
    spi_errors_e errors;
    spi_msg_type_e msg_type;
    bool monitored;
    bool active;
    uint32_t speed;
    uint32_t data_width;
};

const uint32_t NUM_OF_SPI_MONITORED_MSGS = 5;


void spi_set_mode(uint32_t mode);

#ifdef __cplusplus
extern "C" {
#endif

    class SpiCmdTask : public ConsolePage {
    public:
        SpiCmdTask(void);
        bool add_spi_msg(SpiMsg* spi_msg);
        void reset_monitor_index(void);
        std::vector<SpiMsg*>* get_vector(void);
        uint32_t get_speed(void);
        uint32_t get_mode(void);
        uint32_t get_data_width(void);

    private :
        void task(SpiCmdTask* this_ptr);
        static void taskfunwrapper(void* parm);
        TaskHandle_t task_handle;
        uint32_t test;

        bool log_errors(SpiCmdTask* this_ptr);
        void print_errors(SpiCmdTask* this_ptr);

        QueueHandle_t spi_tx_queue;
        SpiMsg* spi_msg;
        uint8_t* tx_bytes;
        uint8_t* rx_bytes;
        spi_state_e state;
        spi_cmd_state_e cmd_state;

        uint32_t byte_buffer;
        uint32_t byte_buffer_index;
        uint32_t byte_counter;
        uint32_t spi_monitor_index;
        bool monitored;

        uint8_t* cmd_buffer;

        SpiMsg* spi_cmd_msg;
        std::vector<SpiMsg*> spi_monitor_msgs;

        ErrorLogger* logger;
        error_t* rx_timeout_err;
        error_t* rx_overrun_err;

        void draw_page(void);
        void draw_data(void);
        void draw_input(int character);
        void draw_help(void);
        void draw_reset(void);
    };

#ifdef __cplusplus
}
#endif





#endif /* SPI_COMMAND_HPP_ */
