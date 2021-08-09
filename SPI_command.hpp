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

#include "console_task.hpp"

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
        QueueHandle_t spi_rx_queue;

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
