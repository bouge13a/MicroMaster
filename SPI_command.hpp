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

class SpiMsg {
public:
    SpiMsg(void);
    uint32_t num_tx_bytes;
    uint32_t num_rx_bytes;
    uint32_t bytes_rxed;
    uint32_t bytes_txed;
    uint32_t* tx_bytes;
    uint32_t* rx_bytes;
    spi_msg_state_e state;
};

#ifdef __cplusplus
extern "C" {
#endif

    class SpiCmdTask : public ConsolePage {
    public:
        SpiCmdTask(void);
    private :
        void task(SpiCmdTask* this_ptr);
        static void taskfunwrapper(void* parm);
        TaskHandle_t task_handle;
        uint32_t test;

        QueueHandle_t spi_tx_queue;
        SpiMsg* spi_msg;
        uint8_t* tx_bytes;
        uint8_t* rx_bytes;
        spi_state_e state;

        error_t* rx_timeout_err;
        errot_t* rx_overrun_err;

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
